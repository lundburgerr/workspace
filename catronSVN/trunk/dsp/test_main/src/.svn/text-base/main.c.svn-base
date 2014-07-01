/*****************************************************************************
 *	\file		main.c
 *
 *	\date		2013-May-27
 *
 *	\brief		Testing of TrueVoice
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
#include "stdio.h"
//#include "dsplib.h"
#include "true_voice.h"
#include <string.h>

// Include file with speech as an example on how to use input buffers.
#pragma  DATA_SECTION (maleSpeechExample, "extvar")
#pragma  DATA_ALIGN(maleSpeechExample, 64)
short maleSpeechExample[11311]={
#include "../../true_voice/include/malespeech8000.inc"
};

// ---< Defines >---
#define N_RX 15
#define N_MIC 5
#define N_TX 8
#define N_LS 6

// ---< Global variabels >---
// Memory allocation for audio buffers
short audioRx[N_RX][BUFLEN_8KHZ];
short audioMic[N_MIC][BUFLEN_8KHZ];
short audioTx[N_TX][BUFLEN_8KHZ];
short audioLs[N_LS][BUFLEN_8KHZ];

// Pointer for audio buffers
tvRxChannels_t rx;
tvMicChannels_t mic;
tvTxChannels_t tx;
tvSpeakerChannels_t ls;

// ---< Local function prototypes >---
static void config_true_voice(void);
static void init_audio_signals(void);
void test_main(void); // Only for debug, if making a release: remove!

/*****************************************************************************
 *	\brief		Run true voice. Update input buffers according to
*				"Specification - Audio processing modules" document.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void main(void) {
	int n, speechIndex;
	int debug = 1; // Only for debug, if making a release: remove!

	if (debug){
		test_main(); // Only for debug, if making a release: remove!
	} else {
		// Initialize rx and mic (input) buffers to zeros
		memset(audioRx, 0, N_RX*BUFLEN_8KHZ*sizeof(short));
		memset(audioMic, 0, N_MIC*BUFLEN_8KHZ*sizeof(short));

		// Setup of pointers to audio buffers
		init_audio_signals();

		// Init and config of TrueVoice
		config_true_voice();

		speechIndex=0;
		while(1){
			//Update input buffer
			for(n=0; n<BUFLEN_8KHZ; n++){
				mic.psTechHeadset[n] = maleSpeechExample[n+speechIndex];
			}
			speechIndex+=BUFLEN_8KHZ;
			if (speechIndex>=11311-BUFLEN_8KHZ){
				speechIndex=0;
			}

			//Run true voice
			true_voice(rx, mic, tx, ls);

			// Do something with output buffers
			// mic.psTechHeadset will for example go to output buffer
			// tx.psTechToRadio

		}
	}
}

/*****************************************************************************
 *	\brief		Initialization and configuration of TrueVoice
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void config_true_voice() { // Catron-controlled function
	rx_channel_enum_t nr;
	
	true_voice_init(TV_CONFIG_BS);
//	true_voice_init(TV_CONFIG_CA);
//	true_voice_init(TV_CONFIG_PT);
	
	//The channel for the first tech
	true_voice_mixer_1st_tech_channel(RX_CH_TECH_1);
	//true_voice_mixer_1st_tech_channel(RX_CH_TECH_8);

	//Unless first tech is channel 8, this should be 0
	true_voice_mixer_mode(0);
	//true_voice_mixer_mode(1);

	//true_voice_pilot_mode(0);			// 0 = Pilot aircraft cable, 1 = Pilot headset
	//true_voice_clt_mode(1);			// 0 = 4-wire, 1 = 2-wire
	//true_voice_volume_speaker(1, 10);
	//true_voice_volume_speaker(2, 5);
	//true_voice_dtmf_settings(80, 80, 0);

//	true_voice_module_enable(AGC, 1);
//	true_voice_module_enable(DIRECTIVE_AUDIO, 1);
//	true_voice_module_enable(LEC, 1);
//	true_voice_module_enable(LS_LIMITER, 1);
//	true_voice_module_enable(NOISE_REDUCTION, 1);

//	true_voice_mixer_active_channel(RX_CH_TECH_3, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_4, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_5, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_6, 0);
//	true_voice_mixer_active_channel(RX_CH_FTN, 0);
	
}

/*****************************************************************************
 *	\brief		Init audio signals
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
static void init_audio_signals() {
	int channel;

	// === Setup of pointers to TrueVoice struct ===
	channel = 0;
	rx.psTech1		= audioRx[channel++];
	rx.psTech2		= audioRx[channel++];
	rx.psTech3		= audioRx[channel++];
	rx.psTech4		= audioRx[channel++];
	rx.psTech5		= audioRx[channel++];
	rx.psTech6		= audioRx[channel++];
	rx.psTech7		= audioRx[channel++];
	rx.psTech8		= audioRx[channel++];
	rx.psMmiSounds	= audioRx[channel++];
	rx.psLmr		= audioRx[channel++];
	rx.psFtn		= audioRx[channel++];
	rx.psPilot		= audioRx[channel++];
	rx.psClt		= audioRx[channel++];
	rx.psClt4wire	= audioRx[channel++];
	rx.psClt2wire	= audioRx[channel++];
	
	channel = 0;
	mic.psMicNoise		= audioMic[channel++];
	mic.psTechHeadset	= audioMic[channel++];
	mic.psPilotHeadset	= audioMic[channel++];
	mic.psPilotCable	= audioMic[channel++];
	mic.psPilotCltCable	= audioMic[channel++];
	
	channel = 0;
	tx.psTechToRadio	= audioTx[channel++];
	tx.psPilotAndCltToRadio	= audioTx[channel++];
	tx.psFtnToLine		= audioTx[channel++];
	tx.psCltToLine		= audioTx[channel++];
	tx.psLmrToLine		= audioTx[channel++];
	tx.psFtnToRadio		= audioTx[channel++];
	tx.psCltToRadio		= audioTx[channel++];
	tx.psLmrToRadio		= audioTx[channel++];
	
	channel = 0;
	ls.psTechLeft			= audioLs[channel++];
	ls.psTechRight			= audioLs[channel++];
	ls.psTechMono			= audioLs[channel++];
	ls.psPilotMono			= audioLs[channel++];
	ls.psPilotMonoCable		= audioLs[channel++];
	ls.psPilotMonoCltCable	= audioLs[channel++];

}
