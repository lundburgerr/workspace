/*****************************************************************************
 *	\file		true_voice_internal.h
 *
 *	\date		2013-May-27
 *
 *	\brief		Header for internal use in TrueVoice
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef TRUE_VOICE_INTERNAL_H
#define TRUE_VOICE_INTERNAL_H

// ---< Include >---
#include "true_voice.h"	// TrueVoice API
#include "fract_math.h"	// Fractional arithmetic
#include "timers.h"
#include "dumpdata.h"	// should not be included here
#include <stdio.h>		// Printf
#include <string.h> 	// memset
#include <math.h> 		// powf and other

// ---< Defines >---
// Enable debug printing
#define ENABLE_PRINT_DEBUG 1
#define USE_DUMPDATA

// In file noise_reduction.c
//Number of bins to be used in FFT
#define NFFT 64//Must be twice minimally twice as big as BUFLEN  2*(((((((BUFLEN_8KHZ-1)|(BUFLEN_8KHZ>>1))|(BUFLEN_8KHZ>>2))|(BUFLEN_8KHZ>>4))|(BUFLEN_8KHZ>>8))|(BUFLEN_8KHZ>>16))+1) //Get the power of 2 above the blocklength
#define NPSD 33//(((NFFT)/2)+1)	//Assumes NFFT is even, size of the PSD

// In file pilot_switch.c
#define VAD_LMR_WAIT_TIME ((short)250) //ms
#define VAD_TIMER_LMR ((short)VAD_LMR_WAIT_TIME>>2)

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

// ---< Macros >---
// Macro for printing debug messages.
// Taken from http://ecloud.org/index.php?title=Debug_printf_macros_for_C/C%2B%2B
// Other solutions at http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing
// To get the ((void)0) trick to work the GCC extension has got to be supported (--gcc compiler flag)
// see http://processors.wiki.ti.com/index.php/GCC_Extensions_in_TI_Compilers
#if ENABLE_PRINT_DEBUG
#define PRINT_DEBUG printf
#else
#define PRINT_DEBUG(format, args...) ((void)0)
#endif
// ---< Enumerations >---
// Mixer channel enumeration
// Sorry for this really ugly solution with " = RX_*" but this is necessary for the interface function to enable/disable certain inputs.
typedef enum {
	MIXER_RX_IN_TECH1  = RX_CH_TECH_1,
	MIXER_RX_IN_TECH2  = RX_CH_TECH_2,
	MIXER_RX_IN_TECH3  = RX_CH_TECH_3,
	MIXER_RX_IN_TECH4  = RX_CH_TECH_4,
	MIXER_RX_IN_TECH5  = RX_CH_TECH_5,
	MIXER_RX_IN_TECH6  = RX_CH_TECH_6,
	MIXER_RX_IN_TECH7  = RX_CH_TECH_7,
	MIXER_RX_IN_TECH8  = RX_CH_TECH_8,
	MIXER_RX_IN_MMI_FX = RX_CH_MMI_SOUNDS,
	MIXER_RX_IN_LMR    = RX_CH_LMR,
	MIXER_RX_IN_FTN    = RX_CH_FTN,
	MIXER_RX_IN_PILOT  = RX_CH_PILOT,
	MIXER_RX_IN_CLT    = RX_CH_CLT,
	MIXER_RX_IN_MIC_TECH,				// Processed mic on unit
	MIXER_IN_DTMF_LS, 					// Channel for DTMF tones to sidetone in LS
	MIXER_IN_DTMF_LINE, 				// Channel to make call with DTMF
	NUM_CH_MIXER_IN
} eMixerIn_t;

typedef enum {
	// Directive audio channels
	MIXER_LS_OUT_TECH_LEFT_FRONT,		// PT and CA
	MIXER_LS_OUT_TECH_LEFT_BACK,		// PT and CA
	MIXER_LS_OUT_TECH_RIGHT_FRONT,		// PT and CA
	MIXER_LS_OUT_TECH_RIGHT_BACK,		// PT and CA
	MIXER_LS_OUT_TECH_CENTER,			// PT and CA
	// Mixer channels
	MIXER_LS_OUT_TECH_MONO,				// BS
	MIXER_LS_OUT_PILOT_MONO,			// BS and CA
//	MIXER_LS_OUT_PILOT_CABLE,			// BS and CA	(In pre-mixer due to LEC)
	MIXER_LS_OUT_PILOT_CLT_CABLE,		// BS
	// Line output
	MIXER_TX_LINE_OUT_LMR,				// BS
	MIXER_TX_LINE_OUT_FTN,				// BS
	MIXER_TX_LINE_OUT_CLT,				// BS
	// Radio output
	MIXER_TX_RADIO_OUT_LMR,				// BS
	MIXER_TX_RADIO_OUT_FTN,				// BS
	MIXER_TX_RADIO_OUT_CLT,				// BS
	MIXER_TX_RADIO_OUT_PILOT_AND_CLT,	// BS
	MIXER_TX_RADIO_OUT_TECH,			// PT, BS and CA
	NUM_CH_MIXER_OUT					
} eMixerOut_t;

//typedef enum {
//	PRE_MIXER_RX_IN_TECH1  = RX_CH_TECH_1,
//	PRE_MIXER_RX_IN_TECH2  = RX_CH_TECH_2,
//	PRE_MIXER_RX_IN_TECH3  = RX_CH_TECH_3,
//	PRE_MIXER_RX_IN_TECH4  = RX_CH_TECH_4,
//	PRE_MIXER_RX_IN_TECH5  = RX_CH_TECH_5,
//	PRE_MIXER_RX_IN_TECH6  = RX_CH_TECH_6,
//	PRE_MIXER_RX_IN_TECH7  = RX_CH_TECH_7,
//	PRE_MIXER_RX_IN_TECH8  = RX_CH_TECH_8,
//	PRE_MIXER_RX_IN_FTN    = RX_CH_FTN,
//	NUM_CH_PRE_MIXER_IN
//} ePreMixerIn_t;

typedef enum {
	MIXER_LS_OUT_PILOT_CABLE,
	NUM_CH_PRE_MIXER_OUT
} ePreMixerOut_t;

// Microphone channel enumeration for noise reduction / VAD
typedef enum {
	NOIRED_MIC_CH_TECH,			// Technician headset channel
	NOIRED_MIC_CH_PILOT,		// Pilot headset channel
	NOIRED_NUM_MIC_CH
} eMicChannels_t;

// Pilot connection enumeration for mixer
typedef enum {
	AIRCRAFT_CABLE,				// Pilot is connected to BS with cable from aicraft
	HEADSET						// Pilot is connected to BS with headset
} ePilotConnection_t;

// Pilot aircraft switch enumeration for mixer
typedef enum {
	CLT,					// Pilot communicates with StriC CLT
	FIRST_TECH				// Pilot communicates with 1st technician
} ePilotSwitch_t;

// ---< Composites declaration >---
// Struct for configuration/settings
typedef struct {
	tvConfig_t platform;
	bool bMixerUserIs1stTech;							// Is the current user 1st technician. This affects mixer_speaker() output
	rx_channel_enum_t enMixer1stTechChannel;			// In what radio channel is the 1st tech?
	bool bMixerInChannelActive[NUM_RX_CHANNELS+1+2];	// Possible to disable mixer input (+1 for techmic sidetone channel that cannot be deactivated)
														// (+2 for DTMF channels to LS and line)
	bool bMixer1stTechTxOutActive[TV_TECH1_NUM_OUT_CH];	// Possible to disable
	bool bCltConnection;								// Is the communication line for take of command connected via 2-wire (0) or 4-wire (1).
	bool bPilotConnection;								// Is microphone on pilot headset connected via aircraft cable (0) or headset jack (1).
	bool bPilotSwitch;									// Is pilot switch put to communication with CLT (0) or 1st technician (1).
} tvSettings_t;
// Struct containing current status
typedef struct {
	int iVersionMajor;				// Version number: Major (change when API changes, so that all builds with the same major number are API-compatible)
	int iVersionMinor;				// Version number: Minor (change when new release)
	int iVersionBuild;				// Version number: Build (automatically updated from SVN revision number)
	char pcVersionDate[12];			// Version number: Date (automatically set during pre processor step)
	char pcVersionTime[9];			// Version number: Time (automatically set during pre processor step)
	//bool pbVadStatus[NR_VAD];		// VAD status
} tvStatus_t;
// Struct for enabled signal processing modules
typedef struct {
	bool bAgc;
	bool bDirectiveAudio;
	bool bLec;
	bool bLsLimiter;
	bool bNoiseReduction;
} tvModules_t;
// Struct for limiter settings 32-bit
typedef struct {
	int iCounter;
	int iDelayIndex;
	q31 q31gain_set;
	q31 q31gain_set2;
	q31 q31gain;
	q15 q15LIMIT_CLIP_LVL;
	q31 *pq31Delay;
	q31 q31deltaRise;
	q31 q31gammaFall;
} limitSet_t;
// Struct for limiter settings 16-bit
typedef struct {
	int iCounter;
	int iDelayIndex;
	q15 q15gain_set;
	q15 q15gain_set2;
	q15 q15gain;
	q15 q15LIMIT_CLIP_LVL;
	q15 *pq15Delay;
	q15 q15deltaRise;
	q15 q15gammaFall;
} limitSet16_t;
// Struct for fir filter
typedef struct {
	q15 x_vec[2*128];
	int index;
} filterSet_t;
// Struct for DTMF settings
typedef struct {
	bool bGenerateTone;
	bool bGenerateSilence;
	bool bSendQueue;		// bool for sending queue to telephone line
	char cKey;				// character to send
	q15 *psDTMFbuffer;		// Pointer to audio buffer for DTMF
	q15 q15DTMFindexIn;		// Index for knowing where in the queue DMTF should be written
	q15 q15lengthTone;		// number of samples left to send for DTMF tone
	q15 q15lengthSilence;   // number of samples left to send for DTMF pause
	q15 q15timeIndex1;		// Index 1 to keep track time to sine generator
	q15 q15timeIndex2;		// Index 2 to keep track time to sine generator
	q15 q15timeTone;		// time in ms for whole DTMF tone
	q15 q15timeSilence;		// time in ms for whole DTMF pause
} DTMFset_t;
// Struct for DTMF queue
typedef struct {
	char c;					// character in queue
	q15 q15lengthTone;		// number of samples for whole DTMF tone in queue
	q15 q15lengthSilence;	// number of samples for whole DTMF pause in queue
} DTMFqueue_t;

//
typedef struct {
	q15 q15volumePilot;			// Volume setting for pilot headset on BS or CA
	q15 q15volumeTech;	    	// Volume setting for technician headset on PT, BS or CA
	q15 q15volumeTechSidetone;  // Volume setting for tech sidetone on BS, CA or PT
	q15 q15volumePilotSidetone; // Volume setting for pilot sidetone on BS or CA
	q15 q15volumeDTMF;			// Volume setting DTMF on ls on BS or PT
	q15 q15volumeMMI;			// Volume setting for MMI sounds on BS, CA or PT
} volumeSet_t;

typedef struct {
	short sVadSensitivity[NOIRED_NUM_MIC_CH];
	short bVadStatus[NOIRED_NUM_MIC_CH];
	short bVadOverride[NOIRED_NUM_MIC_CH];
} vadSet_t;

//In file noise_reduction.c
typedef struct {
	float fGamma[NPSD];
	float fV[NPSD];
	float fXi[NPSD];
	float fProb;
	float fVoxProb;
	timer_handle_t hTimer;
} vox_status_t;

typedef struct {
	q31 q31PhiNN[NPSD];
	q31 q31PhiXX[NPSD];
	q31 q31PhiYY[NPSD];
	q31 q31PhiXXsmooth[NPSD];
	q31 q31PhiYYsmooth[NPSD];
	q31 q31P[NPSD];
	q31 q31Psmooth[NPSD];
	q15 q15Gain[NFFT];
	q15 q15Shat[2*BUFLEN_8KHZ];
	q15 q15Mick_old[BUFLEN_8KHZ];
	q15 q15Noise_old[BUFLEN_8KHZ];
	bool bRescueAdapt;
	unsigned int uiSpeechCount;
	fade_handle_t hFade;
	vox_status_t vox_status;
} nr_status_t;


// ---< Function prototypes >---
// In file agc.c
void agc_init(void);
void agc(q15 *, limitSet16_t *agcSet); 
void limiter_init_16(void);
void limiter_init_pre_mixer(void);
void limit_and_delay_16(const q15 *pq15in, q15 *out, limitSet16_t *lset);

// In file directive_audio.c
void directive_audio_init(void);
void directive_audio(q15 *, q15 *);

// In file dtmf.c
int DTMF_queue(char c, q15 q15timeTone, q15 q15timeSilence,DTMFqueue_t *, DTMFset_t *);
void DTMF_init(void);
void dtmf_clear_queue(DTMFqueue_t *,  DTMFset_t *);
void dtmf_remove_last_added(DTMFqueue_t *,  DTMFset_t *);
void send_dtmf(q15 *, DTMFqueue_t *,  DTMFset_t *);

// In file lec_general.c
void lec(q15 *, q15 *, q15 *, q15 *, q15 *, q15 *);
void lec_aircraft_cable(q15 *, q15 *);
void lec_init(bool reset_all);
void lec1_init(bool reset_all);

// In file mixer.c
void pre_aircraft_mixer(void);
void pre_mixer_init(void);
void pre_mixer_channel_setup(tvRxChannels_t, tvMicChannels_t, tvTxChannels_t, tvSpeakerChannels_t);
void mixer_init(void);
void pilot_channel_selection(q15 *, q15 *, q15 *, q15 *);
void mixer_channel_setup(tvRxChannels_t, tvMicChannels_t, tvTxChannels_t, tvSpeakerChannels_t);
void mixer(void);

// In file noise_reduction.c
bool nr_damp(q15 *pq15x1, const q15 *pq15x2, nr_status_t *nr_status, bool bDisableVAD, short sVadSensitivity);
void nr_init(void);

// In file pilot_switch.c
void pilot_switch(q15 *, q15 *);
void energy_vad(q15 s[], q15 *Eold, bool *bSpeechLMR, timer_handle_t *timer_LMR_VAD);
void vad_lmr_init(void);

// In file volume.c
void volume(q15 *, q15 volumeSetting);
void volume_init(void);
void add_sidetone_mmi(q15 *, q15 *, q15 *, limitSet16_t *limiterSet);
void add_sidetone_dtmf_mmi(q15 *, q15 *, q15 *, q15 *, limitSet16_t *limiterSet);

// ---< External variables >---
// In file agc.c
extern limitSet_t txLimSet;
extern limitSet16_t txLimSet16;
extern limitSet16_t limiter_Out[NUM_CH_MIXER_OUT];
extern limitSet16_t limiter_pre_mixer_Out[NUM_CH_PRE_MIXER_OUT];
extern limitSet16_t agcTech;
extern limitSet16_t agcPilot;
extern limitSet16_t agcLmr;
extern limitSet16_t limiterLsLeft;
extern limitSet16_t limiterLsRight;
extern limitSet16_t limiterLsTechMono;
extern limitSet16_t limiterLsPilotMono;
extern limitSet16_t limiterLsLeft_CA;
extern limitSet16_t limiterLsRight_CA;
extern limitSet16_t limiterLsTechMono_CA;
extern limitSet16_t limiterLsPilotMono_CA;

// In file directive_audio.c
extern q15 pq15lsTechLeftFront[BUFLEN_8KHZ];
extern q15 pq15lsTechLeftBack[BUFLEN_8KHZ];
extern q15 pq15lsTechRightFront[BUFLEN_8KHZ];
extern q15 pq15lsTechRightBack[BUFLEN_8KHZ];
extern q15 pq15lsTechCenter[BUFLEN_8KHZ];

//extern filterSet_t filterSetLeftFront_left;
//extern filterSet_t filterSetLeftFront_right;
//extern filterSet_t filterSetLeftBack_left;
//extern filterSet_t filterSetLeftBack_right;
//extern filterSet_t filterSetRightFront_left;
//extern filterSet_t filterSetRightFront_right;
//extern filterSet_t filterSetRightBack_left;
//extern filterSet_t filterSetRightBack_right;
//extern filterSet_t filterSetCenter_left;
//extern filterSet_t filterSetCenter_right;
extern q15 filterSetLeftFront_left[];
extern q15 filterSetLeftFront_right[];
extern q15 filterSetLeftBack_left[];
extern q15 filterSetLeftBack_right[];
extern q15 filterSetRightFront_left[];
extern q15 filterSetRightFront_right[];
extern q15 filterSetRightBack_left[];
extern q15 filterSetRightBack_right[];
extern q15 filterSetCenter_left[];
extern q15 filterSetCenter_right[];

extern limitSet16_t limitSetDirecAudio[2];

// In file dtmf.c
extern DTMFqueue_t DTMFqueue[];
extern DTMFqueue_t DTMFqueueLs[];
extern DTMFset_t DTMFset;
extern DTMFset_t DTMFsetLs;

// In file interface.c
extern tvStatus_t tvStatus;
extern tvSettings_t tvSettings;
extern tvModules_t tvModules;

// in mixer.c
extern q15 pq15MixerNormFactor[NUM_CH_MIXER_OUT];
extern short psMixerDeNormFactor[NUM_CH_MIXER_OUT];
extern q15 pq15PreMixerNormFactor[NUM_CH_PRE_MIXER_OUT];
extern short psPreMixerDeNormFactor[NUM_CH_PRE_MIXER_OUT];
extern eMixerIn_t eMixerIn;
extern eMixerOut_t eMixerOut;
//extern ePreMixerIn_t ePreMixerIn;
extern ePreMixerOut_t ePreMixerOut;
extern ePilotConnection_t ePilotConnetion;
extern ePilotSwitch_t ePilotSwitch;

// In file noise_reduction.c
extern vadSet_t vadSettings;
extern nr_status_t nr_status[];

extern float fGamma[NPSD];
extern float fV[NPSD];
extern float fXi[NPSD];
extern float fProb;
extern float fVoxProb;

// in file volume.c
extern volumeSet_t volumeSet;

// In file pilot_switch.c
extern q15 q15Elmr;
extern bool bSpeechLMR;
extern timer_handle_t timer_LMR_VAD;
extern bool bLMRVadOverride;


#endif // TRUE_VOICE_INTERNAL_H
