/*****************************************************************************
 *	\file		pilot_switch.c
 *
 *	\date		2013-Nov-20
 *
 *	\brief		
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
#include "timers.h"

// ---< Defines >---

// ---< Global variabels >---
q15 q15Elmr;
bool bSpeechLMR;
timer_handle_t timer_LMR_VAD;
bool bLMRVadOverride;

// ---< Local function prototypes >---

// ---< Functions >---
/*****************************************************************************
 *	\brief		Calculates the energy in each input buffer to see which of them
 *				that contains speech.
 *	\parameters	pq15toTech	- Buffer from aircraft directed towards first tech
 * 				pq15toClt	- Buffer from aircraft directed towards StriC (CLT)
 *	\return		None
 *****************************************************************************/
void pilot_switch(q15 pq15toTech[], q15 pq15toClt[]) {
	int i;
	q15 Eclt = 0;
	q15 Etech = 0;
	//TODO: What should threshold t and alpha be when we have noise in the background?
	q15 alpha = 2*MAX_16/3;
	q15 t = 20; // threshold for speech
	q15 norm = MAX_16/BUFLEN_8KHZ;
	static q15 EoldClt = 0;
	static q15 EoldTech = 0;
	int Switch;

	// Save old pilot switch setting
	Switch = tvSettings.bPilotSwitch;

	// Calculate total energy in each buffer
	for (i=0; i<BUFLEN_8KHZ; i++) {
	    Eclt  = add_q15(Eclt,  mult_q15(pq15toClt[i], pq15toClt[i]));
	    Etech = add_q15(Etech, mult_q15(pq15toTech[i], pq15toTech[i]));
	}

	// Normalize energy
	Eclt  = mult_q15(Eclt, norm);
	Etech = mult_q15(Etech, norm);

	// Calculate a energy value based on the new and the old buffer.
    Eclt  = add_q15(mult_q15(Eclt, alpha) , mult_q15(EoldClt, sub_q15(MAX_16,alpha)));
    Etech = add_q15(mult_q15(Etech, alpha), mult_q15(EoldTech, sub_q15(MAX_16,alpha)));

    if (Eclt>=Etech && Eclt>t) {
   	   // Speech in Pilot-CLT Channel
       tvSettings.bPilotSwitch = 0;
    }

    if (Etech>Eclt && Etech>t) {
   		// Speech in Pilot-Tech Channel
   		tvSettings.bPilotSwitch = 1;
    }

    if (Switch != tvSettings.bPilotSwitch){
	   // Settings has changed
	   mixer_init();
	   EoldTech = 0;
	   EoldClt = 0;
	   lec1_init(true);
	   PRINT_DEBUG("Pilot switch has changed: %d\n", tvSettings.bPilotSwitch);
   }
   else{
	   EoldClt = Eclt;
	   EoldTech = Etech;
   }
}


/*****************************************************************************
 *	\brief		Calculates the energy in an input buffer and decides if speech or not
 *	\parameters	s			- Input signal
 *				Eold		- Previously calculated energy from last iteration.
 *				voxScale	- One for speech and 0 for mute
 *				timer		- timer handle that decides when to
 *	\return		None
 *****************************************************************************/
//Init
//	E = 0;
//	voxScale = 0;
//	timer;
//Run
//	resetTimerHandle(&timer, VAD_TIMER_NR);
//	energy_vad(signal, E, &voxScale, &timer);
void energy_vad(q15 s[], q15 *Eold, bool *bSpeech, timer_handle_t *timer) {
	int i;
	q15 Es = 0;
	//TODO: What should threshold t and alpha be when we have noise in the background?
	q15 alpha = 2*MAX_16/3;
	q15 thresh = 1000; // threshold for speech
	q15 norm = MAX_16/BUFLEN_8KHZ;

	// Calculate total energy in signal
	for (i=0; i<BUFLEN_8KHZ; i++) {
	    Es  = add_q15(Es,  mult_q15(s[i], s[i]));
	}

	// Normalize energy
	Es  = mult_q15(Es, norm);

	// Calculate a energy value based on the new and the old buffer.
    Es  = add_q15(mult_q15(Es, alpha) , mult_q15(*Eold, sub_q15(MAX_16,alpha)));

    //Check if speech
    if(Es > thresh){
		resetTimerHandle(timer, VAD_TIMER_LMR);
		*bSpeech = 1;
    }
    else{
		*bSpeech = isFinished(timer);
	}
}

void vad_lmr_init(){
	q15Elmr = 0;
	bSpeechLMR = false;
	resetTimerHandle(&timer_LMR_VAD, VAD_TIMER_LMR);
	bLMRVadOverride = false;
}

