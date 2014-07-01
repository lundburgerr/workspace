/*****************************************************************************
 *	\file		directive_audio.c
 *
 *	\date		2013-Jun-03
 *
 *	\brief		Creation of stereo channels from five input directions
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
//#include "directive_audio_filter.h"
#include "front_coefs_left.h"
#include "front_coefs_right.h"
#include "leftfront_coefs_left.h"
#include "leftfront_coefs_right.h"
#include "leftback_coefs_left.h"
#include "leftback_coefs_right.h"
#include "rightback_coefs_left.h"
#include "rightback_coefs_right.h"
#include "rightfront_coefs_left.h"
#include "rightfront_coefs_right.h"

#if USE_DEBUG_PLOT
#include "../../true_voice/include/plot.h" // Should not be included here but used for testing of filter
#endif

// ---< Defines >---
#define FILTER_LENGTH COEFS_LENGTH
#define RXTX_LIM_DELAY_LENGTH	(1*BUFLEN_8KHZ) // Length of delay buffer in limiter

// ---< Global variabels >---
// Audio buffers needed for directive audio
q15 pq15lsTechLeftFront[BUFLEN_8KHZ];
q15 pq15lsTechLeftBack[BUFLEN_8KHZ];
q15 pq15lsTechRightFront[BUFLEN_8KHZ];
q15 pq15lsTechRightBack[BUFLEN_8KHZ];
q15 pq15lsTechCenter[BUFLEN_8KHZ];

// For fir filters
//filterSet_t filterSetLeftFront_left;
//filterSet_t filterSetLeftFront_right;
//filterSet_t filterSetLeftBack_left;
//filterSet_t filterSetLeftBack_right;
//filterSet_t filterSetRightFront_left;
//filterSet_t filterSetRightFront_right;
//filterSet_t filterSetRightBack_left;
//filterSet_t filterSetRightBack_right;
//filterSet_t filterSetCenter_left;
//filterSet_t filterSetCenter_right;
q15 filterSetLeftFront_left[COEFS_LENGTH-1];
q15 filterSetLeftFront_right[COEFS_LENGTH-1];
q15 filterSetLeftBack_left[COEFS_LENGTH-1];
q15 filterSetLeftBack_right[COEFS_LENGTH-1];
q15 filterSetRightFront_left[COEFS_LENGTH-1];
q15 filterSetRightFront_right[COEFS_LENGTH-1];
q15 filterSetRightBack_left[COEFS_LENGTH-1];
q15 filterSetRightBack_right[COEFS_LENGTH-1];
q15 filterSetCenter_left[COEFS_LENGTH-1];
q15 filterSetCenter_right[COEFS_LENGTH-1];

#pragma  DATA_ALIGN(pq15lsTechLeftFront_left, 4)
#pragma  DATA_ALIGN(pq15lsTechLeftFront_right, 4)
#pragma  DATA_ALIGN(pq15lsTechLeftBack_left, 4)
#pragma  DATA_ALIGN(pq15lsTechLeftBack_right, 4)
#pragma  DATA_ALIGN(pq15lsTechRightFront_left, 4)
#pragma  DATA_ALIGN(pq15lsTechRightFront_right, 4)
#pragma  DATA_ALIGN(pq15lsTechRightBack_left, 4)
#pragma  DATA_ALIGN(pq15lsTechRightBack_right, 4)
#pragma  DATA_ALIGN(pq15lsTechCenter_left, 4)
#pragma  DATA_ALIGN(pq15lsTechCenter_right, 4)
q15 pq15lsTechLeftFront_left[BUFLEN_8KHZ];
q15 pq15lsTechLeftFront_right[BUFLEN_8KHZ];
q15 pq15lsTechLeftBack_left[BUFLEN_8KHZ];
q15 pq15lsTechLeftBack_right[BUFLEN_8KHZ];
q15 pq15lsTechRightFront_left[BUFLEN_8KHZ];
q15 pq15lsTechRightFront_right[BUFLEN_8KHZ];
q15 pq15lsTechRightBack_left[BUFLEN_8KHZ];
q15 pq15lsTechRightBack_right[BUFLEN_8KHZ];
q15 pq15lsTechCenter_left[BUFLEN_8KHZ];
q15 pq15lsTechCenter_right[BUFLEN_8KHZ];

// For limiter
static q15 pq15TX_limit_delay_directive_audio[2][RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust

limitSet16_t limitSetDirecAudio[2];

// ---< Local function prototypes >---
static void FIR_DSP(const q15 *input, q15 *output, q15 *h, q15 *oldValue);
static void DSP_fir_gen (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
);


// ---< Functions >---
/*****************************************************************************
 *	\brief		Initialization of directive audio
 *	\parameters	None 
 *	\return		None
 *****************************************************************************/
void directive_audio_init(void) {
	int i;
	
	// Limiter settings
	for (i=0; i<2; i++){
		memset(pq15TX_limit_delay_directive_audio[i], 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);
		
		limitSetDirecAudio[i].iCounter			= 0;
		limitSetDirecAudio[i].iDelayIndex		= 0;
		limitSetDirecAudio[i].q15gain_set		= MAX_16;
		limitSetDirecAudio[i].q15gain_set2		= MAX_16;
		limitSetDirecAudio[i].q15gain			= MAX_16;
		limitSetDirecAudio[i].q15LIMIT_CLIP_LVL = 6226; //0.95*MAX_16/5
		limitSetDirecAudio[i].pq15Delay			= pq15TX_limit_delay_directive_audio[i];
		limitSetDirecAudio[i].q15deltaRise		= 16;	//0x0010; //0.001
		limitSetDirecAudio[i].q15gammaFall		= 31000; //-14.5 dB in 32 samples	//0x7a00; //0.95
	}
	// Filter settings
//	for (i=0; i<2*FILTER_LENGTH; i++) {
//
//		filterSetLeftFront_left.x_vec[i] = 0;
//		filterSetLeftFront_right.x_vec[i] = 0;
//		filterSetLeftBack_left.x_vec[i] = 0;
//		filterSetLeftBack_right.x_vec[i] = 0;
//		filterSetRightFront_left.x_vec[i] = 0;
//		filterSetRightFront_right.x_vec[i] = 0;
//		filterSetRightBack_left.x_vec[i] = 0;
//		filterSetRightBack_right.x_vec[i] = 0;
//		filterSetCenter_left.x_vec[i] = 0;
//		filterSetCenter_right.x_vec[i] = 0;
//	}
//
//	filterSetLeftFront_left.index = 0;
//	filterSetLeftFront_right.index = 0;
//	filterSetLeftBack_left.index = 0;
//	filterSetLeftBack_right.index = 0;
//	filterSetRightFront_left.index = 0;
//	filterSetRightFront_right.index = 0;
//	filterSetRightBack_left.index = 0;
//	filterSetRightBack_right.index = 0;
//	filterSetCenter_left.index = 0;
//	filterSetCenter_right.index = 0;
	memset(filterSetLeftFront_left, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetLeftFront_right, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetLeftBack_left, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetLeftBack_right, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetRightFront_left, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetRightFront_right, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetRightBack_left, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetRightBack_right, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetCenter_left, 0, (COEFS_LENGTH-1)*sizeof(q15));
	memset(filterSetCenter_right, 0, (COEFS_LENGTH-1)*sizeof(q15));

}


/*****************************************************************************
 *	\brief		Creates stereo output from the five input channel directions
 * 				left front, left back, right front, right back and center.
 *	\parameters	left[]	- Left channel output buffer
 * 				right[]	- Right channel output buffer
 *	\return		None
 *****************************************************************************/
void directive_audio(q15 pq15left[], q15 pq15right[]) {
	int k;
	q15 q15temp1, q15temp2, q15temp3, q15temp4, q15temp5;
	const q15 q15norm=MAX_16/5; // Normalization to avoid clipping when no directive audio is used
	const q15 q15norm_DA=MAX_16/5; // Normalization to avoid clipping when directive audio is used
	// Buffers to use for left/right filters
//	q15 pq15tempplot[BUFLEN_8KHZ];
//	q15 pq15tempplot2[BUFLEN_8KHZ];
//  q15 pq15tempplot3[BUFLEN_8KHZ];

	if(!tvModules.bDirectiveAudio) {

		// Loop through with no directive audio
		for(k=0; k<BUFLEN_8KHZ; k++) {
			// Add signals to left channel
			q15temp1 = mult_q15(pq15lsTechLeftFront[k], q15norm);
			q15temp2 = mult_q15(pq15lsTechLeftBack[k], q15norm);
			q15temp3 = mult_q15(pq15lsTechRightFront[k], q15norm);
			q15temp4 = mult_q15(pq15lsTechRightBack[k], q15norm);
			q15temp5 = mult_q15(pq15lsTechCenter[k], q15norm);
			pq15left[k] = add_q15(add_q15(add_q15(add_q15(q15temp1, q15temp2), q15temp3),q15temp4),q15temp5);
			// Add signals to right channel
			pq15right[k] = pq15left[k];
		}

		if(tvModules.bLsLimiter){
			limit_and_delay_16(pq15left, pq15left, &limitSetDirecAudio[0]);
			limit_and_delay_16(pq15right, pq15right, &limitSetDirecAudio[1]);

			for(k=0; k<BUFLEN_8KHZ; k++) {
				// Amplify signals with 1/q15norm_DA
				pq15right[k] = pq15right[k]*5;
				pq15left[k] = pq15left[k]*5;
			}
		}

		return;
	}

	// Directive Audio on CA and PT
	if(tvSettings.platform != TV_CONFIG_BS){

		// Filter signals for left ls
		FIR_DSP(pq15lsTechLeftFront, pq15lsTechLeftFront_left, pq15LeftFront_coefs_left, filterSetLeftFront_left);
		FIR_DSP(pq15lsTechLeftBack,  pq15lsTechLeftBack_left,  pq15LeftBack_coefs_left,  filterSetLeftBack_left);
		FIR_DSP(pq15lsTechRightFront,pq15lsTechRightFront_left,pq15RightFront_coefs_left,filterSetRightFront_left);
		FIR_DSP(pq15lsTechRightBack, pq15lsTechRightBack_left, pq15RightBack_coefs_left, filterSetRightBack_left);
		FIR_DSP(pq15lsTechCenter,    pq15lsTechCenter_left,    pq15Front_coefs_left,     filterSetCenter_left);

		// Filter signals for right ls
		FIR_DSP(pq15lsTechLeftFront, pq15lsTechLeftFront_right, pq15LeftFront_coefs_right, filterSetLeftFront_right);
		FIR_DSP(pq15lsTechLeftBack,  pq15lsTechLeftBack_right,  pq15LeftBack_coefs_right,  filterSetLeftBack_right);
		FIR_DSP(pq15lsTechRightFront,pq15lsTechRightFront_right,pq15RightFront_coefs_right,filterSetRightFront_right);
		FIR_DSP(pq15lsTechRightBack, pq15lsTechRightBack_right, pq15RightBack_coefs_right, filterSetRightBack_right);
		FIR_DSP(pq15lsTechCenter,    pq15lsTechCenter_right,    pq15Front_coefs_right,     filterSetCenter_right);

//		for(k=0; k<BUFLEN_8KHZ; k++) {
//			pq15tempplot[k] = pq15lsTechLeftBack[k];
//			pq15tempplot2[k] = pq15lsTechLeftBack_left[k];
//			pq15tempplot3[k] = pq15lsTechLeftBack_right[k];
//		}
	
		for(k=0; k<BUFLEN_8KHZ; k++) {
			// Add signals to left channel
			q15temp1 = mult_q15(pq15lsTechLeftFront_left[k], q15norm_DA);
			q15temp2 = mult_q15(pq15lsTechLeftBack_left[k], q15norm_DA);
			q15temp3 = mult_q15(pq15lsTechRightFront_left[k], q15norm_DA);
			q15temp4 = mult_q15(pq15lsTechRightBack_left[k], q15norm_DA);
			q15temp5 = mult_q15(pq15lsTechCenter_left[k], q15norm_DA);
			pq15left[k] = add_q15(add_q15(add_q15(add_q15(q15temp1, q15temp2), q15temp3),q15temp4),q15temp5);

			// Add signals to right channel
			q15temp1 = mult_q15(pq15lsTechLeftFront_right[k], q15norm_DA);
			q15temp2 = mult_q15(pq15lsTechLeftBack_right[k], q15norm_DA);
			q15temp3 = mult_q15(pq15lsTechRightFront_right[k], q15norm_DA);
			q15temp4 = mult_q15(pq15lsTechRightBack_right[k], q15norm_DA);
			q15temp5 = mult_q15(pq15lsTechCenter_right[k], q15norm_DA);
			pq15right[k] = add_q15(add_q15(add_q15(add_q15(q15temp1, q15temp2), q15temp3),q15temp4),q15temp5);
		}

		if(tvModules.bLsLimiter){
			limit_and_delay_16(pq15left, pq15left, &limitSetDirecAudio[0]);
			limit_and_delay_16(pq15right, pq15right, &limitSetDirecAudio[1]);
			for(k=0; k<BUFLEN_8KHZ; k++) {
				// Amplify signals with 1/q15norm_DA
				pq15right[k] = pq15right[k]*5;
				pq15left[k] = pq15left[k]*5;
			}
		}
		//write_plot_data(pq15tempplot,  &plotVar1, BUFLEN_8KHZ);
		//write_plot_data(pq15tempplot2,  &plotVar2, BUFLEN_8KHZ);
		//write_plot_data(pq15tempplot3,  &plotVar3, BUFLEN_8KHZ);
	}
	
	return;
	
}


/*****************************************************************************
 *	\brief		Filters a signal with the coefficients in vector h.
 *	\parameters	input 		- Input signal
 * 				output 		- buffer for filtered signal
 * 				h 			- filter coefficients in reversed orders
 *				oldValue 	- Buffer of previous values from previous iterations
 *							needed to calculate new output
 *	\return		None
 *****************************************************************************/
//TODO: oldValue can not be longer than BUFLEN as of now.
static void FIR_DSP(const q15 *input, q15 *output, q15 *h, q15 *oldValue){
	int ncopy, copyIdx;
	q15 x[BUFLEN_8KHZ+COEFS_LENGTH-1];
	ncopy = COEFS_LENGTH-1;
	copyIdx = BUFLEN_8KHZ-ncopy;

	memcpy(x, oldValue, (COEFS_LENGTH-1)*sizeof(q15));
	memcpy(&x[COEFS_LENGTH-1], input, BUFLEN_8KHZ*sizeof(q15));
	memcpy(oldValue, &input[copyIdx], ncopy*sizeof(q15));
	DSP_fir_gen(x, h, output, COEFS_LENGTH, BUFLEN_8KHZ);
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* DSP_fir_gen.c -- FIR Filter (General)                                   */
/*                  Optimized C Implementation (w/ Intrinsics)             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_gen (                                                  */
/*         const short *restrict x,                                        */
/*         const short *restrict h,                                        */
/*         short *restrict r,                                              */
/*         int nh,                                                         */
/*         int nr,                                                         */
/*     )                                                                   */
/*                                                                         */
/*  Description                                                            */
/*     Computes a real FIR filter (direct-form) using coefficients         */
/*     stored in vector h.  The real data input is stored in vector x.     */
/*     The filter output result is stored in vector r.  Input data and     */
/*     filter taps are 16-bit, with intermediate values kept at 32-bit     */
/*     precision.  Filter taps are expected in Q15 format.                 */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x, h, and r do not overlap                                   */
/*     nh >= 5;                                                            */
/*     nr >= 4; nr % 4 == 0                                                */
/*                                                                         */
/* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/  */
/***************************************************************************/
static void DSP_fir_gen (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
)
{
/* *********************** */
/* Code for the C64x+ Core */
/* *********************** */
//#ifdef _TMS320C6400_PLUS
    int i, j, mask;
    int h_32, h_10;
    int sum0, sum1, sum2, sum3;
    int sum_32, sum_10;

    long long x_3210, x_4321, x_7654;
    long long h_3210, h_3210_mod;
    long long r3, r2, r1, r0;

    /*---------------------------------------------------------------------*/
    /* Pad the end of the filter tap array with zeros in order make the    */
    /* array length a multiple of 4. This allows the loop to be optimized. */
    /*---------------------------------------------------------------------*/
    mask = nh & 3;
    if (!mask) mask = 4;

    h_3210 = _mem8_const(&h[nh - mask]);
    h_10 = _loll(h_3210);
    h_32 = _hill(h_3210);

    if (mask == 4) {    // Already a multiple of 4 (do nothing)
//      h_32 &= 0xFFFFFFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 3) {    // Mask out the last 16 bits (1 short)
        h_32 &= 0x0000FFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 2) {    // Mask out the last 32 bits (2 shorts)
        h_32 &= 0x00000000;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 1) {    // Mask out the last 48 bits (3 shorts)
        h_32 &= 0x00000000;
        h_10 &= 0x0000FFFF;
    }

    /*---------------------------------------------------------------------*/
    /* Modified taps to be used during the the filter tap loop             */
    /*---------------------------------------------------------------------*/
    h_3210_mod = _itoll(h_32, h_10);

    _nassert((int)x % 8 == 0);
    _nassert(nr % 4 == 0);
    _nassert(nr >= 4);
    for (j = 0; j < nr; j += 4) {
        sum0 = 0;
        sum1 = 0;
        sum2 = 0;
        sum3 = 0;

        _nassert((int)x % 8 == 0);
        #pragma MUST_ITERATE(1,,1)
        for (i = 0; i < nh; i += 4) {
            h_3210 = _mem8_const(&h[i]);
            x_3210 = _mem8_const(&x[i + j]);
            x_4321 = _mem8_const(&x[i + j + 1]);
            x_7654 = _mem8_const(&x[i + j + 4]);

            /*-------------------------------------------------------------*/
            /* Use modified taps during the last iteration of the loop.    */
            /*-------------------------------------------------------------*/
            if (i >= nh - 4)
                h_3210 = h_3210_mod;

            h_32 = _hill(h_3210);
            h_10 = _loll(h_3210);

            r3 = _ddotpl2(x_7654, h_32);    // x6h3+x5h2, x5h3+x4h2
            r2 = _ddotph2(x_4321, h_32);    // x4h3+x3h2, x3h3+x2h2
            r1 = _ddotph2(x_4321, h_10);    // x4h1+x3h0, x3h1+x2h0
            r0 = _ddotpl2(x_3210, h_10);    // x2h1+x1h0, x1h1+x0h0

            sum3 += _hill(r3) + _hill(r1);
            sum2 += _loll(r3) + _loll(r1);
            sum1 += _hill(r2) + _hill(r0);
            sum0 += _loll(r2) + _loll(r0);
        }

        sum_10 = _packh2(sum1 << 1, sum0 << 1);
        sum_32 = _packh2(sum3 << 1, sum2 << 1);

        _mem8(&r[j]) = _itoll(sum_32, sum_10);
    }
}





// ---< Unused code >----
#if 0
static void FIR_filter(q15 *, q15 *, q15 *, q15 Lx, filterSet_t *filterSet);

/*****************************************************************************
 *	\brief		Filters a signal with the coefficients in vector h.
 * 				Assupmtion: Lx<Lh.
 * 				(TODO: produce optimal code for filtering. In debug directive audio
 * 				takes about 3ms to run because of the filtering. It takes 0.2 ms if
 * 				no directive audio is used and the signals is only set to left
 * 				and right buffer)
 *	\parameters	x - Input signal
 * 				y - buffer for filtered signal
 * 				h - filter coefficients in vector
 *				Lx - length of input/output vector
 * 				filterSet - struct to keep track of index for different filters
 *	\return		None
 *****************************************************************************/
static void FIR_filter(q15 x[], q15 y[], q15 h[], q15 Lx, filterSet_t *filterSet) {
	int i, j;
	q31 temp;

	for (i=0; i<Lx; i++) {
		y[i]=0;
		temp=0;
		filterSet->x_vec[filterSet->index]=x[i];
		filterSet->x_vec[filterSet->index+FILTER_LENGTH]=x[i];
		for (j=0; j<FILTER_LENGTH; j++) {
			temp+=mult_q15(filterSet->x_vec[filterSet->index+j],h[j]);
		}
		y[i]=temp;
		filterSet->index--;
		if (filterSet->index<0) {
			filterSet->index=FILTER_LENGTH-1;
		}
	}

}


#endif
