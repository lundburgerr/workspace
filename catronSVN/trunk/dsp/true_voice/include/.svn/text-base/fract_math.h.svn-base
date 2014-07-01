/*****************************************************************************
 *	\file		fract_math.h
 *
 *	\date		2013-May-28
 *
 *	\brief		Fractional (q15, q31) format declaration and arithmetics.
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef FRACT_MATH_H
#define FRACT_MATH_H

// ---< Include >---

// ---< Composites declaration >---
// 15-bit fractional + sign bit
typedef short q15;
// Complex fractional
typedef struct {
	q15 re;
	q15 im;
} complex_q15;

// 31-bit fractional + sign bit
typedef int q31;
// Complex fractional
typedef struct {
	q31 re;
	q31 im;
} complex_q31;

// Booleans. Alternative is to use #include <stdbool.h>
#ifndef false
	#define false 0
#endif
#ifndef true
	#define true 1
#endif
typedef unsigned int bool;


// ---< Defines >---
// Domain for fractional types (q15 and q31)
#define MAX_8 ((char)127)
#define MIN_8 ((char)(-128))
#define MAX_16 ((short)32767)
#define MIN_16 ((short)(-32768))	// There are problems when casting 0x8000 as short, use decimal form instead
#define MAX_32 ((int)2147483647)
#define MIN_32 ((int)(-2147483647 - 1))
#define MAX_64	((long long)0x7fffffff)
#define MIN_64	((long long)0x80000000)
#define MAX_UI	((unsigned int)0xFFFFFFFF)

// Circular indexing for buffer
//		i		- Current position
//		step	- Desired increase/decrease
//		len		- Buffer length
#define circindex(i, step, len) (((i)+(step)+(len))%(len))
// Circular indexing for buffer length that is a power of 2
#define circindex_pow2(i, step, len) (((i)+(step)+(len))&((len)-1))

// Minimum and maximum value
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))

// q31 absolute value
#define abs_q31(a)			_abs(a)
#define abs_q15(a)			_abs(a)
// q15 -> q31 conversion
#define q15_to_q31(a)		((q31)(a)<<16)
// q31 -> q15 conversion, IS THIS CORRECT ???
#define q31_to_q15(a)		((q31)(a)>>16)

// q15 left shift. Handles saturation and negative shifts
#define shl_q15(a,s)		(q31_to_q15(shl_q31(q15_to_q31(a),(s)))) // without macros: ((_sshvl((a)<<16,(s)))>>16)
#define shl_q15_pos(a,s)	((q15)(MIN((int)a<<s,MAX_16)))
// q15 right shift
#define shr_q15(a,s)		shl_q15((a),-(s))
// q15 right shift, a and s must be >= 0
#define shr_q15_pos(a,s) 	((q15)(a)>>(s))
// q31 left shift with saturation
#define shl_q31(a,s)		_sshvl(a,s)
// q31 right shift with saturation
#define shr_q31(a,s)		_sshvr(a,s)

// q15 + q15 addition
#define add_q15(a,b)		((q15)_sadd2(a,b))
// q31 + q31 addition
#define add_q31(a,b)		_sadd(a,b)
// q31 - q31 subtraction
#define sub_q31(a,b)		_ssub(a,b)
// q15 - q15 subtraction
#define sub_q15(a,b)		_ssub2(a,b) //TODO: Test if it works

// q15 x q15 multiplication, result is q15
// NOTE: There is an error when calculating -32768 * -32768 = -32768 but we will have to live with that...
#define mult_q15(a,b)		(((a)*(b))>>15)		//_mpyh(a,b) ??
// q15 x q15 multiplication, result is q31
#define mult_q31(a,b)		_smpy((int)a,(int)b)
// q31 x q15 multiplication, result is q31
#define mult_q31x15(a,b)	_mpylir(b,a)
// q31 x q31 multiplication, result is 64 bits
#define multll_q31x31(a,b)	_mpy32ll(a,b)


// ---< Function prototypes >---
// ---< External variables >---


// ---< Inline functions >---
// Shifts a 64 bit variable (long long) down to q31
inline q31 shift64_q31(const long long a, const unsigned int shift) {
	unsigned int hi, lo;
	q31 out;

	hi = (unsigned int)_hill(a);
	lo = (unsigned int)_loll(a);
	out = (hi<<(32-shift))|(lo>>shift);
	return(out);
}

// q31 x q31 multiplication with q31 (rounded!) as result
// NOTE: Error when calculating mult_q31x31(MIN_32, MIN_32) gives result MIN_32
inline q31 mult_q31x31(const q31 a, const q31 b) {
	long long res_64;

	res_64 = multll_q31x31(a, b);
	res_64 += 1L<<(31-1); // Rounding
	return(shift64_q31(res_64, 31));
}



// Squared magnitude of a complex variable
inline q15 cabs_square(const complex_q15 z) {
	q15 a, b, c;

	a = abs_q15(z.re);
	b = abs_q15(z.im);
	c = mult_q15(a, a);
	c = add_q15(c, mult_q15(b, b));
	return(c);
}

// Squared magnitude of a complex variable, input is q15 variable
inline q15 cabs_square_q15(const complex_q15 z) {
	q31 a, b, c;

	a = abs_q15(z.re);
	b = abs_q15(z.im);
	c = mult_q15(a, a);
	c = add_q15(c, mult_q15(b, b));
	return(c);
}

// Approximate complex absolute value (~1dB max error)
inline q31 cabs_approx(const complex_q31 z[]) {
	q31 a, b;

	a = abs_q31(z->re);
	b = abs_q31(z->im);
	return(add_q31(MAX(a,b), shr_q31(MIN(a,b), 1)));
}

// Complex multiplication (with conjugate) : c = a*conj(b)
inline complex_q31 cmultc_q31x31(const complex_q31 a[], const complex_q31 b[]) {
	complex_q31 c;

	c.re = add_q31(mult_q31x31(a->re, b->re), mult_q31x31(a->im, b->im));
	c.im = sub_q31(mult_q31x31(a->im, b->re), mult_q31x31(a->re, b->im));
	return(c);
}

/*****************************************************************************/
/*
 *	\brief	q31 by q31 division
 *	\input	a	- Numerator (täljare)
 * 			b	- Denominator (nämnare)
 *	\output	Quotient
 *	\date	2010-07-08
 *	\author	Markus Borgh
 */
static inline q31 div_q31x31(q31 a, q31 b) {
	q31 bnorm, out;
	unsigned int x_new;
	unsigned int acc1, acc2;
	unsigned short sbits;
	long long lltemp;

	// Check for negative arguments
	if(b < 0) {
		// -a/-b = a/b
		// a/-b = -a/b
		a = -a;
		b = -b;
	}

	// Check for overflow
	if(abs_q31(a) >= abs_q31(b)) {

		// a determines the sign of the result
		if(a < 0) return(MIN_32);
		else return(MAX_32);
	}

	// Normalize (left shift until the two most significant bits become non-equal)
	sbits = (unsigned short)_norm(b);
	bnorm = (int)(b << sbits);

	acc1 = (unsigned int)bnorm + (unsigned int)bnorm;
	// Using initial guess x0 = 2.9142 - 2*b
	x_new = 0x17504816C - acc1;

	// 4 Newton-Raphson iterations
	// (see e.g. http://en.wikipedia.org/wiki/Division_%28digital%29#Newton.E2.80.93Raphson_division)

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
//	acc1 += _mpyu(x_new, bnorm)>>31;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
//	acc2 += _mpyu(x_new, acc1)>>31;
	// x_new = acc2;
	x_new = acc2;

	// Un-normalize and calculate result as a multiplication
//	out = (a*((long long)x_new))>>(31-sbits);
	lltemp = (a*((long long)x_new));
	out = shift64_q31(lltemp, 31-sbits);
	if(out < 0) out++; // Rounding-fix if output is negative
	return(out);
}


//TODO: norm should be able to calculate leading ones aswell
/*****************************************************************************/
/*
 *	\brief	calculates number of leading zeros in binary representation
 *	\input	a	- variable to calclute leading zeros in
 *	\output	Quotient
 *	\date	2013-07-09
 *	\author	Robin Lundberg
 */
 //TODO: Can I use _norm - 16 instead of this function?
static inline unsigned short norm_q15 (q15 x)
{
 if (x ==0)
  return 16;
 unsigned short n=0;
 if ((x & 0xFF00) == 0) { n += 8; x =x << 8;} //1111 1111 0000 0000 // 8 bits from left are zero! so we omit 8left bits
 if ((x & 0xF000) == 0){ n += 4; x = x <<  4;} // 4 left bits are 0
 if ((x & 0xC000) ==0){ n += 2; x = x <<  2;} // 2 left bits are 0
 if ((x & 0x8000) == 0) n += 1;  // 1 left bits are zero
 return n;
}


/*****************************************************************************/
/*
 *	\brief	q15 by q15 division where both numerator and denominator MUST be positive!!
 * 			This function is not optimized and have unnecessary round off errors.
 *	\input	a	- Numerator (täljare)
 * 			b	- Denominator (nämnare)
 *	\output	Quotient
 *	\date	2013-07-08
 *	\author	Robin Lundberg
 */
static inline q15 div_pos_q15xq15(q15 a, q15 b) {
	q15 bnorm, out;
	unsigned short x_new;
	unsigned short acc1, acc2;
	unsigned short sbits;
	bool cond;

	// Overflow condition
	cond = (a > b);

	// Normalize (left shift until the two most significant bits become non-equal)
	sbits = norm_q15(b)-1;
	bnorm = (short)(b << sbits);
	
	acc1 = (unsigned short)bnorm + (unsigned short)bnorm;
	// Using initial guess x0 = 2.9142 - 2*b //TODO: Can I do this when everything should be below 1?
	x_new = 0x17502 - acc1;

	// 3 Newton-Raphson iterations //TODO: Maybe it should be 2 iterations?
	// (see e.g. http://en.wikipedia.org/wiki/Division_%28digital%29#Newton.E2.80.93Raphson_division)

//TODO: Everything is floored now, round better, as in the q31 version
//1
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;
//2
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;
//3	
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;

	// Un-normalize and calculate result as a multiplication
	out = MIN((a*x_new)>>(15-sbits), MAX_16);
	// Overflow?
	out = MAX(out, cond*MAX_16);
	return(out);
}


/*****************************************************************************/
/*
 *	\brief	q15 by q15 division where both numerator and denominator MUST be positive!!
 *			Values are stored in q31 so the result can be greater than MAX_16
 * 			This function is not optimized and have unnecessary round off errors.
 *	\input	a	- Numerator (täljare)
 * 			b	- Denominator (nämnare)
 *	\output	Quotient
 *	\date	2013-08-05
 *	\author	Robin Lundberg
 */
static inline q31 div_pos_q15xq15oq31(q15 a, q15 b) {
	unsigned long tmp;
	q31 out;
	q15 bnorm;
	unsigned short x_new;
	unsigned short acc1, acc2;
	unsigned short sbits;

	// Normalize (left shift until the two most significant bits become non-equal)
	sbits = norm_q15(b)-1;
	bnorm = (short)(b << sbits);

	acc1 = (unsigned short)bnorm + (unsigned short)bnorm;
	// Using initial guess x0 = 2.9142 - 2*b //TODO: Can I do this when everything should be below 1?
	x_new = 0x17502 - acc1;

	// 3 Newton-Raphson iterations //TODO: Maybe it should be 2 iterations?
	// (see e.g. http://en.wikipedia.org/wiki/Division_%28digital%29#Newton.E2.80.93Raphson_division)

//TODO: Everything is floored now, round better, as in the q31 version
//1
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;
//2
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;
//3
	acc1 = _mpyhu(x_new<<16, bnorm<<16)>>(16-1);
	acc1 = 0xFFFE - acc1;
	acc2 = _mpyhu(x_new<<16, acc1<<16)>>(16-1);
	x_new = acc2;

	// Un-normalize and calculate result as a multiplication
	//TODO: Can this ever even overflow, even if it does, does this even help? DO I need the tmp variable?
	//tmp = (a*x_new)>>(15-sbits);
	tmp = (a*x_new)>>(15-sbits);
	out = MIN(tmp, MAX_32);
	return(out);
}


/*****************************************************************************/
/*
 *	\brief	q31 by q31 division where both numerator and denominator MUST be positive!!
 *	\input	a	- Numerator (täljare)
 * 			b	- Denominator (nämnare)
 *	\output	Quotient
 *	\date	2010-07-08
 *	\author	Markus Borgh
 */
static inline q31 div_pos_q31x31(q31 a, q31 b) {
	q31 bnorm, out;
	unsigned int x_new;
	unsigned int acc1, acc2;
	unsigned short sbits;
	long long lltemp;
	bool cond;

	// Overflow condition
	cond = (a > b);

	// Normalize (left shift until the two most significant bits become non-equal)
	sbits = (unsigned short)_norm(b);
	bnorm = (int)(b << sbits);

	acc1 = (unsigned int)bnorm + (unsigned int)bnorm;
	// Using initial guess x0 = 2.9142 - 2*b
	x_new = 0x17504816C - acc1;

	// 4 Newton-Raphson iterations
	// (see e.g. http://en.wikipedia.org/wiki/Division_%28digital%29#Newton.E2.80.93Raphson_division)

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
//	acc1 += _mpyu(x_new, bnorm)>>31;
	// 2 - acc
	acc1 = 0xFFFFFFFE - acc1;
	// acc2 = (x_new*acc1)>>31
	acc2 = _mpyhu(x_new, acc1)<<1;
	acc2 += _mpyhlu(x_new, acc1)>>15;
	acc2 += _mpylhu(x_new, acc1)>>15;
//	acc2 += _mpyu(x_new, acc1)>>31;
	// x_new = acc2;
	x_new = acc2;

	// Un-normalize and calculate result as a multiplication
//	out = (a*((long long)x_new))>>(31-sbits);
	lltemp = (a*(long long)x_new);
	out = shift64_q31(lltemp, 31-sbits);
	// Overflow?
	out = MAX(out, cond*MAX_32);
	return(out);
}




// Smoothing for e.g. calculating averages
static inline q31 smoothing(const q31 in, const q31 ave, const q31 ky, const short s) {
	q31 out;

	out = shl_q31(mult_q31x31(in, MAX_32 - ky), s);
	out = add_q31(out, mult_q31x31(ave, ky));
	return(out);
}

static inline q31 smoothing2(const q31 in, const q31 ave, const q31 ky_rise, const q31 ky_fall, short s) {
	q31 ky;

	// Rise or fall?
	if(in > shr_q31(ave, s))
		ky = ky_rise;
	else
		ky = ky_fall;

	return(smoothing(in, ave, ky, s));
}

// Smoothing for e.g. calculating averages
static inline q15 smoothing_q15(const q15 in, q15 ave, const q15 ky_rise, const q15 ky_fall, short s) {
	q15 out, ky;

	// Rise or fall?
	if(in > shr_q15(ave, s))
		ky = ky_rise;
	else
		ky = ky_fall;

	out = shl_q15(mult_q15(in, MAX_16 - ky), s);
	out = add_q15(out, mult_q15(ave, ky));
	return(out);
}

// Smoothing for e.g. calculating averages
static inline q15 lec_smoothing_q15(const q15 in, q15 ave, const q15 ky, const unsigned int s) {
	q15 out;

	out = shl_q15(mult_q15(in, MAX_16 - ky), s);
	out = add_q15(out, mult_q15(ave, ky));
	return(out);
}

// Multiply (q31 * q31 = q31) with shifting (WITHOUT saturation!)
static inline q31 mult_q31x31_shift(q31 x, q31 y, unsigned int s) {
	long long acc = (long long)x*y;
	return(acc >> (31LL - s));
}


//Calculating 1/(1+x), where x>0
//TODO: Optimize this using no floating point arithmetics, http://en.wikipedia.org/wiki/Division_%28digital%29#Newton.E2.80.93Raphson_division
static inline q15 recp1_q15(const q15 q15Input){
//	q15 out;
//	div_pos_q15xq15((MAX_16+q15Input)>>1, )
//	out = (q15) MAX_16/(MAX_16+q15Input);
//	return out;
	return div_pos_q15xq15(MAX_16>>1, (MAX_16+q15Input)>>1);
}


#endif // FRACT_MATH_H
