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
//TODO	short *psTechFromBS_CA;		//	| Yes	| No	| No	|
	short *psMmiSounds;			//	| Yes	| Yes	| Yes	|	Voice feedback + more
	//note that  Lmr, Ftn, Pilot and Clt are either Radio (PT) or "line"
	short *psLmr;				//	| Yes	| Yes	| No	|	Land mobile radio
	short *psFtn;				//	| Yes	| Yes	| No	|	Telephone line
	short *psPilot;				//	| Yes	| No	| No	|	Pilot (on BS and CA this is a mic input instead)
	short *psClt;				//	| Yes	| No	| No	|	Processed CLT telephone line
	short *psClt4wire;			//	| No	| Yes	| No	|	Unprocessed CLT telephone line TODO: do the application know if 2 or 4??
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
	NOISE_REDUCTION,
} eTvModules_t;

// ---< Function prototypes >---
/*****************************************************************************
 *	\brief		Provide TrueVoice with information on which clt connection that
 *				is used, i.e. 2-wire or 4-wire.
 *				NOTE: This setting only affects BS.
 *	\parameters	Mixer configuration
 * 					0 4-wire (default)
 * 					1 2-wire
 * 					-1 Read current setting
 *	\return		Current pilot configuration.
 *****************************************************************************/
short true_voice_clt_mode(short);

/*****************************************************************************
 *	\brief		Initialize TrueVoice
 *	\parameters	Platform to be configured for
 *	\return		None
 *****************************************************************************/
void true_voice_init(tvConfig_t);

/*****************************************************************************
 *	\brief		Audio processing including noise reduction, voice activity
 *				detection, directive hearing. Called continuously as new audio
 *				input samples are available and new (processed) output is required.
 *	\parameters	Pointers to receive signal buffers
 *				Pointers to microphone signal buffers
 *				Pointers to transmit signal buffers
 *				Pointers to speaker signal buffers
 *	\return		None
 *****************************************************************************/
void true_voice(tvRxChannels_t, tvMicChannels_t, tvTxChannels_t, tvSpeakerChannels_t);

/*****************************************************************************
 *	\brief		Specify which of the rx channels that the 1st technician
 * 				uses. This is for the mixer so that the correct direction of
 * 				the 1st tech is made.
 *	\parameters	Channel number n according to the RX_CH_TECH_n channels, the
 * 				default channel is RX_CH_TECH_1. To only read current
 * 				configuration use NUM_RX_CHANNELS.
 *	\return		Current 1st technician channel.
 *****************************************************************************/
rx_channel_enum_t true_voice_mixer_1st_tech_channel(rx_channel_enum_t);

/*****************************************************************************
 *	\brief		Provide TrueVoice with information on which channels that
 * 				should be used in the speaker channel mixer. If e.g.
 * 				communication with tech 3 and 4 should not be heard, these
 * 				channels has got to be disabled.
 * 				NOTE: The mixing configuration for each platform is not
 * 				configured with this function. This function is used only to
 * 				mute channels that should not be heard right now...
 *	\parameters	- Channel number n according to the RX_CH_TECH_n channels.
 * 				- On/off state for the specified channel
 * 					0 Channel closed
 * 					1 Channel open (default)
 * 					-1 Read current setting
 *	\return		Current setting for the specified channel.
 *****************************************************************************/
short true_voice_mixer_active_channel(rx_channel_enum_t nr, short s);

/*****************************************************************************
 *	\brief		Provide TrueVoice with information on how the directive audio
 * 				should be configured, i.e. the stereo direction of the input
 * 				channels. NOTE: This setting only affects PT.
 *	\parameters	Mixer configuration
 * 					0 Regular technician (default)
 * 					1 1st technician
 * 					-1 Read current setting
 *	\return		Current mixer configuration.
 *****************************************************************************/
short true_voice_mixer_mode(short);

/*****************************************************************************
 *	\brief		Provide TrueVoice with information on which pilot connection
 *				that is used,  i.e. Pilot cable from aircraft or headset jack.
 * 				NOTE: This setting only affects CA and BS.
 *	\parameters	Mixer configuration
 * 					0 Pilot from aircraft cable (default)
 * 					1 Pilot from headset jack
 * 					-1 Read current setting
 *	\return		Current pilot configuration.
 *****************************************************************************/
short true_voice_pilot_mode(short);

/*****************************************************************************
 *	\brief		Override of VAD via push to talk button.
 *	\parameters	setting - 	VAD override setting.
 *							0 (default) normal VAD operation mode.
 *							1 Override VAD and open channel
 *							-1 Read current setting
 *	\return		Current VAD override setting.
 *****************************************************************************/
short true_voice_push_to_talk(short setting);

/*****************************************************************************
 *	\brief		Adjust sensitivity of VAD.
 *	\parameters	channel - 	  Channel number for adjusting:
 *						  	  1 Technician headset on PT, BS or CA
 *						  	  2 Pilot headset on BS or CA
 *				sensitivity - Sensitivity level at which voice is detected
 *							  0 Weak voice required
 *							  1 Semi-weak voice required
 *							  2 Normal mode (default)
 *							  3 Semi-loud voice required
 *							  4 Loud voice required
 *							  -1 Read current setting
 *	\return		Current VAD sensitivity setting
 *****************************************************************************/
short true_voice_vad_sensitivity(short channel, short sensitivity);

/*****************************************************************************
 *	\brief		Read current VAD status.
 *	\parameters	channel - Channel number for adjusting:
 *						  1 Technician headset on PT, BS or CA
 *						  2 Pilot headset on BS or CA
 *	\return		Current VAD status
 *				0 Channel closed
 *				1 Channel open
 *****************************************************************************/
short true_voice_vad_status(short channel);

/*****************************************************************************
 *	\brief		Set volume damping level.
 *	\parameters	channel - Channel number for adjusting:
 *						  1 Technician headset on PT, BS or CA
 *						  2 Pilot headset on BS or CA
 *				volume  - Volume damping settings:
 *						  0 for 0 dB damping
 *						  1 for 1 dB damping
 *						  ...
 *						  29 for 29 dB damping
 *						  30 for 30 dB damping
 *						  -1 to read current setting.
 *	\return		Current volume setting
 *****************************************************************************/
short true_voice_volume_speaker(short channel, short volume);

/*****************************************************************************
 *	\brief		Remove all characters currently in queue
 *	\parameters
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_clear_queue();

/*****************************************************************************
 *	\brief		Sets characters from DTMF in queue before sending.
 *	\parameters	cKey - Character to set in DTMF-queue
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_queue(char);

/*****************************************************************************
 *	\brief		Remove the character that was last added to the queue.
 *	\parameters
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_remove_last_added();

/*****************************************************************************
 *	\brief		Send all characters currently in queue
 *	\parameters None
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_send_queue();

/*****************************************************************************
 *	\brief		Specify the tone and pause duration for DTMF signals
 *	\parameters	timeTone  - Tone duration in ms
 *				timePause - Pause duration in ms
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_settings(short, short);


// Debug functions
/*****************************************************************************
 *	\brief		Get version number of TrueVoice software
 *	\parameters	*major	- Major version number
 *				*minor	- Minor version number
 *				*build	- Build version number
 *				date	- String with build date for lib-file
 *				time	- String with build time for lib-file
 *	\return		None
 *****************************************************************************/
void true_voice_get_version_info(short *major, short *minor, short *build, char date[12], char time[9]);

/*****************************************************************************
 *	\brief		Enables/disables modules in true voice
 *	\parameters	- One of the following modules
  	  	  	  	  	 AGC, DIRECTIVE_AUDIO, LEC, LS_LIMITER or NOICE_REDUCTION
 *				- On/off state for the specified module
 * 					 0 Module disabled
 * 					 1 Module enabled
 * 					-1 Read current setting
 *	\return		Current setting for the specified module.
 *****************************************************************************/
short true_voice_module_enable(eTvModules_t module, short setting);

// ---< External variables >---

#endif // TRUE_VOICE_H
