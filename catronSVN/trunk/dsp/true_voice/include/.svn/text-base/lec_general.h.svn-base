/*****************************************************************************
 *	\file		lec_general.h
 *
 *	\date		2013-Sep-23
 *
 *	\brief		Header for LEC library
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef LEC_GENERAL_H
#define LEC_GENERAL_H

// ---< Include >---

#include "true_voice_internal.h"

// ---< Defines >---
#define USE_LEC 	1
#define USE_LEC_2	1
#define USE_LEC_3	1

// Number of samples in each accumulated signal block
#define NUM_ACK	8
// Initial value for noise estimation
#define NOI_EST_INIT_VALUE (10000<<SUB_SIGNAL_SHIFT)

#define NOI_DAMP_SHIFT_LS 0 // Corresponds to 0 dB noise suppression
#define NOI_DAMP_SHIFT_TX 0 // Corresponds to 0 dB noise suppression

#define TX_DELAY 4

 #define NUM_PROT_COEFS		256 // Must be a power of 2 !!!
 #define DEC_INDEX 			32
 // Number of subbands
 #define NUM_SUB			64
 // Shift for increased subband precision

//#define NUM_OLD_AEC			5					// (NUM_OLD_AEC-1) must be a power of 2 !!!
// Number of coefficients in AEC filter
#define NLMS_FILTER_LENGTH	32	//64 gives MIPS problems ... // Must be a power of 2 !!!
#define NLMS_FILTER_LENGTH_SHIFT 5  //2^5=32 //TODO: mixing numbers from different codes, NOT! good.
// Number of samples delay to reduce the number of initial zeros in the impulse response
#define AEC_LS_DELAY		1
// Number of coefficients in filterbank prototype filter

#define NUM_ACTIVE_SUB (DEC_INDEX+1) // Number of "active" subbands

#define AEC_COEFS_SHIFT_NEG 1
extern const unsigned int puiAEC_coefs_shift[NUM_ACTIVE_SUB];

// Number of 64 sample buffers (how many repeated calls to rx-ls and mic-tx processing)
//#define NUM_AUDIO_BUF 5

#define SUB_SIGNAL_SHIFT	12

// Right shift when accumulating output error
#define ACC_ERROR_E_RSHIFT	4
// Time to hold LS_AVE - should be increased !!!
#define LS_AVE_HOLD_TIME 26

// ---< Composites declaration >---

// Struct for noise estimation settings
typedef struct {
	q31 gamma;				// Smoothing parameter
	q31 gamma_inv;			// Correction with last accumulated signal block in average
	q31 beta;				// Rise parameter
	unsigned int j;			// Sample counter
	unsigned int k;			// Block counter
	bool bNoiEstAct;		// Full signal block accumulated
	bool bInstantFall;		// Drop estimation to average level
	bool bEstimateNoise;	// Signal is stationary -> estimate noise
	q31	pq31block[NUM_ACTIVE_SUB]; // Accumulated samples ("block")
	q31 pq31max1[NUM_ACTIVE_SUB]; // Running max 1 in each subband
	q31 pq31max2[NUM_ACTIVE_SUB]; // Running max 2 in each subband
	q31 pq31min1[NUM_ACTIVE_SUB]; // Running min 1 in each subband
	q31 pq31min2[NUM_ACTIVE_SUB]; // Running min 2 in each subband
	q31 *noiEst;
} noiEstSet_t;

// Types of averages (of input signals, internal signals or output signals)
enum AVE_TYPES { AVE_RX_IN, AVE_MIC_IN, AVE_LS_OUT, AVE_INTER, AVE_TX_OUT };

// ---< Function prototypes >---

void noi_est_init(noiEstSet_t *nSet,q31 *nEst);
complex_q31 cvecdot_conj(const complex_q15 h_vec[], const complex_q31 x_vec[],
		const int index, const int length_delay, const int length_filt);
void find_minmaxgamma(complex_q15 coefs[][NLMS_FILTER_LENGTH], q15 *pmax, unsigned int *pmax_idx, q15 *plast, q31 *pgamma, const unsigned short subband);
void cvecvaddsmult(complex_q15 *__restrict h_vec, const complex_q31 *__restrict x_vec, const complex_q31 c_step, const int index);
void noise_reduce(q15 *__restrict gains, q31 *__restrict signal_ave, q31 *__restrict noi_est, bool bEnabled, bool bLS);
void resdamp_tx_gains_adjust(q15 gain_set[], q31 aerr_fg[]);
void resdamp_tx_gains_smooth(bool bIsBtalk, q15 *gain_set, q15 *gain);
void resdamp_apply_gains(q15 *gains, complex_q31 *in, complex_q31 *out);

// Estimates Echo Return Loss Enhancement. Minimum level is -30 dB.
__inline void erle_estimation(q15 *pq15erle, q31 e_fast, q31 e_slow, q31 y_fast, q31 y_slow, bool bincrease) {
	q15 q15temp, q15temp2;
	const q15 gamma = 400; //1.2% 12dB during a 1/4s  of constant doubletalk
	// ERLE is estimated by taking minimum of two different average ratios. This has the effect that ERLE_bg is not set too high after a filter copy.
	// This could possibly lead to problems during an echo path change since ERLE_bg could be set lower than the true ERLE

	q15temp = q31_to_q15(div_q31x31(add_q31(e_fast,1), add_q31(y_fast,1)));
	q15temp2 = q31_to_q15(div_q31x31(add_q31(e_slow,1), add_q31(y_slow,1)));
	q15temp = MIN(q15temp, q15temp2); // Take minimum
	// Improved performance?
	if(*pq15erle > q15temp) {
		q15temp = MAX(q15temp, MAX_16>>6); // No more than -36 dB allowed
		*pq15erle = q15temp; // Update ERLE
	} else {
		// Let ERLE increase?
		if(bincrease)
			*pq15erle = add_q15(*pq15erle, MAX(mult_q15(*pq15erle, gamma), 50));
	}
}

// Check for stationarity
inline void check_stationary(const q31 *__restrict pmax1, const q31 *__restrict pmax2, const q31 *__restrict pmin1, const q31 *__restrict pmin2, bool *__restrict b) {
	const q15 NOISE_THRESH = 4096; // Allowed ratio between max and min in accumulated blocks for estimating noise
	q31 temp1, temp2;
	unsigned int i;

	// Signal sufficiently stationary in all subbands? (except the two lowest)
	for(i=2;i<NUM_ACTIVE_SUB;i++) {
		temp1 = MIN(pmin1[i], pmin2[i]);
		temp2 = MAX(pmax1[i], pmax2[i]);
		// Block differences, if difference is larger than x NOISE_THRESH -> don't estimate noise
		if(mult_q31x15(temp2, NOISE_THRESH) > temp1) {
			*b = false;
			break;
		}
	}
}

// Count number of 1-bits in a 16-bit word
static inline int countbits(unsigned int i) {
	//return(__builtin_popcount(a));
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
};

static inline q15 lec_smoothing2_q15(const q15 in, q15 ave, const q15 ky_rise, const q15 ky_fall, const unsigned int s) {
	q15 ky;

	// Rise or fall?
	ky = ky_fall;
	if(in > shr_q15(ave, s))
		ky = ky_rise;

	return(lec_smoothing_q15(in, ave, ky, s));
}

// Smoothing for e.g. calculating averages
static inline q31 lec_smoothing(const q31 in, q31 ave, const q31 ky) {
	q31 out;

	out = mult_q31x31(in, MAX_32 - ky);
	out = add_q31(out, mult_q31x31(ave, ky));
	return(out);
}


// Smoothing with shift for e.g. calculating averages
static inline q31 lec_smoothing_shift(const q31 in, q31 ave, const q31 ky, const unsigned int s) {
	q31 out;

	out = mult_q31x31_shift(in, MAX_32 - ky, s);
	out = add_q31(out, mult_q31x31(ave, ky));
	return(out);
}

static inline q31 lec_smoothing2(const q31 in, q31 ave, const q31 ky_rise, const q31 ky_fall) {
	q31 ky;

	// Rise or fall?
	ky = ky_fall;
	if(in > ave)
		ky = ky_rise;

	return(lec_smoothing(in, ave, ky));
}

/*
// Multiplication with increased precision for AEC energy measure
inline q31 mult_q19x19_shift(const q31 a, const q31 b) {

	return(shift64_q31(multll_q31x31(a, b) + (1<<(18+SUB_SIGNAL_SHIFT)), 19+SUB_SIGNAL_SHIFT));
}

// Multiplication with increased precision for LP energy measure
inline q31 mult_q19x19(const q31 a, const q31 b) {

	return(shift64_q31(multll_q31x31(a, b) + (1<<18), 19));
}
// Multiplication with increased precision for AEC energy measure
inline q31 square_shift14(const q31 a) {

	return(shift64_q31(multll_q31x31(a, a), 14));
}
*/
//from infocusSvn Rev27
static inline q31 square_shift14(const q31 a) {
	long long b;

	b = (long long)a*a;
	return((q31)(b >> 35LL)); //should be 36??
}


static inline q31 mult_q19x19_shift(const q31 a, const q31 b) {
	long long c = 1LL<<(29);
	c += (long long)a*b;
	return((q31)(c >> (30)));
}

static inline q31 div_q20x31(const q31 a, q31 b) {
	return((q31)(((long long)a << 9LL) / b));
}

/*****************************************************************************/
/*
 *	\brief	Fast version of
 *
 * 	c = ((long long)a<<20) / b;
 *
 *	\input	a
 * 			b
 *	\output	c
 *	\date	2010-09-27

 *	\author	Christian Schüldt
 */
/*
inline q31 div_q20x31(q31 a, q31 b) {
	q31 bnorm, out;
	unsigned int x_new;
	unsigned int acc1, acc2;
	unsigned short sbits;

	// Check for negative arguments
	if(b < 0) {
		// -a/-b = a/b
		// a/-b = -a/b
		a = -a;
		b = -b;
	}

	// Check for overflow
	if(abs_q31(a>>11) >= abs_q31(b)) {

		// a determines the sign of the result
		if(a < 0) return(MIN_32);
		else return(MAX_32);
	}

	// Normalize (left shift until the two most significant bits become unequal)
	sbits = (unsigned short)_norm(b);
	bnorm = (int)(b << sbits);

	acc1 = (unsigned int)bnorm + (unsigned int)bnorm;
	// Using initial guess x0 = 2.9142 - 2*b
	x_new = 0x17504816C - acc1;

	// 4 Newton-Raphson iterations

	// acc1 = (x_new*bnorm)>>31
	acc1 = _mpyhu(x_new, bnorm)<<1;
	acc1 += _mpyhlu(x_new, bnorm)>>15;
	acc1 += _mpylhu(x_new, bnorm)>>15;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
	// x_new = acc2;
	x_new = acc2;

	// acc1 = (x_new*bnorm)>>31
	acc1 = _mpyhu(x_new, bnorm)<<1;
	acc1 += _mpyhlu(x_new, bnorm)>>15;
	acc1 += _mpylhu(x_new, bnorm)>>15;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
	// x_new = acc2;
	x_new = acc2;

	// acc1 = (x_new*bnorm)>>31
	acc1 = _mpyhu(x_new, bnorm)<<1;
	acc1 += _mpyhlu(x_new, bnorm)>>15;
	acc1 += _mpylhu(x_new, bnorm)>>15;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
	// x_new = acc2;
	x_new = acc2;

	// acc1 = (x_new*bnorm)>>31
	acc1 = _mpyhu(x_new, bnorm)<<1;
	acc1 += _mpyhlu(x_new, bnorm)>>15;
	acc1 += _mpylhu(x_new, bnorm)>>15;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
	// x_new = acc2;
	x_new = acc2;

	// Un-normalize and calculate result as a multiplication
	out = (a*((long long)x_new))>>(42-sbits);
//	lltemp = (a*((long long)x_new));
//	out = shift64_q31(lltemp, 42-sbits);
	if(out < 0) out++; // Rounding-fix if output is negative
	return(out);
}
*/
// ---< External variables >---

#endif // LEC_GENERAL_H
