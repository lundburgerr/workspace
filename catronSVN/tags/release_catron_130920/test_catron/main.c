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
#include "dsplib.h"
#include "true_voice.h"




// ---< Defines >---
#define N_RX 15
#define N_MIC 4
#define N_TX 8
#define N_LS 4


// ---< Global variabels >---
// The buffers used outside of truevoice to retrieve/send data
short audioRx[N_RX][BUFLEN_8KHZ];
short audioMic[N_MIC][BUFLEN_8KHZ];
short audioTx[N_TX][BUFLEN_8KHZ];
short audioLs[N_LS][BUFLEN_8KHZ];

tvRxChannels_t rx;
tvMicChannels_t mic;
tvTxChannels_t tx;
tvSpeakerChannels_t ls;


static void config_true_voice(void);
static void display_version_number(void);
static void init_audio_signals(void);


/*****************************************************************************
 *	\brief		mic.psTechHeadset will be filled with values that will end up in
*				ls.psTechMono (with some delay) according to the
*				"Specification - Audio processing modules" document.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
int main(void) {

	unsigned int n,m;
	short newBuffer[BUFLEN_8KHZ];

	// === Initializes rx and mic (input) buffers to zeros
	for(n=0; n<N_RX; n++){
		for(m=0; m<BUFLEN_8KHZ; m++)
			audioRx[n][m] = 0;
	}
	for(n=0; n<N_MIC; n++){
		for(m=0; m<BUFLEN_8KHZ; m++)
			audioMic[n][m] = 0;
	}

	// Setup of pointers to audio buffers
	init_audio_signals();

	// Init and config of TrueVoice
	config_true_voice();

	while(1){
		//Update input buffer to whatever
		for(n=0; n<BUFLEN_8KHZ; n++){
			audioMic[1][n] = 1000*n; //mic.psTechHeadset = audioMic[1]
		}

		//Run true voice
		true_voice(rx, mic, tx, ls);

		//Do something with outputBuffer, outputBuffer is delayed. Therefore true_voice
		//has to be run a couple of times before the input data is received in output buffer.
		for(n=0; n<BUFLEN_8KHZ; n++){
			newBuffer[n] = audioLs[2][n]; //ls.psTechMono = audioLs[2]
		}
	}
	return -1;
}



static void config_true_voice(void){
	true_voice_init(TV_CONFIG_BS);
//	true_voice_init(TV_CONFIG_CA);
//	true_voice_init(TV_CONFIG_PT);
	display_version_number();
	
	//The channel for the first tech
	true_voice_mixer_1st_tech_channel(RX_CH_TECH_1);
	//true_voice_mixer_1st_tech_channel(RX_CH_TECH_8);

	//Unless first tech is channel 8, this should be 0
	true_voice_mixer_mode(0);
	//true_voice_mixer_mode(1);

//	true_voice_mixer_active_channel(RX_CH_TECH_3, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_4, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_5, 0);
//	true_voice_mixer_active_channel(RX_CH_TECH_6, 0);
//	true_voice_mixer_active_channel(RX_CH_FTN, 0);

}

/*****************************************************************************
 *	\brief		Display TrueVoice version number
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
static void display_version_number() {
	short major, minor, build;
	char date[12], time[9], s[100];

	true_voice_get_version_info(&major, &minor, &build, date, time);
	sprintf(s, "Limes Audio TrueVoice library v%d.%02d.%03d\tBuild date:\t%s\t%s\n", major, minor, build, date, time);
	printf("%s\n", s);
}



/*****************************************************************************
 *	\brief		Init audio signals
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
static void init_audio_signals() {
	int channel;

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

	channel = 0;
	tx.psTechToRadio	= audioTx[channel++];
	tx.psPilotToRadio	= audioTx[channel++];
	tx.psFtnToLine		= audioTx[channel++];
	tx.psCltToLine		= audioTx[channel++];
	tx.psLmrToLine		= audioTx[channel++];
	tx.psFtnToRadio		= audioTx[channel++];
	tx.psCltToRadio		= audioTx[channel++];
	tx.psLmrToRadio		= audioTx[channel++];

	channel = 0;
	ls.psTechLeft	= audioLs[channel++];
	ls.psTechRight	= audioLs[channel++];
	ls.psTechMono	= audioLs[channel++];
	ls.psPilotMono	= audioLs[channel++];

}
