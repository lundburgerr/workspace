/*****************************************************************************
 *	\file		true_voice.h
 *
 *	\date		2013-May-27
 *
 *	\brief		Header for TrueVoice library
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef TRUE_VOICE_H
#define TRUE_VOICE_H

// ---< Include >---

// ---< Defines >---
// Audio buffer size in samples
#define BUFLEN_8KHZ 32

// ---< Composites declaration >---
// List of configuration for each platform
typedef enum {
	TV_CONFIG_PT,	// Personal Transceiver
	TV_CONFIG_BS,	// Base Stations
	TV_CONFIG_CA	// Communications Adapter
} tvConfig_t;
// Enumeration of RX channels. This is necessary for configuring the mixer
typedef enum {
	RX_CH_TECH_1,
	RX_CH_TECH_2,
	RX_CH_TECH_3,
	RX_CH_TECH_4,
	RX_CH_TECH_5,
	RX_CH_TECH_6,
	RX_CH_TECH_7,
	RX_CH_TECH_8,
	RX_CH_MMI_SOUNDS,
	RX_CH_LMR,
	RX_CH_FTN,
	RX_CH_PILOT,
	RX_CH_CLT,
	NUM_RX_CHANNELS
} rx_channel_enum_t;

// Pointer to audio buffers for receive channels
typedef struct {// Signals used on	| PT	| BS	| CA	|
	short *psTech1;				//	| Yes	| Yes	| Yes	|
	short *psTech2;				//	| Yes	| Yes	| Yes	|
	short *psTech3;				//	| Yes	| Yes	| Yes	|
	short *psTech4;				//	| Yes	| Yes	| Yes	|
	short *psTech5;				//	| Yes	| Yes	| Yes	|
	short *psTech6;				//	| Yes	| Yes	| No	|
	short *psTech7;				//	| Yes	| Yes	| No	|
	short *psTech8;				//	| Yes	| Yes	| No	|
	short *psMmiSounds;			//	| Yes	| Yes	| Yes	|	Voice feedback + more
	short *psLmr;				//	| Yes	| Yes	| No	|	Land mobile radio
	short *psFtn;				//	| Yes	| Yes	| No	|	Telephone line
	short *psPilot;				//	| Yes	| No	| No	|	Pilot (on BS and CA this is a mic input instead)
	short *psClt;				//	| Yes	| No	| No	|	Processed CLT telephone line
	short *psClt4wire;			//	| No	| Yes	| No	|	Unprocessed CLT telephone line
	short *psClt2wire;			//	| No	| Yes	| No	|	Unprocessed CLT telephone line
} tvRxChannels_t;
// Pointer to audio buffers for microphone channels
typedef struct {// Signals used on	| PT	| BS	| CA	|
	short *psMicNoise;			//	| Yes	| Yes	| Yes	|
	short *psTechHeadset;		//	| Yes	| Yes	| Yes	|
	short *psPilotHeadset;		//	| No	| Yes	| Yes	|
	short *psPilotCable;		//	| No	| Yes	| Yes	|
} tvMicChannels_t;
// Pointer to audio buffers for transmit channels
typedef struct {// Signals used on	| PT	| BS	| CA	|
	short *psTechToRadio;		//	| Yes	| Yes	| Yes	|	Processed technician microphone signal
	short *psPilotToRadio;		//	| No	| Yes	| Yes	|	Processed pilot microphone signal (taken from either aircraft cable or pilot headset to be sent to 1st tech PT)
	short *psFtnToLine;			//	| No	| Yes	| No	|	Output to FTN line
	short *psCltToLine;			//	| No	| Yes	| No	|	Output to CLT line
	short *psLmrToLine;			//	| No	| Yes	| No	|	Output to LMR line
	short *psFtnToRadio;		//	| No	| Yes	| No	|	Processed FTN line with echo removed
	short *psCltToRadio;		//	| No	| Yes	| No	|	Processed CLT line with echo removed
	short *psLmrToRadio;		//	| No	| Yes	| No	|	Processed LMR line with VAD and AGC
} tvTxChannels_t;
// Pointer to audio buffers for speaker channels
typedef struct {// Signals used on	| PT	| BS	| CA	|
	short *psTechLeft;			//	| Yes	| No	| Yes	|	Stereo headset signal
	short *psTechRight;			//	| Yes	| No	| Yes	|	Stereo headset signal
	short *psTechMono;			//	| No	| Yes	| No	|	Mono headset signal to be used when directive audio is not used
	short *psPilotMono;			//	| No	| Yes	| Yes	|	Pilot always hear mono audio
} tvSpeakerChannels_t;
// List of signal processing modules
typedef enum {
	AGC,
	DIRECTIVE_AUDIO,
	LEC,
	LS_LIMITER,
	MIXER,
	NOISE_REDUCTION,
	VAD,
	DTMF,
} eTvModules_t;

// ---< Function prototypes >---
void true_voice_init(tvConfig_t);
void true_voice(tvRxChannels_t, tvMicChannels_t, tvTxChannels_t, tvSpeakerChannels_t);
rx_channel_enum_t true_voice_mixer_1st_tech_channel(rx_channel_enum_t);
short true_voice_mixer_active_channel(rx_channel_enum_t nr, short s);
short true_voice_mixer_mode(short);
short true_voice_push_to_talk(short setting);
short true_voice_vad_sensitivity(short channel, short sensitivity);
short true_voice_volume_speaker(short volume);
void true_voice_dtmf_clear_queue();
void true_voice_dtmf_remove_last_added();
void true_voice_dtmf_queue(char);
void true_voice_dtmf_send_queue();
void true_voice_dtmf_settings(short, short);


// Debug functions
short true_voice_vad_status(short channel);
void true_voice_get_version_info(short *major, short *minor, short *build, char date[12], char time[9]);
short true_voice_module_enable(eTvModules_t module, short setting);

// ---< External variables >---

#endif // TRUE_VOICE_H
