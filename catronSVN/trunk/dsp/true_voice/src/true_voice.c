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

	//VAD for LMR
	//init Elmr, bSpeechLMR, timer bLMRVadOverride
//	energy_vad(rx.psLmr, &q15Elmr, &bSpeechLMR, &timer_LMR_VAD);
//	if(bLMRVadOverride == true){
//		bSpeechLMR = 1;
//	}
//	if(bSpeechLMR == false){
//		memset(rx.psLmr, 0, sizeof(q15)*BUFLEN_8KHZ);
//	}

//    clock_t t_start, t_stop, t_overhead;
//    t_start = clock();
//    t_stop = clock();
//    t_overhead = t_stop - t_start;
//    t_start = clock();
	/*** Microphone noise reduction ***/
	if(tvSettings.platform == TV_CONFIG_BS){
		vadSettings.bVadStatus[NOIRED_MIC_CH_TECH] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status[NOIRED_MIC_CH_TECH], vadSettings.bVadOverride[NOIRED_MIC_CH_TECH], vadSettings.sVadSensitivity[NOIRED_MIC_CH_TECH]);
		vadSettings.bVadStatus[NOIRED_MIC_CH_PILOT] = nr_damp(mic.psPilotHeadset, mic.psMicNoise, &nr_status[NOIRED_MIC_CH_PILOT], vadSettings.bVadOverride[NOIRED_MIC_CH_PILOT], vadSettings.sVadSensitivity[NOIRED_MIC_CH_PILOT]);
	}
	else if(tvSettings.platform == TV_CONFIG_CA){
		vadSettings.bVadStatus[NOIRED_MIC_CH_TECH] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status[NOIRED_MIC_CH_TECH], vadSettings.bVadOverride[NOIRED_MIC_CH_TECH], vadSettings.sVadSensitivity[NOIRED_MIC_CH_TECH]);
		vadSettings.bVadStatus[NOIRED_MIC_CH_PILOT] = nr_damp(mic.psPilotHeadset, mic.psMicNoise, &nr_status[NOIRED_MIC_CH_PILOT], vadSettings.bVadOverride[NOIRED_MIC_CH_PILOT], vadSettings.sVadSensitivity[NOIRED_MIC_CH_PILOT]);
	}
	else if(tvSettings.platform == TV_CONFIG_PT){
		// I think that pilot channel should be input?
		vadSettings.bVadStatus[0] = nr_damp(mic.psTechHeadset, mic.psMicNoise, &nr_status[NOIRED_MIC_CH_PILOT], vadSettings.bVadOverride[NOIRED_MIC_CH_PILOT], vadSettings.sVadSensitivity[NOIRED_MIC_CH_PILOT]);
	}

//    t_stop  = clock();
//	printf("Noise reduction took: %f ms\n", (t_stop-t_start-t_overhead)/300E6*1000); //Enable clock first while in debug: Run->Clock->Enable

	/*** Microphone and LMR channel AGC ***/
	// agc(input buffer, struct with agc settings for this channel)
	agc(mic.psTechHeadset, &agcTech);

	if(tvSettings.platform == TV_CONFIG_BS || tvSettings.platform == TV_CONFIG_CA){
		agc(mic.psPilotHeadset, &agcPilot);
	}
	if(tvSettings.platform == TV_CONFIG_BS){
		agc(rx.psLmr, &agcLmr);
	}

	/*** Pilot switch ***/
	// Calculate what state the pilot switch is in (1st tech or CLT).
	pilot_switch(mic.psPilotCable, mic.psPilotCltCable);

	/*** CLT/FTN channels LEC ***/
	//tx.psCltToLine, tx.psFtnToLine is reference signals to be able to cancel the echo
	lec(rx.psClt2wire, rx.psClt4wire, tx.psCltToLine, tx.psFtnToLine, rx.psClt, rx.psFtn);

	/*** Pre mixer ***/
	// Pre mixer of technician and ftn so lec can be made on pilot-technician cable
	// Provide mixer with pointers to audio signals
	pre_mixer_channel_setup(rx, mic, tx, ls);

	pre_aircraft_mixer();

	/*** Pilot-Tech cable connection LEC ***/
	// insert LEC for pilot Tech Cable connection. Only needed when pilot in aircraft choose
	// to talk to technician i.e when tvSettings.bPilotSwitch = 1.
	// mic.psPilotCable = (Pilot+CLT+1st tech (+FTN)), ls.psPilotMonoCable = 1st tech (+FTN)
	// lec to remove 1st tech (+FTN) from mic.psPilotCable
	if (tvSettings.bPilotSwitch){
		lec_aircraft_cable(mic.psPilotCable, ls.psPilotMonoCable);
	}

	/*** DTMF tones ***/
	// Send to line only on BS. Send to LS on PT as well?
	if(tvSettings.platform == TV_CONFIG_BS){
		send_dtmf(DTMFset.psDTMFbuffer,DTMFqueue, &DTMFset);
		send_dtmf(DTMFsetLs.psDTMFbuffer,DTMFqueueLs, &DTMFsetLs);
	} else if(tvSettings.platform == TV_CONFIG_PT){
		send_dtmf(DTMFsetLs.psDTMFbuffer,DTMFqueueLs, &DTMFsetLs);
	}
	/*** Channel mixer ***/
	// Create pilot microphone signals from the processed headset signal or from one of the aircraft cables. Output to rx.psPilot (not on PT)
	pilot_channel_selection(mic.psPilotHeadset, mic.psPilotCable, mic.psPilotCltCable, rx.psPilot);

	// Provide mixer with pointers to audio signals
	mixer_channel_setup(rx, mic, tx, ls);
	// Creates mono output speaker signals and five channels that serve as input to directive audio. Line output signals are also created
	mixer();


	/*** Directive audio ***/
	// Create stereo signal from the five channels created in mixer()
	directive_audio(ls.psTechLeft, ls.psTechRight);

	/*** Volume and limiter adjustment on LS ***/
	// Add side tone, dtmf and MMI (since it has a fixed volume setting)
	if(tvSettings.platform == TV_CONFIG_PT) {
		// volume damping ls channels
		volume(ls.psTechLeft,  volumeSet.q15volumeTech);
		volume(ls.psTechRight, volumeSet.q15volumeTech);

		// volume damping sidetone, DTMF and MMI
		volume(rx.psMmiSounds, volumeSet.q15volumeMMI);
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);
		volume(DTMFsetLs.psDTMFbuffer, volumeSet.q15volumeDTMF);

		// Add sidetone, dtmf and MMI to ls signal
		//TODO: Dtmf only if first tech. But empty if not making a call so maybe this is ok?
		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechLeft, &limiterLsLeft);
		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechRight, &limiterLsRight);

	} else if(tvSettings.platform == TV_CONFIG_BS) {
		// volume damping ls channels
		volume(ls.psTechMono,  volumeSet.q15volumeTech);
		volume(ls.psPilotMono, volumeSet.q15volumePilot);

		// volume damping sidetone, dtmf and mmi
		volume(rx.psPilot, volumeSet.q15volumePilotSidetone);
		volume(rx.psMmiSounds, volumeSet.q15volumeMMI);
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);
		volume(DTMFsetLs.psDTMFbuffer, volumeSet.q15volumeDTMF);

		// Add sidetone, dtmf and mmi to ls signal

		add_sidetone_dtmf_mmi(mic.psTechHeadset,DTMFsetLs.psDTMFbuffer,rx.psMmiSounds, ls.psTechMono, &limiterLsTechMono);
		// rx.psPilot should only be put in ls.psPilotMono if pilot is connected with headset to CA/BS.
		// If connected with aircraft cable No sidetone and MMI and DTMF should be generated.
		if (tvSettings.bPilotConnection == HEADSET) {
			add_sidetone_dtmf_mmi(rx.psPilot,DTMFsetLs.psDTMFbuffer, rx.psMmiSounds, ls.psPilotMono, &limiterLsPilotMono);
		}

	} else if(tvSettings.platform == TV_CONFIG_CA) {
		// volume damping ls
		volume(ls.psTechLeft,  volumeSet.q15volumeTech);
		volume(ls.psTechRight, volumeSet.q15volumeTech);
		volume(ls.psPilotMono, volumeSet.q15volumePilot);

		// volume damping sidetone
		volume(rx.psPilot, volumeSet.q15volumePilotSidetone);
		volume(rx.psMmiSounds, volumeSet.q15volumeMMI);
		volume(mic.psTechHeadset, volumeSet.q15volumeTechSidetone);

		// Add sidetone and mmi to ls signal
		add_sidetone_mmi(mic.psTechHeadset, rx.psMmiSounds, ls.psTechLeft, &limiterLsLeft_CA);
		add_sidetone_mmi(mic.psTechHeadset, rx.psMmiSounds, ls.psTechRight, &limiterLsRight_CA);
		// rx.psPilot should only be put in ls.psPilotMono if pilot is connected with headset to CA/BS.
		// If connected with aircraft cable No sidetone and MMI and DTMF should be generated.
		if (tvSettings.bPilotConnection == HEADSET) {
			add_sidetone_mmi(rx.psPilot, rx.psMmiSounds, ls.psPilotMono, &limiterLsPilotMono_CA);
		}

	}

	#ifdef USE_DUMPDATA
	dumpdata_post_dump();
	#endif
}

