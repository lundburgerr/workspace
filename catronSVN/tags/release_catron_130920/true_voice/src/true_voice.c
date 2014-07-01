/*****************************************************************************
 *	\file		true_voice.c
 *
 *	\date		2013-May-15
 *
 *	\brief		Main routine for TrueVoice
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
#include <time.h> //This should be removed
#include "../../true_voice/include/plot.h" // Should not be included here but used for testing of limiter

// ---< Defines >---

// ---< Global variabels >---
nr_status_t nr_status_PT_Pilot;
nr_status_t nr_status_BS_Pilot;
nr_status_t nr_status_BS_Tech;
nr_status_t nr_status_CA_Pilot;
nr_status_t nr_status_CA_Tech;
//nr_status_t nr_status;

// ---< Local function prototypes >---

// ---< Functions >---

/*****************************************************************************
 *	\brief		Signal processing
 * 				TODO: Add signal processing modules
 * 				TODO: Add on/off setting for each module
 *	\parameters	
 *	\return		None
 *****************************************************************************/
void true_voice(tvRxChannels_t rx, tvMicChannels_t mic, tvTxChannels_t tx, tvSpeakerChannels_t ls) {
	
//    clock_t t_start, t_stop, t_overhead;
//    t_start = clock();
//    t_stop = clock();
//    t_overhead = t_stop - t_start;
//    t_start = clock();
	/*** Microphone noise reduction ***/
	if(tvSettings.platform == TV_CONFIG_BS){
		vadSettings.bVadStatus[0] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status_BS_Tech, vadSettings.bVadOverride[0], vadSettings.sVadSensitivity[0]);
		vadSettings.bVadStatus[1] = nr_damp(mic.psPilotHeadset, mic.psMicNoise, &nr_status_BS_Pilot, vadSettings.bVadOverride[1], vadSettings.sVadSensitivity[1]);
	}
	else if(tvSettings.platform == TV_CONFIG_CA){
		vadSettings.bVadStatus[0] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status_CA_Tech, vadSettings.bVadOverride[0], vadSettings.sVadSensitivity[0]);
		vadSettings.bVadStatus[1] = nr_damp(mic.psPilotHeadset, mic.psMicNoise, &nr_status_CA_Pilot, vadSettings.bVadOverride[1], vadSettings.sVadSensitivity[1]);
	}
	else if(tvSettings.platform == TV_CONFIG_PT){
		vadSettings.bVadStatus[0] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status_PT_Pilot, vadSettings.bVadOverride[1], vadSettings.sVadSensitivity[1]);
	}
//    t_stop  = clock();
//	printf("Noise reduction took: %f ms\n", (t_stop-t_start-t_overhead)/300E6*1000); //Enable clock first while in debug: Run->Clock->Enable

	/*** Microphone and LMR channel AGC ***/
	// agc(input buffer, struct with agc settings for this channel)
	agc(mic.psTechHeadset, &agcTech);
	//only BS
	if(tvSettings.platform == TV_CONFIG_BS || tvSettings.platform == TV_CONFIG_CA){
		agc(mic.psPilotHeadset, &agcPilot);
	}
	if(tvSettings.platform == TV_CONFIG_BS){
		agc(rx.psLmr, &agcLmr);
	}
	 
	/*** CLT/FTN channels LEC ***/
	//tx.psCltToLine, tx.psFtnToLine is reference signals to be able to cancel the echo
//	t_start = clock();
	lec(rx.psClt2wire, rx.psClt4wire, tx.psCltToLine, tx.psFtnToLine, rx.psClt, rx.psFtn);
//    t_stop  = clock();
//	printf("LEC took: %f ms\n", (t_stop-t_start-t_overhead)/300E6*1000); //Enable clock first while in debug: Run->Clock->Enable

	/*** DTMF tones ***/
	// Send to line only on BS. Send to LS on PT as well?
	if(tvSettings.platform == TV_CONFIG_BS){
		send_dtmf(DTMFset.psDTMFbuffer,DTMFqueue, &DTMFset);
		send_dtmf(DTMFsetLs.psDTMFbuffer,DTMFqueueLs, &DTMFsetLs);
	} else if(tvSettings.platform == TV_CONFIG_PT){
		send_dtmf(DTMFsetLs.psDTMFbuffer,DTMFqueueLs, &DTMFsetLs);
	}

	/*** Channel mixer ***/
	// Create pilot microphone signals from the processed headset signal or from the aircraft cable. Output to rx.psPilot (not on PS)
	pilot_channel_selection(mic.psPilotHeadset, mic.psPilotCable, rx.psPilot);

	// Provide mixer with pointers to audio signals
	mixer_channel_setup(rx, mic, tx, ls);
	// Creates mono output speaker signals and five channels that serve as input to directive audio. Line output signals are also created
	mixer();

	/*** Directive audio ***/
	// Create stereo signal from the five channels created in mixer()
	directive_audio(ls.psTechLeft, ls.psTechRight);

	/*** Volume and limiter adjustment on LS ***/
	//add side tone, dtmf and MMI (since it has a fixed volume setting)
	if(tvSettings.platform == TV_CONFIG_PT) {
		// volume damping ls channels
		volume(ls.psTechLeft,  volumeSet.q15volumeTech);
		volume(ls.psTechRight, volumeSet.q15volumeTech);

		// volume damping sidetone, DTMF and MMI
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);
		volume(DTMFsetLs.psDTMFbuffer, volumeSet.q15volumeDTMF);
		volume(rx.psMmiSounds, volumeSet.q15volumeMMI);

		// Add sidetone, dtmf and MMI to ls signal
		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechLeft, &limiterLsLeft);
		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechRight, &limiterLsRight);

	} else if(tvSettings.platform == TV_CONFIG_BS) {
		// volume damping ls channels
		volume(ls.psTechMono,  volumeSet.q15volumeTech);
		volume(ls.psPilotMono, volumeSet.q15volumePilot);

		// volume damping sidetone, dtmf and mmi
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);
		volume(rx.psPilot, volumeSet.q15volumePilotSidetone);
		volume(DTMFsetLs.psDTMFbuffer, volumeSet.q15volumeDTMF);

		// Add sidetone, dtmf and mmi to ls signal
		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechMono, &limiterLsTechMono);
		add_sidetone_dtmf_mmi(rx.psPilot,DTMFsetLs.psDTMFbuffer, rx.psMmiSounds, ls.psPilotMono, &limiterLsPilotMono);

	} else if(tvSettings.platform == TV_CONFIG_CA) {
		// volume damping ls
		volume(ls.psTechLeft,  volumeSet.q15volumeTech);
		volume(ls.psTechRight, volumeSet.q15volumeTech);
		volume(ls.psPilotMono, volumeSet.q15volumePilot);

		// volume damping sidetone
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);
		volume(rx.psPilot, volumeSet.q15volumePilotSidetone);

		// Add sidetone and mmi to ls signal
		add_sidetone_mmi(mic.psTechHeadset, rx.psMmiSounds, ls.psTechLeft, &limiterLsLeft_CA);
		add_sidetone_mmi(mic.psTechHeadset, rx.psMmiSounds, ls.psTechRight, &limiterLsRight_CA);
		add_sidetone_mmi(rx.psPilot, rx.psMmiSounds, ls.psPilotMono, &limiterLsPilotMono_CA);

	}

	//dumpdata_post_dump();
}

