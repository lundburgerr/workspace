/*****************************************************************************
 *	\file		interface.c
 *
 *	\date		2013-May-27
 *
 *	\brief		Interface functions for TrueVoice
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
// ---< Include >---
#include "true_voice_internal.h"
#include "subband.h"
#include <string.h>
#include "version.h"

// ---< Defines >---

// ---< Global variabels >---
tvSettings_t tvSettings;
tvStatus_t tvStatus;
tvModules_t tvModules;


// Number of input and output channels
int iNrRxChannels;
int iNrMicChannels;
int iNrTxChannels;
int iNrLsChannels;

// ---< Local function prototypes >---

// ---< Functions >---
/*****************************************************************************
 *	\brief		Initialize TrueVoice
 *	\parameters	Platform to be configured for
 *	\return		None
 *****************************************************************************/
void true_voice_init(tvConfig_t conf) {
	int i;
	/*
	q15 q15a = MIN_16, q15b = MIN_16;
	q31 q31a, q31b;
	
	PRINT_DEBUG("Checking fractional arithmetics\n");
	PRINT_DEBUG("q15: %d + %d = %d\n", q15a, q15b, add_q15(q15a, q15b));
	PRINT_DEBUG("q15: %d * %d = %d\n", q15a, q15b, mult_q15(q15a, q15b));
	q15b += 1;
	PRINT_DEBUG("q15: %d * %d = %d\n", q15a, q15b, mult_q15(q15a, q15b));
	PRINT_DEBUG("q15: %d * %d = %d\n", q15a, q15b, mult_q15(q15a, q15b));
	PRINT_DEBUG("q15: %d << %d = %d\n", q15a, q15b, shl_q15(q15a, q15b));
	PRINT_DEBUG("q15: %d >> %d = %d\n", q15a, q15b, shr_q15(q15a, q15b));
	
	q15a = MIN_16;
	q15b = MIN_16;
	PRINT_DEBUG("q31: %d * %d = %d\n", q15a, q15b, mult_q31(q15a, q15b));
	
	q31a = MAX_32;
	q31b = MAX_32;
	PRINT_DEBUG("q31x31: %d * %d = %d\n", q31a, q31b, mult_q31x31(q31a, q31b));
	q31a = MAX_32;
	q31b = MIN_32;
	PRINT_DEBUG("q31x31: %d * %d = %d\n", q31a, q31b, mult_q31x31(q31a, q31b));
	q31a = MIN_32;
	q31b = MIN_32;
	PRINT_DEBUG("q31x31: %d * %d = %d\n", q31a, q31b, mult_q31x31(q31a, q31b));
	*/
	// General initialization
	
	// Store platform
	tvSettings.platform = conf;
	tvSettings.bMixerUserIs1stTech = false;
	tvSettings.enMixer1stTechChannel = RX_CH_TECH_1; // This could be between RX_CH_TECH_1 and RX_CH_TECH_8
	// Mixer
	for(i=0; i<NUM_RX_CHANNELS+1+2; i++) { // (+1 for techmic sidetone channel that cannot be deactivated)(+2 for DTMF channels to LS and line)
		tvSettings.bMixerInChannelActive[i] = true;		// No channels disabled
	}
	for(i=0; i<TV_TECH1_NUM_OUT_CH; i++) {								// Who should hear 1st tech?
		tvSettings.bMixer1stTechTxOutActive[i] = false;
	}

	tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LS_TECH] = true; // TODO: Is this default setting correct?
	tvSettings.bCltConnection = 0;		// 4-wire=0, 2-wire=1.
	// TODO: how is the choice of pilot connection made?
	tvSettings.bPilotConnection = 0;	// PilotCable=0, PilotHeadset=1.
	tvSettings.bPilotSwitch = 0;		// Aircraft communication with CLT=0, 1st tech=1.

	// Version number (generated in pre-build step and written to version.h)
	tvStatus.iVersionMajor = TV_VERSION_MAJOR;
	tvStatus.iVersionMinor = TV_VERSION_MINOR;
	tvStatus.iVersionBuild = TV_VERSION_BUILD;
	sprintf(tvStatus.pcVersionDate, "%s", __DATE__);
	sprintf(tvStatus.pcVersionTime, "%s", __TIME__);

	// Enabled signal processing modules
	tvModules.bAgc				= true;
	tvModules.bDirectiveAudio	= true;
	tvModules.bLec				= true;
	tvModules.bLsLimiter		= true;
	tvModules.bNoiseReduction	= true;

	// Number of input and output channels
	iNrRxChannels = sizeof(tvRxChannels_t)/sizeof(short *);
	iNrMicChannels = sizeof(tvMicChannels_t)/sizeof(short *);
	iNrTxChannels = sizeof(tvTxChannels_t)/sizeof(short *);
	iNrLsChannels = sizeof(tvSpeakerChannels_t)/sizeof(short *);
	PRINT_DEBUG("Number of RX channels: %d\n", iNrRxChannels);
	PRINT_DEBUG("Number of MIC channels: %d\n", iNrMicChannels);
	PRINT_DEBUG("Number of TX channels: %d\n", iNrTxChannels);
	PRINT_DEBUG("Number of LS channels: %d\n", iNrLsChannels);
	
	// Initialization of modules
	pre_mixer_init();
	mixer_init();
	directive_audio_init();
	nr_init();
	lec_init(true);
	DTMF_init();
	subband_init();
	volume_init();
	limiter_init_pre_mixer();
	limiter_init_16();
	/*
	vad_init();
	...
	*/
	agc_init();
	dumpdata_dump_init();
	vad_lmr_init();
}

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
short true_voice_clt_mode(short s) {
	bool bA, bB;
	bA = (0 == s || 1 == s);				// Correct input
	bB = (s != tvSettings.bCltConnection);	// Setting has changed
	if(bA && bB) {
		tvSettings.bCltConnection = s;
		// Reconfigure mixer
		mixer_init();
	}
	return(tvSettings.bCltConnection);
}

/*****************************************************************************
 *	\brief		Get version number of TrueVoice software
 *	\parameters	*major	- Major version number
 *				*minor	- Minor version number
 *				*build	- Build version number
 *				date	- String with build date for lib-file
 *				time	- String with build time for lib-file
 *	\return		None
 *****************************************************************************/
void true_voice_get_version_info(short *major, short *minor, short *build, char date[12], char time[9]) {
	// Copy from internal status struct
	*major = tvStatus.iVersionMajor;
	*minor = tvStatus.iVersionMinor;
	*build = tvStatus.iVersionBuild;
	strncpy(date, tvStatus.pcVersionDate, sizeof(char)*12);
	strncpy(time, tvStatus.pcVersionTime, sizeof(char)*9);
}


/*****************************************************************************
 *	\brief		Specify which of the rx channels that the 1st technician
 * 				uses. This is for the mixer so that the correct direction of 
 * 				the 1st tech is made.
 *	\parameters	Channel number n according to the RX_CH_TECH_n channels, the
 * 				default channel is RX_CH_TECH_1. To only read current
 * 				configuration use NUM_RX_CHANNELS.
 *	\return		Current 1st technician channel.
 *****************************************************************************/
rx_channel_enum_t true_voice_mixer_1st_tech_channel(rx_channel_enum_t nr) {
	
	if(nr <= RX_CH_TECH_8) {
		if(tvSettings.enMixer1stTechChannel != nr) {
			// Store new setting
			tvSettings.enMixer1stTechChannel = nr;
			// Reconfigure mixer
			pre_mixer_init();
			mixer_init();
		}
	}
	// Return current setting
	return(tvSettings.enMixer1stTechChannel);
}


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
short true_voice_mixer_mode(short s) {
	bool bA, bB;
	
	bA = (0 == s || 1 == s);					// Correct input
	bB = (s != tvSettings.bMixerUserIs1stTech);	// Setting has changed
	if(bA && bB) {
		tvSettings.bMixerUserIs1stTech = s;
		// Reconfigure mixer
		pre_mixer_init();
		mixer_init();
	}
	return(tvSettings.bMixerUserIs1stTech);
}

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
short true_voice_mixer_active_channel(rx_channel_enum_t nr, short s) {
	bool bA, bB;
	
	bA = (0 == s || 1 == s);							// Correct input
	bB = (s != tvSettings.bMixerInChannelActive[nr]);	// Setting has changed
	
	if(bA && bB) {
		tvSettings.bMixerInChannelActive[nr] = s;
		// Reconfigure mixer
		pre_mixer_init();
		mixer_init();
	}
	return(tvSettings.bMixerInChannelActive[nr]);
}

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
short true_voice_module_enable(eTvModules_t module, short setting) {
	bool* moduleSetting;

	switch (module) {
	case AGC:
		moduleSetting = &tvModules.bAgc;
		break;
	case DIRECTIVE_AUDIO:
		moduleSetting = &tvModules.bDirectiveAudio;
		break;
	case LEC:
		moduleSetting = &tvModules.bLec;
		break;
	case LS_LIMITER:
		moduleSetting = &tvModules.bLsLimiter;
		break;
	case NOISE_REDUCTION:
		moduleSetting = &tvModules.bNoiseReduction;
		break;
	}

	if (0 == setting) {
		*moduleSetting = false;
	}
	else if (1 == setting) {
		*moduleSetting = true;
	}
	return *moduleSetting;
}

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
short true_voice_pilot_mode(short s) {
	bool bA, bB;
	bA = (0 == s || 1 == s);					// Correct input
	bB = (s != tvSettings.bPilotConnection);	// Setting has changed
	if(bA && bB) {
		tvSettings.bPilotConnection = s;
		// Reconfigure mixer
		mixer_init();
	}
	return(tvSettings.bPilotConnection);
}

/*****************************************************************************
 *	\brief		Override of VAD via push to talk button.
 *	\parameters	setting - 	VAD override setting.
 *							0 (default) normal VAD operation mode.
 *							1 Override VAD and open channel
 *							-1 Read current setting
 *	\return		Current VAD override setting.
 *****************************************************************************/
short true_voice_push_to_talk(short setting)
{
	short i;
	if(setting != -1){
		for (i=0; i<NOIRED_NUM_MIC_CH; i++)
		{
			vadSettings.bVadOverride[i] = setting;
		}
	}
	return vadSettings.bVadOverride[NOIRED_MIC_CH_TECH];
}

/*****************************************************************************
 *	\brief		Override of LMR VAD via push to talk button.
 *	\parameters	setting - 	VAD override setting.
 *							0 (default) normal VAD operation mode.
 *							1 Override VAD and open channel
 *							-1 Read current setting
 *	\return		Current VAD override setting.
 *****************************************************************************/
short true_voice_LMR_push_to_talk(short setting)
{
	if(setting != -1){
		bLMRVadOverride = setting;
	}
	return bLMRVadOverride;
}


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
short true_voice_vad_sensitivity(short channel, short sensitivity)
{
	if (channel < 1 || channel > 2)
		return -1;

	if (sensitivity >= 0 && sensitivity <= 4)
	{
		vadSettings.sVadSensitivity[channel-1] = sensitivity;
	}
	return vadSettings.sVadSensitivity[channel-1];
}

/*****************************************************************************
 *	\brief		Read current VAD status.
 *	\parameters	channel - Channel number for adjusting:
 *						  1 Technician headset on PT, BS or CA
 *						  2 Pilot headset on BS or CA
 *	\return		Current VAD status
 *				0 Channel closed
 *				1 Channel open
 *****************************************************************************/
short true_voice_vad_status(short channel)
{
	if (channel < 1 || channel > 2)
		return -1;
	return vadSettings.bVadStatus[channel-1];
}

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
short true_voice_volume_speaker(short channel, short volume){
	q15 *pq15volume;

	// Set pointer to correct channel
	if(channel == 1) {
		pq15volume = &volumeSet.q15volumeTech;
	} else if(channel == 2) {
		pq15volume = &volumeSet.q15volumePilot;
	}
	// Change volume
	if(volume >= 0 && volume <= 30) {
		*pq15volume = volume;
	}
	return(*pq15volume);
}


/*****************************************************************************
 *	\brief		Sets characters from DTMF in queue before sending.
 *	\parameters	cKey - Character to set in DTMF-queue
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_queue(char cKey){
	// Queue entry to line buffer
	DTMF_queue(cKey, DTMFset.q15timeTone, DTMFset.q15timeSilence, DTMFqueue, &DTMFset);
	// Queue entry to ls buffer
	DTMF_queue(cKey, DTMFsetLs.q15timeTone, DTMFsetLs.q15timeSilence, DTMFqueueLs, &DTMFsetLs);
}

/*****************************************************************************
 *	\brief		Send all characters currently in queue
 *	\parameters None
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_send_queue(){
	DTMFset.bSendQueue = true;
}

/*****************************************************************************
 *	\brief		Specify the tone and pause duration for DTMF signals
 *	\parameters	timeTone   - Tone duration in ms
 *				timePause  - Pause duration in ms
 *				volumeDTMF - Volume damping settings:
 *						 	 0 for 0 dB damping
 *						 	 1 for 1 dB damping
 *						 	 ...
 *						 	 29 for 29 dB damping
 *						 	 30 for 30 dB damping
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_settings(short timeTone, short timePause, short volumeDTMF){
	DTMFset.q15timeTone = timeTone;
	DTMFsetLs.q15timeTone = timeTone;
	DTMFset.q15timeSilence = timePause;
	DTMFsetLs.q15timeSilence = timePause;
	if(volumeDTMF >= 0 && volumeDTMF <= 30) {
		volumeSet.q15volumeDTMF = volumeDTMF;
	}
}

/*****************************************************************************
 *	\brief		Remove all characters currently in queue
 *	\parameters
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_clear_queue(){
	dtmf_clear_queue(DTMFqueue, &DTMFset);
	DTMFset.bSendQueue = false;
}

/*****************************************************************************
 *	\brief		Remove the character that was last added to the queue.
 *	\parameters
 *	\return		None
 *****************************************************************************/
void true_voice_dtmf_remove_last_added(){
	dtmf_remove_last_added(DTMFqueue, &DTMFset);
}

/*****************************************************************************
 *	\brief		Provide TrueVoice with information on which transmit channels
 *				that should be open/closed in the speaker channel mixer. This
 *				functions intended use is for disabling 1st technician
 *				transmit to a specific channel.
 *				NOTE: This function is only relevant for BS.
 *	\parameters	- Channel number according to the tech1_tx_channel_enum_t channels.
 * 				- On/off state for the specified channel
 * 					0 Channel closed
 * 					1 Channel open (default)
 * 					-1 Read current setting
 *	\return		Current setting for the specified channel.
 *****************************************************************************/
short true_voice_1st_tech_active_channel_out(tech1_tx_channel_enum_t nr, short s) {
	bool bA, bB;

	bA = (0 == s || 1 == s);								// Correct input
	bB = (s != tvSettings.bMixer1stTechTxOutActive[nr]);	// Setting has changed

	if(bA && bB) {
		tvSettings.bMixer1stTechTxOutActive[nr] = s;
		// Reconfigure mixer
		pre_mixer_init();
		mixer_init();
	}
	return(tvSettings.bMixer1stTechTxOutActive[nr]);
}
