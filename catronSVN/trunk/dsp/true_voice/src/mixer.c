/*****************************************************************************
 *	\file		mixer.c
 *
 *	\date		2013-May-28
 *
 *	\brief		Audio signal mixer for mono and stereo signals
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
#include <string.h>

// ---< Enumerations >---
eMixerIn_t eMixerIn;
eMixerOut_t eMixerOut;
//ePreMixerIn_t ePreMixerIn;
ePreMixerOut_t ePreMixerOut;
ePilotConnection_t ePilotConnetion;
ePilotSwitch_t ePilotSwitch;

// ---< Defines >---

// ---< Global variabels >---
// On/off state for each mixing matrix connection
bool ppbMixerActive[NUM_CH_MIXER_OUT][NUM_CH_MIXER_IN];
bool ppbPreMixerActive[NUM_CH_PRE_MIXER_OUT][NUM_CH_MIXER_IN];

// Normalization factors for each output channel in Mixer (depends on the number of active input channels)
q15 pq15MixerNormFactor[NUM_CH_MIXER_OUT];
// DeNormalization factors for each output channel (depends on the number of active input channels)
short psMixerDeNormFactor[NUM_CH_MIXER_OUT];

// Normalization factors for each output channel in Pre mixer (depends on the number of active input channels)
q15 pq15PreMixerNormFactor[NUM_CH_PRE_MIXER_OUT];
// DeNormalization factors for each output channel (depends on the number of active input channels)
short psPreMixerDeNormFactor[NUM_CH_PRE_MIXER_OUT];

//Buffer for sidetone
q15 pq15lsSidetone[BUFLEN_8KHZ];

// Array of pointers to audio signal buffers
q15 *ppq15mixerIn[NUM_CH_MIXER_IN];
q15 *ppq15mixerOut[NUM_CH_MIXER_OUT];

// Array of pointers to audio signal buffers
q15 *ppq15preMixerIn[NUM_CH_MIXER_IN];
q15 *ppq15preMixerOut[NUM_CH_PRE_MIXER_OUT];

#ifdef USE_DUMPDATA
extern q15 mixer_out_ave[NUM_CH_MIXER_OUT];
#endif
// ---< Local function prototypes >---
static void clear_output_channels(void);
static inline void mixer_channel(int, int);


// ---< Functions >---
/*****************************************************************************
 *	\brief		Routes external signal pointers to internal struct in pre mixer.
 * 				This has got to be run before the pre mixer function.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void pre_mixer_channel_setup(tvRxChannels_t rx, tvMicChannels_t mic, tvTxChannels_t tx, tvSpeakerChannels_t ls){
//	// Speaker pre mixer input
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH1]		= rx.psTech1;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH2]		= rx.psTech2;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH3]		= rx.psTech3;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH4]		= rx.psTech4;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH5]		= rx.psTech5;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH6]		= rx.psTech6;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH7]		= rx.psTech7;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_TECH8]		= rx.psTech8;
//	ppq15preMixerIn[PRE_MIXER_RX_IN_FTN]    	= rx.psFtn;

	// Speaker mixer input
	ppq15preMixerIn[MIXER_RX_IN_TECH1]		= rx.psTech1;
	ppq15preMixerIn[MIXER_RX_IN_TECH2]		= rx.psTech2;
	ppq15preMixerIn[MIXER_RX_IN_TECH3]		= rx.psTech3;
	ppq15preMixerIn[MIXER_RX_IN_TECH4]		= rx.psTech4;
	ppq15preMixerIn[MIXER_RX_IN_TECH5]		= rx.psTech5;
	ppq15preMixerIn[MIXER_RX_IN_TECH6]		= rx.psTech6;
	ppq15preMixerIn[MIXER_RX_IN_TECH7]		= rx.psTech7;
	ppq15preMixerIn[MIXER_RX_IN_TECH8]		= rx.psTech8;
	ppq15preMixerIn[MIXER_RX_IN_MMI_FX]		= rx.psMmiSounds;

	// On PT these are taken direct from radio input
	// on BS these are taken from "line" and processed in the same buffer AFTER pre mixer
	// On CA they are not used
	ppq15preMixerIn[MIXER_RX_IN_LMR]		= rx.psLmr;
	ppq15preMixerIn[MIXER_RX_IN_FTN]		= rx.psFtn;
	ppq15preMixerIn[MIXER_RX_IN_CLT]		= rx.psClt;

	// On PT radio input
	// if BS or CA one of mic.psPilotCable and mic.psPilotHeadset is put in rx.psPilot buffer AFTER pre mixer
	ppq15preMixerIn[MIXER_RX_IN_PILOT]		= rx.psPilot;

	// Mic signal from tech headset on PT, BS, or CA
	ppq15preMixerIn[MIXER_RX_IN_MIC_TECH]	= mic.psTechHeadset;

	// DTMF (internal signals inside true_voice)
	ppq15preMixerIn[MIXER_IN_DTMF_LS] 		= DTMFsetLs.psDTMFbuffer;
	ppq15preMixerIn[MIXER_IN_DTMF_LINE]		= DTMFset.psDTMFbuffer;

	// Pre mixer out
	ppq15preMixerOut[MIXER_LS_OUT_PILOT_CABLE] 	 = ls.psPilotMonoCable;
}
/*****************************************************************************
 *	\brief		Initializes audio signal pre mixer
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void pre_mixer_init(){
	int iOut, iIn;
	int iNrInChannels;

	// Reset all channel mixes
	for(iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++) {
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			ppbPreMixerActive[iOut][iIn] = false;
		}
	}

	switch(tvSettings.platform) {

		case TV_CONFIG_PT:
		break;

		case TV_CONFIG_BS:
			ppbPreMixerActive[MIXER_LS_OUT_PILOT_CABLE][tvSettings.enMixer1stTechChannel] = true;
			ppbPreMixerActive[MIXER_LS_OUT_PILOT_CABLE][MIXER_RX_IN_FTN] = true;
		break;

		case TV_CONFIG_CA:
			// TODO: tx-select on CA? -> use tvSettings.enMixer1stTechChannel (no 1st tech is heard by everybody all the time)
			// ppbPreMixerActive[MIXER_LS_OUT_PILOT_CABLE][tvSettings.enMixer1stTechChannel] = true;
			ppbPreMixerActive[MIXER_LS_OUT_PILOT_CABLE][MIXER_RX_IN_MIC_TECH] = true;

		break;

		default:
		break;

	}

	// Remove unused input channels (from user selection)
	for(iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++) {
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			ppbPreMixerActive[iOut][iIn] *= tvSettings.bMixerInChannelActive[iIn];
		}
	}
	// Selection of 1st technician output. Only BS is affected
	ppbPreMixerActive[MIXER_LS_OUT_PILOT_CABLE][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LS_PILOT];

	// Calculate normalization factor for each output channel
	for(iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++) {
		iNrInChannels = 0;
		// Count nr of active input channels
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			iNrInChannels += ppbPreMixerActive[iOut][iIn];
		}
		if(0 != iNrInChannels) {
			pq15PreMixerNormFactor[iOut] = MAX_16/iNrInChannels;
			psPreMixerDeNormFactor[iOut] = iNrInChannels;
		} else {
			// No input channels
			pq15PreMixerNormFactor[iOut] = 0;
		}
	}
	//limiter_init_pre_mixer();
	for (iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++){
		limiter_pre_mixer_Out[iOut].q15LIMIT_CLIP_LVL	= mult_q15(pq15PreMixerNormFactor[iOut],0x7a00);
	}
}

/*****************************************************************************
 *	\brief		Mix signal to send to aircraft via cable J8. Pre mixer is needed
 *				so LEC can be done before main mixer.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void pre_aircraft_mixer() {
	int iOut, iIn;
	int k;
	q15 q15temp;

	// clear output buffers
	for(k=0; k<NUM_CH_PRE_MIXER_OUT; k++) {
		memset(ppq15preMixerOut[k], 0, sizeof(q15)*BUFLEN_8KHZ);
	}

	for(iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++) {
		// Only do mixing if we have any input for this output channel
		if(0 != pq15PreMixerNormFactor[iOut]) {
			for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
				if(ppbPreMixerActive[iOut][iIn]) {
					// Add input channel to output channel

					//mixer_channel(iIn, iOut);
					for(k=0; k<BUFLEN_8KHZ; k++) {
						// Apply normalization factor to input signal
						q15temp = mult_q15(ppq15preMixerIn[iIn][k], pq15PreMixerNormFactor[iOut]);

						//PRINT_DEBUG("iIn=%2d\tk=%2d\t%d * %d = %d\n", iIn, k, ppq15preMixerIn[iIn][k], pq15PreMixerNormFactor[iOut], q15temp);

						// Accumulate signals
						ppq15preMixerOut[iOut][k] = add_q15(ppq15preMixerOut[iOut][k], q15temp);
					}
				}
			}
		}
	}

	if(tvModules.bLsLimiter){
		// limit and delay signals with normalization treshold
		for(iOut=0; iOut<NUM_CH_PRE_MIXER_OUT; iOut++) {
			if(0 != pq15PreMixerNormFactor[iOut]) {
				limit_and_delay_16(ppq15preMixerOut[iOut], ppq15preMixerOut[iOut], &limiter_pre_mixer_Out[iOut]);
				for(k=0; k<BUFLEN_8KHZ; k++) {
					//amplify signals with 1/pq15MixerNormFactor
					ppq15preMixerOut[iOut][k] = (ppq15preMixerOut[iOut][k]*psPreMixerDeNormFactor[iOut]);
				}
			}
		}
	}

}


/*****************************************************************************
 *	\brief		Initializes audio signal mixer
 *	\parameters	None 
 *	\return		None
 *****************************************************************************/
void mixer_init() {
	int iOut, iIn;
	int iNrInChannels;
	
	// Reset all channel mixes
	for(iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++) {
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			ppbMixerActive[iOut][iIn] = false;
		}
	}
	// Enable channel mix
	switch(tvSettings.platform) {
		
		case TV_CONFIG_PT:
		// Left front: All other technicians
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH1] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH2] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH3] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH4] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH5] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH6] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH7] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_TECH8] = false; //we are channel 8 (we don't want to hear ourself)
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][tvSettings.enMixer1stTechChannel] = false; // 1st tech is in left back instead
		// Assume that tech8 channel is empty on PT (this device creates the 8th channel)
		
		// Left back
		if(tvSettings.bMixerUserIs1stTech) {
			ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_LMR] = true; // LMR
		} else {
			//TBD?
		}
		
		// Right front:
		if(tvSettings.bMixerUserIs1stTech) {
			ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_FRONT][MIXER_RX_IN_FTN] = true; //FTN
		} else {
			ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_FRONT][tvSettings.enMixer1stTechChannel] = true; // 1st technician
		}
		
		// Right back: CLT
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_CLT] = true;
		if(tvSettings.bMixerUserIs1stTech) {
			// Pilot should also be heard. CLT is added into this channel in the BS so rx.Pilot is Pilot+CLT
			ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_PILOT] = true;
			// The CLT channel is not used since CLT is added to the pilot channel in the BS
			ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_CLT] = false;
		} else {
			// Nothing extra
		}
		
		// Centered: Side tone and sound effects should have no direction
		//ppbMixerActive[MIXER_LS_OUT_TECH_CENTER][MIXER_RX_IN_MIC_TECH] = true; // sidetone not in mixer, separate volume setting
		//ppbMixerActive[MIXER_LS_OUT_TECH_CENTER][MIXER_RX_IN_MMI_FX] = true; //MMI not in mixer, separate volume setting
		
		// Technician mono: Not connected to PT
		// Pilot: Not connected to PT
		// LMR: Not connected to PT
		// FTN: Not connected to PT
		// CLT: Not connected to PT

		// Radio:
		ppbMixerActive[MIXER_TX_RADIO_OUT_TECH][MIXER_RX_IN_MIC_TECH] = true;

		// DTMF:
		//ppbMixerActive[MIXER_LS_OUT_TECH_CENTER][MIXER_IN_DTMF_LS] = true; DTMF on ls not in mixer, separate volume setting

		break;
		
		case TV_CONFIG_BS:
		// Left front: Not connected to BS
		// Left back: Not connected to BS
		// Right front: Not connected to BS
		// Right back: Not connected to BS
		// Centered: Not connected to BS
		
		// Technician mono: 
		ppbMixerActive[MIXER_LS_OUT_TECH_MONO][tvSettings.enMixer1stTechChannel] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_RX_IN_LMR] = false; // only 1st tech in PT
		ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_RX_IN_FTN] = true;
		// TODO: should pilot headset connected to BS be in tech mono headset?
		if (tvSettings.bPilotConnection == HEADSET) {
			ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_RX_IN_PILOT] = true;
		}
		//ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_RX_IN_MMI_FX] = true; //MMI not in mixer, separate volume setting
		//ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_RX_IN_MIC_TECH] = true; //sidetone not in mixer, separate volume setting
		
		// Pilot:
		// Case 1, headset connected to BS via headset jack:
		if (tvSettings.bPilotConnection == HEADSET) {
			ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][tvSettings.enMixer1stTechChannel] = true;
			ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_FTN] = true;
			// TODO: should tech headset connected to BS be in pilot mono headset?
			ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_MIC_TECH] = true;
			//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_MMI_FX] = true; //MMI not in mixer, separate volume setting
			//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_PILOT] = true; //sidetone not in mixer, separate volume setting
		}

		// Case 2, connected to BS from aircraft via CLT cable J9:
		ppbMixerActive[MIXER_LS_OUT_PILOT_CLT_CABLE][MIXER_RX_IN_CLT] = true;
		
		// Case 3, connected to BS from aircraft via aircraft cable J8:
		// [MIXER_LS_OUT_PILOT_CABLE] mixed in pre_aircraft_mixer due to LEC.

		// LMR: 1st tech
		ppbMixerActive[MIXER_TX_LINE_OUT_LMR][tvSettings.enMixer1stTechChannel] = true;
		ppbMixerActive[MIXER_TX_LINE_OUT_LMR][MIXER_RX_IN_MIC_TECH] = false; // only 1st tech in PT

		// FTN:
		ppbMixerActive[MIXER_TX_LINE_OUT_FTN][tvSettings.enMixer1stTechChannel] = true;
		ppbMixerActive[MIXER_TX_LINE_OUT_FTN][MIXER_RX_IN_MIC_TECH] = true;

		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE && tvSettings.bPilotSwitch == FIRST_TECH) {
			// TODO: in aircraft it is only true when having a three-part conference
			// and when the pilot switch is for communication with 1st tech.
			ppbMixerActive[MIXER_TX_LINE_OUT_FTN][MIXER_RX_IN_PILOT] = true;
		}
		if (tvSettings.bPilotConnection == HEADSET) {
			ppbMixerActive[MIXER_TX_LINE_OUT_FTN][MIXER_RX_IN_PILOT] = true;
		}

		// CLT:
		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE && tvSettings.bPilotSwitch == CLT) {
			ppbMixerActive[MIXER_TX_LINE_OUT_CLT][MIXER_RX_IN_PILOT] = true;
		}
		if (tvSettings.bPilotConnection == HEADSET) {
			ppbMixerActive[MIXER_TX_LINE_OUT_CLT][MIXER_RX_IN_PILOT] = false;
		}
		
		// Radio:	
		ppbMixerActive[MIXER_TX_RADIO_OUT_LMR][MIXER_RX_IN_LMR] = true;
		ppbMixerActive[MIXER_TX_RADIO_OUT_CLT][MIXER_RX_IN_CLT] = true;
		ppbMixerActive[MIXER_TX_RADIO_OUT_FTN][MIXER_RX_IN_FTN] = true;
		ppbMixerActive[MIXER_TX_RADIO_OUT_TECH][MIXER_RX_IN_MIC_TECH] = true;

		// Pilot and Clt to radio (only 1st tech can hear this radio channel)
		if (tvSettings.bPilotConnection == HEADSET) {
			ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_PILOT] = true;
			ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_CLT] = true;
		}
		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE && tvSettings.bPilotSwitch == CLT) {
			ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_PILOT] = true;
			ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_CLT] = true;
		}
		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE && tvSettings.bPilotSwitch == FIRST_TECH) {
			ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_PILOT] = true;
		}

		// DTMF:
		//ppbMixerActive[MIXER_LS_OUT_TECH_MONO][MIXER_IN_DTMF_LS] = true; DTMF to ls not in mixer, separate volume setting
		//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_IN_DTMF_LS] = true; DTMF to ls not in mixer, separate volume setting
		ppbMixerActive[MIXER_TX_LINE_OUT_FTN][MIXER_IN_DTMF_LINE] = true;

		break;
		
		case TV_CONFIG_CA:

		// Tech connected to CA is by default first technician

		// Left Back:
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH1] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH2] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH3] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH4] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH5] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH6] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH7] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][MIXER_RX_IN_TECH8] = false; //we are channel 8 (we don't want to hear ourself)
//		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_BACK][tvSettings.enMixer1stTechChannel] = false; // tech on CA is first tech.
		// Assume that tech8 channel is empty on CA (this device creates the 8th channel)

		// Right back:
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH1] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH2] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH3] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH4] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH5] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH6] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH7] = true;
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][MIXER_RX_IN_TECH8] = false; //we are channel 8 (we don't want to hear ourself)
//		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_BACK][tvSettings.enMixer1stTechChannel] = false; // tech on CA is first tech.
		// Assume that tech8 channel is empty on CA (this device creates the 8th channel)
		
		// Left front:
		ppbMixerActive[MIXER_LS_OUT_TECH_LEFT_FRONT][MIXER_RX_IN_PILOT] = true;
		
		// Right front:
		ppbMixerActive[MIXER_LS_OUT_TECH_RIGHT_FRONT][MIXER_RX_IN_PILOT] = true;
		
		// Centered: Not connected to CA
		//ppbMixerActive[MIXER_LS_OUT_TECH_CENTER][MIXER_RX_IN_MMI_FX] = true; // MMI not in mixer, separate volume setting
		//ppbMixerActive[MIXER_LS_OUT_TECH_CENTER][MIXER_RX_IN_MIC_TECH] = true; //sidetone not in mixer, separate volume setting
		
		// Technician mono: Not connected to CA
		
		// Pilot: 
		//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_MMI_FX] = true;	// MMI not in mixer, separate volume setting
		//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_PILOT] = true;	// Sidetone not in mixer, separate volume setting

		// TODO: Has CA tx-selsect? -> use tvSettings.enMixer1stTechChannel? (now 1st tech is heard by everybody all the time)
		//ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][tvSettings.enMixer1stTechChannel] = true;
		ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][MIXER_RX_IN_MIC_TECH] = true;

		//ppbMixerActive[MIXER_LS_OUT_PILOT_CABLE][MIXER_RX_IN_MIC_TECH] = true; // mixed in Pre-Mixer
		
		// LMR: Not connected to CA
		// FTN: Not connected to CA
		// CLT: Not connected to CA
		
		// Radio
		ppbMixerActive[MIXER_TX_RADIO_OUT_TECH][MIXER_RX_IN_MIC_TECH] = true;
		//ppbMixerActive[MIXER_TX_RADIO_OUT_PILOT_AND_CLT][MIXER_RX_IN_PILOT] = true; // Not needed, 1st tech is tech on CA. Other techs should not hear pilot

		// DTMF: Not connected to CA

		break;
		
		default:
		break;
	}
	// Remove unused input channels (from user selection)
	for(iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++) {
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			ppbMixerActive[iOut][iIn] *= tvSettings.bMixerInChannelActive[iIn];
		}
	}
	// Selection of 1st technician output. Only BS is affected
	ppbMixerActive[MIXER_TX_LINE_OUT_LMR][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LMR];
	ppbMixerActive[MIXER_TX_LINE_OUT_FTN][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_FTN];
	//ppbMixerActive[MIXER_TX_LINE_OUT_CLT][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_CLT];
	ppbMixerActive[MIXER_LS_OUT_TECH_MONO][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LS_TECH];
	ppbMixerActive[MIXER_LS_OUT_PILOT_MONO][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LS_PILOT];
	ppbMixerActive[MIXER_LS_OUT_PILOT_CLT_CABLE][tvSettings.enMixer1stTechChannel] *= tvSettings.bMixer1stTechTxOutActive[TV_TECH1_TO_LS_PILOT];
	
	// Calculate normalization factor for each output channel
	for(iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++) {
		iNrInChannels = 0;
		// Count nr of active input channels
		for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
			iNrInChannels += ppbMixerActive[iOut][iIn];
		}
		if(0 != iNrInChannels) {
			pq15MixerNormFactor[iOut] = MAX_16/iNrInChannels;
			psMixerDeNormFactor[iOut] = iNrInChannels;
		} else {
			// No input channels
			pq15MixerNormFactor[iOut] = 0;
		}
	}
	//limiter_init_16();
	for (iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++){
		limiter_Out[iOut].q15LIMIT_CLIP_LVL= mult_q15(pq15MixerNormFactor[iOut],0x7a00);
	}
}
/*****************************************************************************
 *	\brief		Create pilot microphone signals from the processed headset
 * 				signal or from the aircraft cable. Output to rx.psPilot. Only
 * 				used on CA and BS.On PT only connection via radio is possible.
 *	\parameters	MicPilotHeadset - Microphone on pilot headset when connected to
 * 							   	  headset jack on BT or CA
 * 				MicPilotCable   - Microphone on pilot headset when connected via
 * 							      aircraft cable J8 to BS or CA
 * 				MicPilotCltCable- Microphone on pilot headset when connected via
 * 							      aircraft CLT cable J9 to BS
 * 				PilotToRadio	- Output
 *	\return		None
 *****************************************************************************/
void pilot_channel_selection(q15 pq15MicPilotHeadSet[], q15 pq15MicPilotCable[], q15 pq15MicPilotCltCable[], q15 pq15PilotToRadio[]){
	int k;
	
	if (tvSettings.platform == TV_CONFIG_BS) {
		
		//if (tvSettings.bPilotCable) {
		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE) {

			if (tvSettings.bPilotSwitch == FIRST_TECH) {
				for(k=0; k<BUFLEN_8KHZ; k++) {
					pq15PilotToRadio[k] = pq15MicPilotCable[k];
				}
			}
			if (tvSettings.bPilotSwitch == CLT) {
				for(k=0; k<BUFLEN_8KHZ; k++) {
					pq15PilotToRadio[k] = pq15MicPilotCltCable[k];
				}
			}
		}
		if (tvSettings.bPilotConnection == HEADSET) {
			
			for(k=0; k<BUFLEN_8KHZ; k++) {
				pq15PilotToRadio[k] = pq15MicPilotHeadSet[k];
			}
		}
	}

	if (tvSettings.platform == TV_CONFIG_CA) {

		//if (tvSettings.bPilotCable) {
		if (tvSettings.bPilotConnection == AIRCRAFT_CABLE) {
			for(k=0; k<BUFLEN_8KHZ; k++) {
				pq15PilotToRadio[k] = pq15MicPilotCable[k];
			}
		}
		if (tvSettings.bPilotConnection == HEADSET) {
			for(k=0; k<BUFLEN_8KHZ; k++) {
				pq15PilotToRadio[k] = pq15MicPilotHeadSet[k];
			}
		}
	}

	return;	
}

/*****************************************************************************
 *	\brief		Routes external signal pointers to internal struct in mixer.
 * 				This has got to be run before the mixer function.
 *	\parameters	None 
 *	\return		None
 *****************************************************************************/
void mixer_channel_setup(tvRxChannels_t rx, tvMicChannels_t mic, tvTxChannels_t tx, tvSpeakerChannels_t ls) {
	
	// Speaker mixer input
	ppq15mixerIn[MIXER_RX_IN_TECH1]		= rx.psTech1;
	ppq15mixerIn[MIXER_RX_IN_TECH2]		= rx.psTech2;
	ppq15mixerIn[MIXER_RX_IN_TECH3]		= rx.psTech3;
	ppq15mixerIn[MIXER_RX_IN_TECH4]		= rx.psTech4;
	ppq15mixerIn[MIXER_RX_IN_TECH5]		= rx.psTech5;
	ppq15mixerIn[MIXER_RX_IN_TECH6]		= rx.psTech6;
	ppq15mixerIn[MIXER_RX_IN_TECH7]		= rx.psTech7;
	ppq15mixerIn[MIXER_RX_IN_TECH8]		= rx.psTech8;
	ppq15mixerIn[MIXER_RX_IN_MMI_FX]	= rx.psMmiSounds;

	// On PT these are taken direct from radio input
	// on BS these are taken from "line" and processed in the same buffer
	// On CA they are not used
	ppq15mixerIn[MIXER_RX_IN_LMR]		= rx.psLmr;
	ppq15mixerIn[MIXER_RX_IN_FTN]		= rx.psFtn;
	ppq15mixerIn[MIXER_RX_IN_CLT]		= rx.psClt;

	// On PT only connection via radio is possible
	// if BS or CA one of mic.psPilotCable and mic.psPilotHeadset is put in rx.psPilot buffer.
	ppq15mixerIn[MIXER_RX_IN_PILOT]		= rx.psPilot;

	// Mic signal from tech headset on PT, BS, or CA
	ppq15mixerIn[MIXER_RX_IN_MIC_TECH]	= mic.psTechHeadset;
	
	// DTMF (internal signals inside true_voice)
	ppq15mixerIn[MIXER_IN_DTMF_LS] 		= DTMFsetLs.psDTMFbuffer;
	ppq15mixerIn[MIXER_IN_DTMF_LINE]	= DTMFset.psDTMFbuffer;

	// Speaker mixer output
	ppq15mixerOut[MIXER_LS_OUT_TECH_LEFT_FRONT]		= pq15lsTechLeftFront;
	ppq15mixerOut[MIXER_LS_OUT_TECH_LEFT_BACK]		= pq15lsTechLeftBack;
	ppq15mixerOut[MIXER_LS_OUT_TECH_RIGHT_FRONT]	= pq15lsTechRightFront;
	ppq15mixerOut[MIXER_LS_OUT_TECH_RIGHT_BACK]		= pq15lsTechRightBack;
	ppq15mixerOut[MIXER_LS_OUT_TECH_CENTER]			= pq15lsTechCenter;
	ppq15mixerOut[MIXER_LS_OUT_TECH_MONO]			= ls.psTechMono;
	ppq15mixerOut[MIXER_LS_OUT_PILOT_MONO]			= ls.psPilotMono;
	//ppq15mixerOut[MIXER_LS_OUT_PILOT_CABLE]		= ls.psPilotMonoCable; //in pre mixer
	ppq15mixerOut[MIXER_LS_OUT_PILOT_CLT_CABLE]		= ls.psPilotMonoCltCable;
	ppq15mixerOut[MIXER_TX_LINE_OUT_LMR]			= tx.psLmrToLine;
	ppq15mixerOut[MIXER_TX_LINE_OUT_FTN]			= tx.psFtnToLine;
	ppq15mixerOut[MIXER_TX_LINE_OUT_CLT]			= tx.psCltToLine;
	ppq15mixerOut[MIXER_TX_RADIO_OUT_LMR]			= tx.psLmrToRadio;
	ppq15mixerOut[MIXER_TX_RADIO_OUT_FTN]			= tx.psFtnToRadio;
	ppq15mixerOut[MIXER_TX_RADIO_OUT_CLT]			= tx.psCltToRadio;
	ppq15mixerOut[MIXER_TX_RADIO_OUT_PILOT_AND_CLT]	= tx.psPilotAndCltToRadio;
	ppq15mixerOut[MIXER_TX_RADIO_OUT_TECH]			= tx.psTechToRadio;
	
}


/*****************************************************************************
 *	\brief		Creates speaker signals and line output signals from input
 * 				rx channels and processed mic signals.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void mixer() {
	int iOut, iIn;
	int k;
	
	// Clear output buffers
	clear_output_channels();
	
	for(iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++) {
		// Only do mixing if we have any input for this output channel
		if(0 != pq15MixerNormFactor[iOut]) {
			for(iIn=0; iIn<NUM_CH_MIXER_IN; iIn++) {
				if(ppbMixerActive[iOut][iIn]) {
					// Add input channel to output channel
					mixer_channel(iIn, iOut);
				}
			}
		}	
	}
	
	if(tvModules.bLsLimiter){
		// limit and delay signals with normalization threshold
		for(iOut=0; iOut<NUM_CH_MIXER_OUT; iOut++) {
			if(0 != pq15MixerNormFactor[iOut]) {
				#ifdef USE_DUMPDATA
					mixer_out_ave[iOut]=1;//abs(ppq15mixerOut[iOut][0]);
				#endif
				limit_and_delay_16(ppq15mixerOut[iOut], ppq15mixerOut[iOut], &limiter_Out[iOut]);
				for(k=0; k<BUFLEN_8KHZ; k++) {
					//amplify signals with 1/pq15MixerNormFactor
					ppq15mixerOut[iOut][k] = (ppq15mixerOut[iOut][k]*psMixerDeNormFactor[iOut]);
				}
			}
		}
	}
}

/*****************************************************************************
 *	\brief		Adds an input channel to an output channel. Normalization
 * 				for the specific output channel is also performed.
 *	\parameters	iIn		- Input channel index
 * 				iOut	- Output channel index
 *	\return		None
 *****************************************************************************/
static inline void mixer_channel(int iIn, int iOut) {
	int k;
	q15 q15temp;
	
	for(k=0; k<BUFLEN_8KHZ; k++) {
		// Apply normalization factor to input signal
		q15temp = mult_q15(ppq15mixerIn[iIn][k], pq15MixerNormFactor[iOut]);

		//PRINT_DEBUG("iIn=%2d\tk=%2d\t%d * %d = %d\n", iIn, k, ppq15mixerLsIn[iIn][k], q15norm, q15temp);
		
		// Accumulate signals
		ppq15mixerOut[iOut][k] = add_q15(ppq15mixerOut[iOut][k], q15temp);
	}
}


/*****************************************************************************
 *	\brief		Clear speaker mixer output channels
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
static void clear_output_channels() {
	int i;
	
	for(i=0; i<NUM_CH_MIXER_OUT; i++) {
		memset(ppq15mixerOut[i], 0, sizeof(q15)*BUFLEN_8KHZ);
	}
}
