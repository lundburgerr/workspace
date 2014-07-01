/*****************************************************************************
 *	\file		lec.h
 *
 *	\date		2013-Aug-13
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
#ifndef LEC_H
#define LEC_H

// ---< Include >---

// ---< Defines >---
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

// ---< Function prototypes >---

void lec_subband_nlms(complex_q31*, const complex_q31*, complex_q31*, complex_q31*);
void lec_subband_fgfilt(const complex_q31*, complex_q31*, complex_q31*);
void lec_check_performance();
void lec_compare_filters();
void lec_ls_delay_put(complex_q31 *);
void lec_ls_delay_get(complex_q31 *);


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

#endif // LEC_H
