/*****************************************************************************
 *	\file		volume.c
 *
 *	\date		2013-September-03
 *
 *	\brief		Function for damping signal volume
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

// ---< Defines >---

// ---< Global variabels >---

// Vector with values corresponding to 0,1,2,...,29,30 dB damping
q15 setVolume[31] = {32767,29204,26028,23197,20675,18426,
	                 16422,14636,13045,11626,10362, 9235,
	                  8231, 7336, 6538, 5827, 5193, 4628,
	                  4125, 3677, 3277, 2920, 2603, 2320,
	                  2067, 1843, 1642, 1464, 1304, 1163,
	                  1036};

volumeSet_t volumeSet;

// ---< Local function prototypes >---

// ---< Functions >---

/*****************************************************************************
 *	\brief		Initialize volume on loudspeaker outputs
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void volume_init() {
	// signals has 0 dB damping
	volumeSet.q15volumeTech = 0;		  	 // Volume setting for tech headset on BS, CA or PT
	volumeSet.q15volumePilot = 0;		 	 // Volume setting for pilot headset on BS or CA
	// sidetone and MMI has 15 dB damping
	volumeSet.q15volumeTechSidetone = 0;  	 // Volume setting for tech sidetone on BS, CA or PT
	volumeSet.q15volumePilotSidetone = 0;	 // Volume setting for pilot sidetone on BS or CA
	volumeSet.q15volumeDTMF = 0;		  	 // Volume setting for DTMF on ls on BS or PT
	volumeSet.q15volumeMMI = 0;		  		 // Volume setting for MMI sounds on BS, CA or PT
}

/*****************************************************************************
 *	\brief		Set volume damping level.
 *	\parameters	pq15buffer	  -	Buffer with signal that should be damped.
 *				volumeSetting -	Volume damping settings:
 *						 		0 for 0 dB damping
 *						 		1 for 1 dB damping
 *						 		...
 *						 		29 for 29 dB damping
 *						 		30 for 30 dB damping
 *	\return		None
 *****************************************************************************/
void volume(q15 pq15buffer[], q15 volumeSetting) {
	int k;

	for(k=0; k<BUFLEN_8KHZ; k++) {
		//multiply signal with volume setting
		pq15buffer[k] = mult_q15(pq15buffer[k], setVolume[volumeSetting]);

	}
	
}

/*****************************************************************************
 *	\brief		Add 3 signals to ls output.
 *	\parameters	pq15in	  -	Buffer with signal that should be added.
 *				pq15out	  -	Buffer with ls signal.
 *	\return		None
 *****************************************************************************/
void add_sidetone_dtmf_mmi(q15 pq15in1[], q15 pq15in2[], q15 pq15in3[], q15 pq15out[], limitSet16_t *limiterSet) {
	int i;
	q15 q15temp1, q15temp2, q15temp3, q15temp4;

	for (i=0; i<BUFLEN_8KHZ; i++) {
		q15temp1 = shr_q15(pq15in1[i], 2);
		q15temp2 = shr_q15(pq15in2[i], 2);
		q15temp3 = shr_q15(pq15in3[i], 2);
		q15temp4 = shr_q15(pq15out[i], 2);
		pq15out[i] = add_q15(q15temp1, add_q15(q15temp2, (add_q15(q15temp3, q15temp4))));
	}

	if(tvModules.bLsLimiter){
		limit_and_delay_16(pq15out, pq15out, limiterSet);
		for (i=0; i<BUFLEN_8KHZ; i++) {
				pq15out[i] = shl_q15(pq15out[i],2);
		}
	}

}

/*****************************************************************************
 *	\brief		Add 2 signals to ls output.
 *	\parameters	pq15in1	  -	Buffer with signal that should be added to ls.
 *				pq15in2	  -	Buffer with signal that should be added to ls.
 *				pq15out	  -	Buffer with ls signal.
 *	\return		None
 *****************************************************************************/
void add_sidetone_mmi(q15 pq15in1[], q15 pq15in2[], q15 pq15out[], limitSet16_t *limiterSet) {
	int i;
	q15 q15temp1, q15temp2, q15temp3;
	q15 q15norm = MAX_16/3;

	for (i=0; i<BUFLEN_8KHZ; i++) {
		q15temp1 = mult_q15(pq15in1[i], q15norm);
		q15temp2 = mult_q15(pq15in2[i], q15norm);
		q15temp3 = mult_q15(pq15out[i], q15norm);
		pq15out[i] = add_q15(q15temp1, add_q15(q15temp2, q15temp3));
	}

	if(tvModules.bLsLimiter){
		limit_and_delay_16(pq15out, pq15out, limiterSet);
		for (i=0; i<BUFLEN_8KHZ; i++) {
			pq15out[i] = pq15out[i]*3;

		}
	}
}
