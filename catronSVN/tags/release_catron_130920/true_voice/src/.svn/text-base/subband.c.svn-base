/*****************************************************************************
 *	\file		subband.c
 *
 *	\date		2013-Aug-06
 *
 *	\brief		Implementation of Subband library
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
#include "prot_filt.h"
#include "dsplib.h"
#include <string.h>

// ---< Defines >---

// ---< Global variabels >---


#pragma  DATA_ALIGN(pq15Twiddle, 8)
#pragma  DATA_ALIGN(cmplx_temp, 8)
#pragma  DATA_ALIGN(cmplx_temp2, 8)

// Twiddle factors
static q15 pq15Twiddle[2*NFFT];
// Temporary variables used in analysis and synthesis. Can not be local since it has to be double word aligned
static complex_q31 cmplx_temp[NUM_SUB];
static complex_q31 cmplx_temp2[NUM_SUB];

// ---< Local function prototypes >---

int gen_twiddle_fft16x32(short*, int);

void subband_set_zero(complex_q31 []);

static short d2s(double);

// ---< Functions >---

/*****************************************************************************/
/*
 *	\brief	Initialization for subband splitting functions
 *	\input	None
 *	\output	None
 *	\date	2010-04-27
 *	\author	Markus Borgh
 */
void subband_init(void) {

	// Delay buffers

	// Twiddle factors
	gen_twiddle_fft16x32(pq15Twiddle, NFFT);
}

/*****************************************************************************/
/*
 *	\brief	GDFT-analysis subband filtering
 *	\input	in[]			- 
 * 			out[]			- 
 * 			analys_delay[]	- Signal delay buffer
 * 			*index			- Position in delay buffer
 *	\output	None
 *	\date	2010-06-23
 *	\author	Christian Schüldt
 * 			Markus Borgh
 */
//#pragma CODE_SECTION(subband_analysis, CODE_POSITION)
void subband_analysis(q15 in[], complex_q31 out[], q15 analys_delay[], int *index) {
	q15 q15temp[NUM_PROT_COEFS];
	int i, j, k;

	// Tell compiler that delay vector is doubleword aligned
	//_nassert(((int)analys_delay & 0x7) == 0);

	// Put new samples in delay buffer
	for(i=0;i<BUFLEN_8KHZ;i++) {
		analys_delay[*index] = in[i];
		*index = circindex_pow2(*index, 1, NUM_PROT_COEFS);
	}
	k = *index;
	k = circindex(k, -1, NUM_PROT_COEFS);
	// Prototype filtering
	for(i=0;i<NUM_PROT_COEFS;i++) {
		q15temp[i] = (q15)(add_q31(analys_delay[k]*pq15Prot_coefs[i], 1L<<14)>>15); // Add "0.5" for rounding issues
		k = circindex_pow2(k, -1, NUM_PROT_COEFS);
	}

	for(i=0;i<NUM_SUB;i++) {
		cmplx_temp[i].re = 0;
		cmplx_temp[i].im = 0;
		for(j=i;j<NUM_PROT_COEFS;j+=NUM_SUB)
			cmplx_temp[i].re += (q31)q15temp[j];//<<(16-6); // Upshift 16 (use upper half of the 32-bit word, and downshift 6 = div by 64 (NFFT))
	}

	// Mixed Radix Inverse FFT with Bit Reversal
	DSP_ifft16x32(&pq15Twiddle[0], NFFT, (q31*)cmplx_temp, (q31*)cmplx_temp2);

	// Put in output buffer
	for(i=0;i<DEC_INDEX+1;i++) {
		out[i].re = shl_q31(cmplx_temp2[i].re, SUB_SIGNAL_SHIFT);
		out[i].im = shl_q31(cmplx_temp2[i].im, SUB_SIGNAL_SHIFT);
	}
}

/*****************************************************************************/
/*
 *	\brief	GDFT-synthesis subband filtering
 *	\input	in[]			- 
 * 			out[]			- 
 * 			synt_delay[]	- Signal delay buffer
 * 			*index			- Position in delay buffer
 * 			sAmpShift		- Number of left shifts for signal amplification
 *	\output	None
 *	\date	2010-06-23
 *	\author	Christian Schüldt
 * 			Markus Borgh
 */
//#pragma CODE_SECTION(subband_synthesis, CODE_POSITION)
void subband_synthesis(complex_q31 in[], q31 out[], q31 synt_delay[], int *index, short sAmpShift) {
	int i, k;

	// Tell compiler that delay vector is doubleword aligned
	_nassert(((int)synt_delay & 0x7) == 0);

	// Put input in temporary buffer
	for(i=0;i<DEC_INDEX+1;i++) {
		// The amplification of a signal is performed in subband to reduce quantization noise
		cmplx_temp2[i].re = shr_q31(in[i].re, SUB_SIGNAL_SHIFT-sAmpShift);
		cmplx_temp2[i].im = shr_q31(in[i].im, SUB_SIGNAL_SHIFT-sAmpShift);
	}
	// Fix remaining subbands (negative frequencies) => real output
	for(;i<NUM_SUB;i++) {
		cmplx_temp2[i].re = cmplx_temp2[NUM_SUB-i].re;
		cmplx_temp2[i].im = -cmplx_temp2[NUM_SUB-i].im;
	}

	// (Complex) FFT
	// Mixed Radix Forward FFT with Bit Reversal
	DSP_fft16x32(&pq15Twiddle[0], NFFT, (q31*)cmplx_temp2, (q31*)cmplx_temp);

	k = 0;
	// Prototype filtering
	for(i=0;i<NUM_PROT_COEFS;i++) {
		synt_delay[*index] = synt_delay[*index] + mult_q31x15((add_q31(cmplx_temp[k].re, 1L<<5)>>6), pq15Prot_coefs[i]); // Add "0.5" for rounding issues
		// Indices
		*index = circindex_pow2(*index, 1, NUM_PROT_COEFS);
		k = circindex_pow2(k, 1, NUM_SUB);
	}
	k = *index;
	// Output (and clear)
	for(i=0;i<BUFLEN_8KHZ;i++) {
		k = circindex_pow2(k, -1, NUM_PROT_COEFS);
		out[i] = synt_delay[k];
		synt_delay[k] = 0;
	}

	*index = k;
}


/* ======================================================================== */
/*      gen_twiddle_fft16x32.c -- File with twiddle factor generators.      */
/* ======================================================================== */
/*      This code requires a special sequence of twiddle factors stored     */
/*      in 1Q15 fixed-point format.  The following C code is used for       */
/*      the natural C and intrinsic C implementations.                      */
/*                                                                          */
/*      In order to vectorize the FFT, it is desirable to access twiddle    */
/*      factor array using double word wide loads and fetch the twiddle     */
/*      factors needed. In order to do this a modified twiddle factor       */
/*      array is created, in which the factors WN/4, WN/2, W3N/4 are        */
/*      arranged to be contiguous. This eliminates the seperation between   */
/*      twiddle factors within a butterfly. However this implies that as    */
/*      the loop is traversed from one stage to another, that we maintain   */
/*      a redundant version of the twiddle factor array. Hence the size     */
/*      of the twiddle factor array increases as compared to the normal     */
/*      Cooley Tukey FFT.  The modified twiddle factor array is of size     */
/*      "2 * N" where the conventional Cooley Tukey FFT is of size"3N/4"    */
/*      where N is the number of complex points to be transformed. The      */
/*      routine that generates the modified twiddle factor array was        */
/*      presented earlier. With the above transformation of the FFT,        */
/*      both the input data and the twiddle factor array can be accessed    */
/*      using double-word wide loads to enable packed data processing.      */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#include <math.h>
//#include "gen_twiddle_fft16x32.h"

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif


/* ======================================================================== */
/*  D2S -- Truncate a 'double' to a 'short', with clamping.                 */
/* ======================================================================== */
static short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}


/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_fft16x32(short *w, int n)                       */
/*                                                                          */
/*          short *w      Pointer to twiddle-factor array                   */
/*          int   n       Size of FFT                                       */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */
int gen_twiddle_fft16x32(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j) {
            w[k +  5] = d2s(M * cos(6.0 * PI * i / n));
            w[k +  4] = d2s(M * sin(6.0 * PI * i / n));

            w[k +  3] = d2s(M * cos(4.0 * PI * i / n));
            w[k +  2] = d2s(M * sin(4.0 * PI * i / n));

            w[k +  1] = d2s(M * cos(2.0 * PI * i / n));
            w[k +  0] = d2s(M * sin(2.0 * PI * i / n));

            k += 6;
        }
    }
    return k;
}

/* ======================================================================= */
/*  End of file:  gen_twiddle_fft16x32.c                                   */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

