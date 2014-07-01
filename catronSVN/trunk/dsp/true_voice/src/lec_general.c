/*****************************************************************************
 *	\file		lec.c
 *
 *	\date		2013-June-11
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
#include <string.h>
#include "lec_general.h"
#include "true_voice_internal.h"
#include "lec.h"
#include "lec2.h"
#include "subband.h"
#include "fract_math.h"


// ---< Defines >---

// ---< Global variabels >---

const unsigned int puiAEC_coefs_shift[NUM_ACTIVE_SUB] = {
	0,0,0,0,0,0,0,0,//0-7
	0,0,0,0,0,0,0,0,//8-15
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0
};

// ---< Local function prototypes >---

// ---< Functions >---

#define NOI_EST_START_VALUE (MAX_16)
void noi_est_init(noiEstSet_t *nSet,q31 *nEst) {
	nSet->gamma			= 0x7e000000; // ??//0.95
	nSet->gamma_inv		= div_q31x31((MAX_32 - nSet->gamma), NUM_ACK);
	nSet->beta			= (q31)(0.01*MAX_32);//  20*log10(1.03^(256/8))=8dB
	nSet->j				= 0;
	nSet->k				= 0;
	nSet->bNoiEstAct		= false;
	nSet->bInstantFall	= false;
	nSet->bEstimateNoise	= false;
	memset(nSet->pq31block, 0, sizeof(nSet->pq31block));
	memset(nSet->pq31max1, 0, sizeof(nSet->pq31max1));
	memset(nSet->pq31max2, 0, sizeof(nSet->pq31max2));
	memset(nSet->pq31min1, MAX_8, sizeof(nSet->pq31min1));
	memset(nSet->pq31min2, MAX_8, sizeof(nSet->pq31min2));
	nSet->noiEst=nEst;
	memset(nSet->noiEst, NOI_EST_START_VALUE, sizeof(q31)*NUM_ACTIVE_SUB);
}

void lec_init(bool reset_all) {
	internal_lec_init(reset_all);
	internal_lec_2_init_2(reset_all);
}

void lec1_init(bool reset_all) {
	internal_lec_init(reset_all);
}




/*****************************************************************************
 *	\brief		Removes echoes in the receive side for analog phone lines FTN
 * 				and CLT. There are two possible CLT connections, 2-wire and 4-wire.
 * 				LEC will only be applied to the 2-wire connection. The LEC is
 * 				performed in BS only.
 *	\parameters	Clt2wire[]	- CLT 2-wire input
 * 				Clt4wire[]	- CLT 4-wire input
 * 				Clt[]		- CLT output
 * 				Ftn[]		- FNT input/output
 *	\return		None
 *****************************************************************************/
void lec(q15 pq15Clt2wire[], q15 pq15Clt4wire[], q15 pqCltToLine[],
		q15 pqFtnToLine[], q15 pq15Clt[], q15 pq15Ftn[]) {
	// do nothing if the platform is not TV_CONFIG_BS
	if (tvSettings.platform != TV_CONFIG_BS) {
		return;
	}

	// No LEC on 4 wire or when pilot is not talking to StriC. We just copy the data to the out buffer.
	if (tvSettings.bCltConnection == 0 /*4 wire*/) {
		memcpy(pq15Clt, pq15Clt4wire, BUFLEN_8KHZ * sizeof(q15));
	} else {
		// The LEC is turned of. We just copy the data to the out buffer.
		if (!tvModules.bLec || tvSettings.bPilotSwitch/*pilot is not talking to StriC*/) {
			memcpy(pq15Clt, pq15Clt2wire, BUFLEN_8KHZ * sizeof(q15));
		} else {
			// The real LEC (only performed on 2-wire)'
			internal_lec(pq15Clt2wire, pqCltToLine, pq15Clt);
		}
	}

	//Ftn LEC
	if (tvModules.bLec) {
		internal_lec_2(pq15Ftn, pqFtnToLine, pq15Ftn);
	} // do nothing if LEC is disabled.

	return;
}

void lec_aircraft_cable(q15 pq15PilotCable[], q15 pq15PilotMonoCable[]) {
	// do nothing if the platform is not TV_CONFIG_BS or TV_CONFIG_CA
	if (tvSettings.platform == TV_CONFIG_PT) {
		return;
	}

	if (tvModules.bLec) {
		if (tvSettings.platform == TV_CONFIG_BS && tvSettings.bPilotSwitch ){
			internal_lec(pq15PilotCable, pq15PilotMonoCable, pq15PilotCable);
			return;
		} // do nothing if Pilot in aircraft choose to talk to StriC

		if (tvSettings.platform == TV_CONFIG_CA) {
			internal_lec(pq15PilotCable, pq15PilotMonoCable, pq15PilotCable);
			return;
		}
	}
}

/*****************************************************************************/
/*
 *	\brief	Multiplies vector with complex conjugate of second vector
 *	\input	h_vec[]	-
 * 			x_vec[]	-
 * 			index	-
 * 			length	-
 *	\output
 *	\date	2010-07-02
 *	\author	Christian Schüldt
 */

 complex_q31 cvecdot_conj(const complex_q15 h_vec[], const complex_q31 x_vec[],
		const int index, const int length_delay, const int length_filt) {
	int i, k;
	int itemp;
	long l_real, l_imag;
	long long lltemp;
	complex_q31 out;

	// Rounding
	l_real = 0;	//1L<<14;
	l_imag = 0;	//1L<<14;
	k = index;

	_nassert((int)length_delay >= 8);
	_nassert((int)length_filt >= 8);
	_nassert((int)length_delay % 8 == 0);
	_nassert((int)length_filt % 4 == 0);
#pragma MUST_ITERATE(4,,4);
	for (i = 0; i < length_filt; i++) {
		// Next sample in delay buffer
		k = circindex_pow2(k, 1, length_delay);
		// Read 4 bytes from memory (one 32bit words (real- and imaginary part))
		itemp = _amem4_const(&h_vec[i]);
		// Read 8 bytes from memory (two 32bit words (real- and imaginary part))
		lltemp = _amem8_const(&x_vec[k]);

		//  l_real += _mpylill(itemp, _loll(lltemp));
		//  l_imag += _mpylill(itemp, _hill(lltemp));
		//  l_real += _mpyhill(itemp, _hill(lltemp));
		//  l_imag -= _mpyhill(itemp, _loll(lltemp));
		l_real += _mpylir(itemp, _loll(lltemp));
		l_imag += _mpylir(itemp, _hill(lltemp));
		l_real += _mpyhir(itemp, _hill(lltemp));
		l_imag -= _mpyhir(itemp, _loll(lltemp));


	}
	out.re = (q31) (l_real);
	out.im = (q31) (l_imag);
	return (out);
}


// Find largest coefficient and calculate gamma value for a
void find_minmaxgamma(complex_q15 coefs[][NLMS_FILTER_LENGTH], q15 *pmax, unsigned int *pmax_idx, q15 *plast, q31 *pgamma, const unsigned short subband) {
	complex_q15 cq15temp;
	complex_q31 cq31temp;
	q15 q15temp;
	q31 tap_max;
	unsigned int idx_max;
	q31 q31temp;
	unsigned int i;

	// Find largest filter coefficient
	// Step through all coefficients
	tap_max = 0;
	idx_max = 0;
	for(i=0;i<NLMS_FILTER_LENGTH;i++) {

		// Squared magnitude
		q31temp = mult_q31(coefs[subband][i].re, coefs[subband][i].re);
		q31temp += mult_q31(coefs[subband][i].im, coefs[subband][i].im);
		// Max?
		if(q31temp > tap_max) {
			tap_max = q31temp;
			idx_max = i;
		}
	}
	// Largest tap magnitude
	cq31temp.re = q15_to_q31(coefs[subband][idx_max].re);
	cq31temp.im = q15_to_q31(coefs[subband][idx_max].im);
	pmax[subband] = q31_to_q15(cabs_approx(&cq31temp));
	pmax_idx[subband] = idx_max;

	q31temp = 0;
	// End of filter (last 4 coefficients)
	for(i=0;i<4;i++) {
		cq15temp = coefs[subband][NLMS_FILTER_LENGTH-1-i];
		cq31temp.re = q15_to_q31(cq15temp.re);
		cq31temp.im = q15_to_q31(cq15temp.im);
		q31temp = add_q31(q31temp, cabs_approx(&cq31temp));
	}
	q15temp = q31_to_q15(q31temp)>>1;
	// Store last value in public buffer
	plast[subband] = q15temp;

	i = NLMS_FILTER_LENGTH - pmax_idx[subband];
	if(pmax[subband] > 0 && i > 0) {
		// Calculate gamma-value
		//q15temp = powf(q15temp/pmax[subband], 1.0f/(float)i);
		q31temp = (q31)(powf( (float)plast[subband]/pmax[subband], 1.0f/(float)i ) * MAX_32);
		// Store in public buffer
		pgamma[subband] = q31temp;
	}
	//adjust for shifting of the Coefficients

#if AEC_COEFS_SHIFT_NEG
	plast[subband] = shr_q15(plast[subband], puiAEC_coefs_shift[subband]);
	pmax[subband] = shr_q15(pmax[subband], puiAEC_coefs_shift[subband]);
#else
	plast[subband] = shr_q15(plast[subband], puiAEC_coefs_shift[subband]);
	pmax[subband] = shr_q15(pmax[subband], puiAEC_coefs_shift[subband]);
#endif
}

void cvecvaddsmult(complex_q15 *__restrict h_vec, const complex_q31 *__restrict x_vec, const complex_q31 c_step, const int index) {
	// Pointers for faster complex_q15/q31-array addressing
	q15 *__restrict hvec_ptr = (q15*)h_vec;
	q31 *__restrict xvec_ptr = (q31*)x_vec;
	unsigned int i, k;
	int temp1, temp2;

	k = index<<1;
	for(i=NLMS_FILTER_LENGTH<<1;i!=0;i-=2) {

		k = circindex(k, 2, NLMS_FILTER_LENGTH<<1);

		// Add real part
		temp1 = mult_q19x19_shift(xvec_ptr[k], c_step.re);
		temp2 = mult_q19x19_shift(xvec_ptr[k], c_step.im);
		// Add imaginary part
		temp1 -= mult_q19x19_shift(xvec_ptr[k+1], c_step.im);
		temp2 += mult_q19x19_shift(xvec_ptr[k+1], c_step.re);

		// Store the update coefficient.
		*hvec_ptr++ += (q15)temp1;
		*hvec_ptr++ += (q15)temp2;
	}
}

// Removing the noice reduction since we think that it is not needed
void noise_reduce(q15 *__restrict gains, q31 *__restrict signal_ave, q31 *__restrict noi_est, bool bEnabled, bool bLS) {
	unsigned int i;
	//q31 temp31;
	//int NOI_DAMP_SHIFT;

	if(/*!bEnabled*/1) {
		// Module is not enabled --> No damping
		for(i=0;i<NUM_ACTIVE_SUB;i++) {
			gains[i] = MAX_16;
		}
		return;
	}
	/*
	if(bLS){
		NOI_DAMP_SHIFT=NOI_DAMP_SHIFT_LS;
	}else{
		NOI_DAMP_SHIFT=NOI_DAMP_SHIFT_TX;
	}

	// Desired gains for noise reduction
	for(i=0;i<NUM_ACTIVE_SUB;i++) {
		// Gain = signal_ave/noi_est * 0.25
		temp31 = shr_q31(signal_ave[i], NOI_DAMP_SHIFT+1);  //we demand 6dB over the noise level to start "release the damping"
		temp31 = div_q31x31(temp31, noi_est[i]); // division by 0 is not an issue

		// No more than 12 dB damping
		temp31 = max_q31(temp31, MAX_32>>NOI_DAMP_SHIFT);
		gains[i] = q31_to_q15(temp31);
	}
	*/
}

// Calculate the average damping on TX and adjust the dampings to make them more similar.
// This is to avoid "processing"-noise during doubletalk.
void resdamp_tx_gains_adjust(q15 gain_set[], q31 aerr_fg[]) {
	const unsigned int SUBBAND_SPLIT = 20;
	unsigned int i;
	q31 temp32;
	q15 gain_ave;
	q31 weight32;

	// Calculate sum of all dampings, weighted with the
	temp32 = 0;
	weight32 = 0;
	for(i=0;i<SUBBAND_SPLIT;i++) {
		// num_active_sub = 65, 2^6=64, shift down 6 bits to avoid saturation
		temp32 = add_q31(temp32, shr_q31(mult_q31x31(aerr_fg[i], q15_to_q31(gain_set[i])), 6));
		weight32 = add_q31(weight32, shr_q31(aerr_fg[i], 6));
	}
	gain_ave = q31_to_q15(div_q31x31(temp32, weight32));
	for(;i<NUM_ACTIVE_SUB;i++) {
		// num_active_sub = 65, 2^5=64, shift down 6 bits to avoid saturation
		temp32 = add_q31(temp32, shr_q31(mult_q31x31(aerr_fg[i], q15_to_q31(gain_set[i])), 6));
		weight32 = add_q31(weight32, shr_q31(aerr_fg[i], 6));
	}
	gain_ave = MIN(gain_ave, q31_to_q15(div_q31x31(temp32, weight32)));

	// Adjust dampings
	for(i=0;i<NUM_ACTIVE_SUB;i++) {
		if(gain_set[i] > shl_q15_pos(gain_ave, 1))
			gain_set[i] = shl_q15_pos(gain_ave, 1); // Allow 6 dB less damping than average
		else if(gain_set[i] < shr_q15_pos(gain_ave, 3))
			gain_set[i] = shr_q15_pos(gain_ave, 3); // Allow 12 dB more damping than average
	}
}

// Smoothing of TX subband gains
void resdamp_tx_gains_smooth(bool bIsBtalk, q15 *gain_set, q15 *gain) {
	q15 gamma_rise, gamma_fall;
	unsigned int i;

	if(bIsBtalk) {
		// B-talk / Doubletalk
		gamma_rise = 0x7000;
		gamma_fall = 0x6000;
	} else {
		// A-talk - Noise reduction
		gamma_rise = 0; // Instant rise
		gamma_fall = 0x7a00; // Slow fall
	}

	// Smooth gains
	for(i=0;i<NUM_ACTIVE_SUB;i++) {
		gain[i] = lec_smoothing2_q15(gain_set[i], gain[i], gamma_rise, gamma_fall, 0);
	}
}

// Apply subband gains
void resdamp_apply_gains(q15 *gains, complex_q31 *in, complex_q31 *out) {
	// Pointers for faster complex_q31-array addressing
	q31 *in_ptr = (q31*)in;
	q31 *out_ptr = (q31*)out;
	q15 temp;
	unsigned int i;

	// Apply gains in each subband
	for(i=NUM_ACTIVE_SUB;i!=0;i--) {
		temp = *gains++;
		*out_ptr++ = mult_q31x15(*in_ptr++, temp); // Real part
		*out_ptr++ = mult_q31x15(*in_ptr++, temp); // Imaginary part
	}
}
