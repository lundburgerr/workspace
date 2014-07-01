/*****************************************************************************
 *	\file		dtmf.c
 *
 *	\date		2013-August-05
 *
 *	\brief		Generation and detection of tones
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
#include <math.h> // sin()
#if USE_DEBUG_PLOT
#include "plot.h" // Should not be included here. Used for DTMF testing
#endif

// ---< Defines >---
#define DTMF_QUEUE_LENGTH 32

// ---< Global variables >---
// Memory allocation for DTMF buffers
q15 pq15DTMFtoLs[BUFLEN_8KHZ];
q15 pq15DTMFtoLine[BUFLEN_8KHZ];

// ----- DTMF -----
//	 Hz	|1209|1336| 1477|1633|
//	697	| 1	 | 2  |	 3	| A  |
//	770	| 4	 | 5  |	 6	| B  |
//	852	| 7	 | 8  |	 9  | C  |
//	941	| *	 | 0  |  #  | D  |
// DTMF-tones for rows [1,2,3,4]
const unsigned int uiDTMF_row[4] = {697, 770, 852, 941};
// DTMF-tones for colums [1,2,3,4]
const unsigned int uiDTMF_col[4] = {1209, 1336, 1477, 1633};

// ----- DTMF queue -----
// How many DTMF structs should we be able to queue
//extern DTMF_QUEUE_LENGTH 32;

DTMFqueue_t DTMFqueue[DTMF_QUEUE_LENGTH];
DTMFqueue_t DTMFqueueLs[DTMF_QUEUE_LENGTH];
DTMFset_t DTMFset;
DTMFset_t DTMFsetLs;

// ---< Local function prototypes >---
void DTMFqueue_update(DTMFqueue_t *,  DTMFset_t *);
void generate_dtmf(q15 *x, DTMFset_t *);
void sin_generator(q15 *n, q15 sinBuffer[], q15 q15fs, q15 q15frequency);
void test_DTMF_temp(void);

// ---< Functions >---
/*****************************************************************************
 *	\brief		Sine wave generator.
 *				TODO: Optimize sin generator?
 *	\parameters	n - time (sample we are at at the moment)
 *				sinBuffer[] - buffer to hold tone
 *				q15fs - sample rate
 *				q15frequency - frequency of sine wave
 *	\return		None
 *****************************************************************************/
void sin_generator(q15 *n, q15 q15sinBuffer[], q15 q15fs, q15 q15frequency) {
	int i;
	q15 q15amplitude = 16384; // MAX_16/2. (generate_DTMF adds two sin waves.)
	for (i=0; i<+BUFLEN_8KHZ;i++){
		q15sinBuffer[i] = (q15)(q15amplitude * sin((2 * PI * *n * q15frequency) / q15fs));
		*n += 1;
	}
	return;
}

/*****************************************************************************
 *	\brief		Initialize DTMF queue
 *	\parameters
 *	\return		None
 *****************************************************************************/
void DTMF_init(void) {
	int i;

	// Mark entry as empty
	for(i=0; i<DTMF_QUEUE_LENGTH; i++) {
		DTMFqueue[i].c = 'e';
	}

	for(i=0; i<DTMF_QUEUE_LENGTH; i++) {
		DTMFqueueLs[i].c = 'e';
	}

	DTMFset.bSendQueue = false;		// Should queue be generated and sent to buffer
	DTMFset.q15DTMFindexIn = 0;		// Init where in the queue DMTF should first be written
	DTMFset.q15lengthTone = 0;		// No samples in queue
	DTMFset.q15lengthSilence = 0;	// No samples in queue
	DTMFset.q15timeIndex1 = 0;		// Init time index for sin generator tone 1
	DTMFset.q15timeIndex2 = 0;		// Init time index for sin generator tone 2
	DTMFset.q15timeTone = 80;		// Default time for DTMF tone
	DTMFset.q15timeSilence = 80;	// Default time for DTMF pause
	DTMFset.psDTMFbuffer = pq15DTMFtoLine; // Pointer to audio buffer for line

	DTMFsetLs.bSendQueue = true;	// Should queue be generated and sent to buffer
	DTMFsetLs.q15DTMFindexIn = 0;	// Init where in the queue DMTF should first be written
	DTMFsetLs.q15lengthTone = 0;	// No samples in queue
	DTMFsetLs.q15lengthSilence = 0;	// No samples in queue
	DTMFsetLs.q15timeIndex1 = 0;	// Init time index for sin generator tone 1
	DTMFsetLs.q15timeIndex2 = 0;	// Init time index for sin generator tone 2
	DTMFsetLs.q15timeTone = 80;		// Default time for DTMF tone
	DTMFsetLs.q15timeSilence = 80;	// Default time for DTMF pause
	DTMFsetLs.psDTMFbuffer = pq15DTMFtoLs; // Pointer to audio buffer for LS-output
}

/*****************************************************************************
 *	\brief		TODO: remove this function. only for testing DTMF without using
 *				the interface functions.
 *	\parameters
 *	\return		None
 *****************************************************************************/
void test_DTMF_temp() {
	static unsigned int i=0;

	if (i==0){
		DTMF_queue('A' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('2' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('A' , 80, 80,DTMFqueueLs,&DTMFsetLs);
		DTMF_queue('2' , 80, 80,DTMFqueueLs,&DTMFsetLs);
		DTMF_queue('A' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('2' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('A' , 80, 80,DTMFqueueLs,&DTMFsetLs);
		DTMF_queue('2' , 80, 80,DTMFqueueLs,&DTMFsetLs);
	}

	if (i<80){
		DTMFset.bSendQueue = false;
		//DTMFsetLs.bSendQueue = false;
	}
	else {
		DTMFset.bSendQueue = true;
		//DTMFsetLs.bSendQueue = true;
	}
//	if (i<10){
//		DTMFset.bSendQueue = true;
//		//DTMFsetLs.bSendQueue = false;
//	}
//	else {
//		//DTMFset.bSendQueue = true;
//		//DTMFsetLs.bSendQueue = true;
//	}

	if (i>50){
		dtmf_remove_last_added(DTMFqueue, &DTMFset);
		dtmf_remove_last_added(DTMFqueueLs, &DTMFsetLs);
	}

	send_dtmf(pq15DTMFtoLine,DTMFqueue, &DTMFset);
	send_dtmf(pq15DTMFtoLs, DTMFqueueLs, &DTMFsetLs);

	#if (USE_DEBUG_PLOT)
		write_plot_data(pq15DTMFtoLine, &plotVar1, BUFLEN_8KHZ);
		write_plot_data(pq15DTMFtoLs, &plotVar2, BUFLEN_8KHZ);
	#endif

	/*if (i%80==0) {
		DTMF_queue('A' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('A' , 80, 80,DTMFqueueLs,&DTMFsetLs);
	}*/
/*	if (i%30==0) {
		DTMF_queue('*' , 80, 80,DTMFqueueLs,&DTMFsetLs);
	}*/

/*	if (i==50) {
		dtmf_clear_queue(DTMFqueue, &DTMFset);
		dtmf_clear_queue(DTMFqueueLs, &DTMFsetLs);
	}*/

	if (i==200){
		DTMF_queue('2' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('2' , 80, 80,DTMFqueueLs,&DTMFsetLs);
		DTMF_queue('2' , 80, 80,DTMFqueue,&DTMFset);
		DTMF_queue('2' , 80, 80,DTMFqueueLs,&DTMFsetLs);

	}

	i += 1;

}

/*****************************************************************************
 *	\brief		Generates and send DTMF tones
 *	\parameters	q15DTMFbuffer - Buffer for DTMF tone
 *				DTMFtoneQueue - Struct with DTMF tones queued
 *				DTMFinfo 	  - Struct with info about the DTMF tone that is 1st in the queue.
 *	\return		None
 *****************************************************************************/
void send_dtmf(q15 *q15DTMFbuffer, DTMFqueue_t *DTMFtoneQueue, DTMFset_t *DTMFinfo) {
	int i;

	// Send signals?
	if (DTMFinfo->bSendQueue == true) {

		// Update DTMF queue
		DTMFqueue_update(DTMFtoneQueue, DTMFinfo);

		// check if the previous tone has finished
		if(DTMFinfo->q15lengthTone >= 1) {
			DTMFinfo->bGenerateTone = true;
			DTMFinfo->bGenerateSilence = false;
			generate_dtmf(q15DTMFbuffer, DTMFinfo);
			DTMFinfo->q15lengthTone = DTMFinfo->q15lengthTone - BUFLEN_8KHZ;
		}
		else if (DTMFinfo->q15lengthSilence >= 1){
			DTMFinfo->bGenerateTone = false;
			DTMFinfo->bGenerateSilence = true;
			generate_dtmf(q15DTMFbuffer, DTMFinfo);
			DTMFinfo->q15lengthSilence = DTMFinfo->q15lengthSilence - BUFLEN_8KHZ;
		}
		else {
			// TODO: correct to do this?
			for (i=0; i<BUFLEN_8KHZ; i++) {
				q15DTMFbuffer[i] = 0;
			}
		}
	}
	else {
		for (i=0; i<BUFLEN_8KHZ; i++) {
			q15DTMFbuffer[i] = 0;
		}
	}
}

/*****************************************************************************
 *	\brief		Generates and adds DTMF-tones to provided sound buffer.
 *				Valid characters for DTMF tone generation:
 *					   '1', '2', '3', 'A',
 *					   '4', '5', '6', 'B',
 *					   '7', '8', '9', 'C',
 *					   '*', '0', '#', 'D'
 *	\parameters	pq15x - Sound buffer for DTMF tone
 *				DTMFbuff - Struct with info about the DTMF tone that is 1st in the queue.
 *	\return		None
 *****************************************************************************/
void generate_dtmf(q15 *pq15x, DTMFset_t *DTMFbuff) {
	unsigned int uiTone1, uiTone2;
	char cKey;
	int i;
	q15 q15fs = 8000;
	q15 pq15sinBufferTone1[BUFLEN_8KHZ];
	q15 pq15sinBufferTone2[BUFLEN_8KHZ];

	cKey = DTMFbuff->cKey;

	// Create and apply tone
	if(DTMFbuff->bGenerateTone) {
		// Which row?
		if((cKey=='1') || (cKey=='2') || (cKey=='3') || (cKey=='A')) {
			uiTone1 = uiDTMF_row[0];
		} else if((cKey=='4') || (cKey=='5') || (cKey=='6') || (cKey=='B')) {
			uiTone1 = uiDTMF_row[1];
		} else if((cKey=='7') || (cKey=='8') || (cKey=='9') || (cKey=='C')) {
			uiTone1 = uiDTMF_row[2];
		} else if((cKey=='*') || (cKey=='0') || (cKey=='#') || (cKey=='D')) {
			uiTone1 = uiDTMF_row[3];
		} else {
			// Error, but should be checked in dtmf_queue
		}

		// Which column?
		if((cKey=='1') || (cKey=='4') || (cKey=='7') || (cKey=='*')) {
			uiTone2 = uiDTMF_col[0];
		} else if((cKey=='2') || (cKey=='5') || (cKey=='8') || (cKey=='0')) {
			uiTone2 = uiDTMF_col[1];
		} else if((cKey=='3') || (cKey=='6') || (cKey=='9') || (cKey=='#')) {
			uiTone2 = uiDTMF_col[2];
		} else if((cKey=='A') || (cKey=='B') || (cKey=='C') || (cKey=='D')) {
			uiTone2 = uiDTMF_col[3];
		} else {
			// Error, but should be checked in dtmf_queue
		}

		sin_generator(&DTMFbuff->q15timeIndex1, pq15sinBufferTone1, q15fs, uiTone1);
		sin_generator(&DTMFbuff->q15timeIndex2, pq15sinBufferTone2, q15fs, uiTone2);

		// Generate and apply tones
		for(i=0; i<BUFLEN_8KHZ; i++) {
			pq15x[i] = add_q15(pq15sinBufferTone1[i], pq15sinBufferTone2[i]);
		}
	}

	if(DTMFbuff->bGenerateSilence) {
		for(i=0; i<BUFLEN_8KHZ; i++) {
			pq15x[i] = 0;
			DTMFbuff->q15timeIndex1 = 0;
			DTMFbuff->q15timeIndex2 = 0;
		}
	}

}


/*****************************************************************************
 *	\brief		Stores DTMF characters and length of tone and silence in a queue.
 *	\parameters	c 		      - Character to store in queue. Valid characters:
 *						  		  '1', '2', '3', 'A',
 *						 		  '4', '5', '6', 'B',
 *								  '7', '8', '9', 'C',
 *								  '*', '0', '#', 'D'
 *				q15timeTone	  -	Time in ms that the DTMF tone should be generated
 *				q15timeSilece - Time in ms of silence that should be generated after the tone
 *				DTMFtoneQueue - Struct with DTMF tones queued
 *				DTMFbuff 	  - Struct with info about which place in queue the character should be inserted at.
 *	\return		returns 0 upon success, -1 otherwise
 *****************************************************************************/
int DTMF_queue(char c, q15 q15timeTone, q15 q15timeSilence, DTMFqueue_t *DTMFtoneQueue, DTMFset_t *DTMFbuff) {
	q15 q15fs = 8000;

	// Check if correct input and if the previous tone has finished
	if( (c >= '0' && c <= '9') || c == '*' || c == '#' || (c >= 'A' && c <= 'D') ) {

		// Decide position to store
		// If current DTMF is not finished, queue entry
		if( 'e' != DTMFtoneQueue[DTMFbuff->q15DTMFindexIn].c ) {
			// Increase position
			if(++DTMFbuff->q15DTMFindexIn >= DTMF_QUEUE_LENGTH) {
				DTMFbuff->q15DTMFindexIn = 0;
				// Some kind of error message is needed...
			}
		}

		// --- Store info in the correct position ---
		// Store character
		DTMFtoneQueue[DTMFbuff->q15DTMFindexIn].c = c;
		// Store time, convert from ms to samples
		DTMFtoneQueue[DTMFbuff->q15DTMFindexIn].q15lengthTone = (q15timeTone*q15fs)/1000;
		DTMFtoneQueue[DTMFbuff->q15DTMFindexIn].q15lengthSilence = (q15timeSilence*q15fs)/1000;

		return(0);
	}
	else {
		// Bad character input
		return(-1);
	}
}

/*****************************************************************************
 *	\brief		Performs a check if next DTMF should be generated. If so, it reads
 *				the DTMF info to be generated from the DTMF queue.
 *	\parameters DTMFtoneQueue - Struct with DTMF tones queued
 *				DTMFbuff 	  - Struct with info about the DTMF tone that is 1st in the queue.
 *	\return		None
 *****************************************************************************/
void DTMFqueue_update(DTMFqueue_t *DTMFtoneQueue, DTMFset_t *DTMFbuff) {
	q15 i;

	// Is the current DTMF finished
	if(DTMFbuff->q15lengthTone <= 1 && DTMFbuff->q15lengthSilence <= 1) {

		if('e' == DTMFtoneQueue[0].c) {
			// No tone in queue, don't update
			DTMFset.bSendQueue = false; // Todo: is this correct?
		}
		else {
			// Store next tone in buffers
			DTMFbuff->cKey = DTMFtoneQueue[0].c;
			DTMFbuff->q15lengthTone = DTMFtoneQueue[0].q15lengthTone;
			DTMFbuff->q15lengthSilence = DTMFtoneQueue[0].q15lengthSilence;

			// Set queue position to empty
			DTMFtoneQueue[0].c = 'e';

			// Rearrange queue
			for(i=1; i<DTMF_QUEUE_LENGTH; i++) {
				// Is entry empty ('e')
				if('e' != DTMFtoneQueue[i].c) {
					// Move up one step in queue
					DTMFtoneQueue[i-1].c = DTMFtoneQueue[i].c;
					DTMFtoneQueue[i-1].q15lengthTone = DTMFtoneQueue[i].q15lengthTone;
					DTMFtoneQueue[i-1].q15lengthSilence = DTMFtoneQueue[i].q15lengthSilence;

					// Set queue position to empty
					DTMFtoneQueue[i].c = 'e';
				}
			}

			// Decrease queue input index
			if(0 > --DTMFbuff->q15DTMFindexIn) {
				DTMFbuff->q15DTMFindexIn = 0;
			}
		}
	}
}

/*****************************************************************************
 *	\brief		Remove all characters currently in queue
 *	\parameters DTMFtoneQueue - Struct with DTMF tones queued
 *				DTMFinfo 	  - Struct with info about the DTMF tone that is 1st in the queue.
 *	\return		None
 *****************************************************************************/
void dtmf_clear_queue(DTMFqueue_t *DTMFtoneQueue, DTMFset_t *DTMFinfo){
	int i;

	if('e' == DTMFtoneQueue[0].c) {
		// No tone in queue, nothing to clear
	}
	else {
		// Mark entry as empty
		for(i=0; i<DTMF_QUEUE_LENGTH; i++) {
			DTMFtoneQueue[i].c = 'e';
		}

		DTMFinfo->q15DTMFindexIn = 0;		// Init where in the queue DMTF should first be written
		DTMFinfo->q15lengthTone = 0;		// No samples in queue
		DTMFinfo->q15lengthSilence = 0;		// No samples in queue
		DTMFinfo->q15timeIndex1 = 0;		// Init time index for sin generator tone 1
		DTMFinfo->q15timeIndex2 = 0;		// Init time index for sin generator tone 2
	}
}

/*****************************************************************************
 *	\brief		Remove the character that was last added to the queue
 *	\parameters DTMFtoneQueue - Struct with DTMF tones queued
 *				DTMFinfo 	  - Struct with info about the DTMF tone that is 1st in the queue.
 *	\return		None
 *****************************************************************************/
void dtmf_remove_last_added(DTMFqueue_t *DTMFtoneQueue, DTMFset_t *DTMFinfo){

	if('e' == DTMFtoneQueue[0].c) {
		// No tone in queue, don't update
		DTMFinfo->q15lengthTone = 0;		// No samples in queue
		DTMFinfo->q15lengthSilence = 0;		// No samples in queue
		DTMFinfo->q15timeIndex1 = 0;		// Init time index for sin generator tone 1
		DTMFinfo->q15timeIndex2 = 0;		// Init time index for sin generator tone 2
	}
	else {
		// Set queue position to empty
		DTMFtoneQueue[DTMFinfo->q15DTMFindexIn].c = 'e';
		// Decrease queue input index
		if(0 > --DTMFinfo->q15DTMFindexIn) {
			DTMFinfo->q15DTMFindexIn = 0;
		}
	}
}









#if 0
/*****************************************************************************/
// Volume for playbacked DTMF tones
//#define DTMF_VOLUME 1024
// 0dB twist for DTMF
//#define DTMF_TWIST 32767
/*
 *	\brief	Generates and adds DTMF-tones to the provided sound buffer
 *
 *  \date	2009-09-07
 */
//void generate_dtmf(q15 *x, char cKey) {
//	unsigned int uiTone1, uiTone2;
//	unsigned int fs = 16000;
//	static unsigned int time1=0, time2=0;
//	fract16 p16tone1[DEC_INDEX], p16tone2[DEC_INDEX], f16damp_set, twist;
//	static fract16 f16damp;
//	short i;
//	bool bGenerateTone, bGenerateSilence;
//	const fract16 rise=32500, fall=32500;
//
//	// Only update queue and add DTMF during active call to avoid tones when waiting for dect-line to initiate
//	if(expected_false(!FSet.bActiveCall)) {
//		return;
//	}
//
//	// Update queue
//	DTMFqueue_update();
//
//	// Should we generate a tone
//	if(expected_false(--FSet.sDTMFTimeTone > 0)) {
//		bGenerateTone = true;
//		fenix_set_tone_active(1);
//		// Default setting for every call
//		fenix_set_outgoing_dect_call(0);
//	} else {
//		bGenerateTone = false;
//		FSet.sDTMFTimeTone++; // Prevent overflow
//		fenix_set_tone_active(0);
//	}
//	// Should we generate silence after tone
//	if(expected_false(!bGenerateTone && --FSet.sDTMFTimeSilent > 0)) {
//		bGenerateSilence = true;
//	} else {
//		bGenerateSilence = false;
//		FSet.sDTMFTimeSilent++; // Prevent overflow
//	}
//
//	// Create and apply tone
//	if(expected_false(bGenerateTone)) {
//		// Which row?
//		if((cKey=='1') || (cKey=='2') || (cKey=='3') || (cKey=='A')) {
//			uiTone1 = uiDTMF_row[0];
//		} else if((cKey=='4') || (cKey=='5') || (cKey=='6') || (cKey=='B')) {
//			uiTone1 = uiDTMF_row[1];
//		} else if((cKey=='7') || (cKey=='8') || (cKey=='9') || (cKey=='C')) {
//			uiTone1 = uiDTMF_row[2];
//		} else if((cKey=='*') || (cKey=='0') || (cKey=='#') || (cKey=='D')) {
//			uiTone1 = uiDTMF_row[3];
//		} else {
//			// Error, but input is already checked so this should not be a problem
//		}
//
//		// Which column?
//		if((cKey=='1') || (cKey=='4') || (cKey=='7') || (cKey=='*')) {
//			uiTone2 = uiDTMF_col[0];
//		} else if((cKey=='2') || (cKey=='5') || (cKey=='8') || (cKey=='0')) {
//			uiTone2 = uiDTMF_col[1];
//		} else if((cKey=='3') || (cKey=='6') || (cKey=='9') || (cKey=='#')) {
//			uiTone2 = uiDTMF_col[2];
//		} else if((cKey=='A') || (cKey=='B') || (cKey=='C') || (cKey=='D')) {
//			uiTone2 = uiDTMF_col[3];
//		} else {
//			// Error, but input is already checked so this should not be a problem
//		}
//
//		// Slowly fade in and out tones
//		if(FSet.sDTMFTimeTone<20) {
//			// Adjust damping towards zero at the end
//			f16damp_set = 0;
//		} else {
//			f16damp_set = DTMF_VOLUME;
//		}
//
//
//		// Generate and apply tones
//		for(i=0; i<DEC_INDEX; i++) {
//			// Generate tones
//			p16tone1[i] = fenix_sin2pi_fr16(&time1, fs, uiTone1); // Tone 1
//			p16tone2[i] = fenix_sin2pi_fr16(&time2, fs, uiTone2); // Tone 2
//
//			// Adjust damping to fade tones
//			smoothing(f16damp_set, &f16damp, rise, fall, 0);
//
//			// Add tone to signal
//			twist = mult_fr1x16(f16damp, DTMF_TWIST); // Reduce lower tone by twist
//			x[i] = mult_fr1x16(p16tone1[i], twist); // Mute line signal
//			x[i] = add_fr1x16(x[i], mult_fr1x16(p16tone2[i], f16damp)); // Add to line signal
//		}
//	}
//
//	// Create and apply silence
//	if(expected_false(bGenerateSilence)) {
//		// Mute signal
//		for(i=0; i<DEC_INDEX; i++) {
//			x[i] = 0;
//		}
//		// Reset damping
//		f16damp = 0;
//	}
//}
//



// ----- Dial tone detection -----
// The energy in the tone band must be louder than this to be detected
//#define TONE_DETECT_THRESH 16384
// If the energy in the side bands are more than this, a tone can not be detected
//#define SIDE_BAND_ENERGY_THRESH 2000
// Only detect tone during the first 30 seconds of a call on a DECT line
//#define TONE_DETECTION_TIME (30*FENIX_FREQUENCY)

// ----- DTMF queue -----
// How many DTMF structs should we be able to queue
//#define DTMF_QUEUE_LENGTH 32
//// Construct that contains all information needed for a DTMF sequence
//typedef struct {
//	char c;
//	short time_tone;
//	short time_silent;
//} stDTMFqueue_t;
//// Array with all DMTF constructs
//section("sdram0_data") stDTMFqueue_t stDTMFqueue[DTMF_QUEUE_LENGTH];
//// Index for knowing where in the queue DMTF should be written
//short sDTMFindex_in;
//

/*****************************************************************************/
/*
 *	\brief	Detects if a dial tone (424 Hz) is present on loudspeaker.
 *			Only detect tone if we are going off hook in DECT mode.
 *
 *  \date	2009-08-26
 */
//void detect_dial_tone(complex_fract16 *ls) {
//	fract16 kx=0x2000;//0x240;
//	fract16 ky=0x7fff-kx;
//	fract16 temp16, f16limit;
//	static fract16 f16ToneBandEnergy, f16SideBand1Energy, f16SideBand2Energy;
//	static int iCallCounter;
//	bool bDetectTone;
//	bool bToneDetected;
//
//	// --- Decide if we should detect or not ---
//	bDetectTone = false;
//	if( expected_true(FSet.bActiveCall && FSet.bDect) ) {
//
//		if( expected_false(++iCallCounter < TONE_DETECTION_TIME) ) {
//			// We are at the beginning of a call, search for dial tone
//			bDetectTone = true;
//		} else {
//			// The beginning of the call has passed
//			iCallCounter--; // Avoid overflow
//		}
//	} else {
//		// Reset counter
//		iCallCounter = 0;
//	}
//
//	// Search for tones
//	if( expected_false(bDetectTone) ) {
//
//		// --- Get average signal levels ---
//		// Band where the dial tone is largest
//		temp16 = my_cabs(ls[2]); // Sweden - 425 Hz
//		//temp16 = my_cabs(ls[1]); // Some country where tone is 350 Hz
//		f16ToneBandEnergy = add_fr1x16( mult_fr1x16(kx, temp16), mult_fr1x16(ky, f16ToneBandEnergy) );
//		// Side bands where tone should not exist
//		temp16 = my_cabs(ls[4]);
//		f16SideBand1Energy = add_fr1x16( mult_fr1x16(kx, temp16), mult_fr1x16(ky, f16SideBand1Energy) );
//		temp16 = my_cabs(ls[7]);
//		f16SideBand2Energy = add_fr1x16( mult_fr1x16(kx, temp16), mult_fr1x16(ky, f16SideBand2Energy) );
//
//		// --- Detect dial tone ---
//		// Find maximum energy in sidebands
//		temp16 = max_fr1x16(f16SideBand1Energy, f16SideBand2Energy);
//		// Adapt the detection threshold with the volume damping
//		f16limit = max_fr1x16(mult_fr1x16(TONE_DETECT_THRESH,FState.f16LSVolume), 1000);
//		// Compare energies
//		if( expected_true( (f16ToneBandEnergy > f16limit) &&
//			((f16ToneBandEnergy>>3)>temp16) &&
//			(temp16<SIDE_BAND_ENERGY_THRESH) ) ) {
//			// Activate flag
//			bToneDetected = true;
//		} else {
//			// Deactivate flag
//			bToneDetected = false;
//		}
//
//	} else {
//		// No dial tone active later in the call
//		bToneDetected = false;
//
//		// Reset variables
//		f16ToneBandEnergy = 0;
//		f16SideBand1Energy = 0;
//		f16SideBand2Energy = 0;
//	}
//
//	// Activate flag if tone detected either via energy or at the beginning of a dect call
//	if(bToneDetected || FState.bDialToneSNRFix) {
//		fenix_set_tone_active(1);
//	} else {
//		fenix_set_tone_active(0);
//	}
//
//	// Debug
//
//	//for(i=0;i<DEC_INDEX;i++) {
//	//	pPlot1[tick*DEC_INDEX+i] = f16ToneBandEnergy;
//	//	pPlot2[tick*DEC_INDEX+i] = max_fr1x16(f16SideBand1Energy,f16SideBand2Energy);
//	//	pPlot3[tick*DEC_INDEX+i] = mult_fr1x16(TONE_DETECT_THRESH,FSet.f16LSVolume);
//	//	pPlot4[tick*DEC_INDEX+i] = ls[i].re;
//	//	pPlot5[tick*DEC_INDEX+i] = FSet.bToneActive*2000;
//	//}
//
//}




/*****************************************************************************
 *
 *	\brief	Gives directive to improve SNR during dial tone by not using
 *			shaping filter and instead damp all frequencies with 9 dB.
 *
 *  \date	2009-10-02
 */
//bool dial_tone_snr_fix(void) {
//	static int iCallCounter;
//	bool bCallStart = false;
//	bool bTone = false;
//
//	// Beginning of a outgoing dect call
//	if( expected_true(FSet.bActiveCall && FSet.bDect && FSet.bOutgoingCall) ) {
//
//		if( expected_false(++iCallCounter < TONE_DETECTION_TIME) ) {
//			// Dial tone assumed active
//			bCallStart = true;
//		} else {
//			// The beginning of the call has passed
//			iCallCounter--; // Avoid overflow
//			// Default setting for every call
//			fenix_set_outgoing_dect_call(0);
//		}
//	} else {
//		// Reset counter
//		iCallCounter = 0;
//	}
//
//	// Debug. Return to normal dial tone after some time
//	//if(iCallCounter > 0.1*TONE_DETECTION_TIME) {
//	//	FState.bFirstDTMFSent = true;
//	//}
//
//	if(bCallStart) {
//		// Stop when first DTMF
//		if(FState.bFirstDTMFSent) {
//			bTone = false;
//		} else {
//			bTone = true;
//		}
//	}
//
//	return(bTone);
//}
#endif

