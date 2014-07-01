/*****************************************************************************
 *	\file		noise_reduction.c
 *
 *	\date		2013-May-15
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
#include "dsplib.h"
#include <math.h>
#include "expexpint.h"


// ---< Defines >---

#define VAD_WAIT_TIME ((short)100) //ms
#define VAD_TIMER_NR ((short)VAD_WAIT_TIME>>2)

// ---< Global variabels >---
//nr_status_t nr_status;
q15 q15Afft[NFFT];
q15 q15Bfft[NFFT];
vadSet_t vadSettings;

#pragma  DATA_ALIGN(twiddleFFT, 8)
#pragma  DATA_ALIGN(twiddleIFFT, 8)
#pragma  DATA_ALIGN(input, 8)
#pragma  DATA_ALIGN(output, 8)
#pragma  DATA_ALIGN(X, 8)
short twiddleFFT[2*NFFT+2*PAD];
short twiddleIFFT[2*NFFT+2*PAD];
short input[2*NFFT + 2*PAD];
short output[2*NFFT + 2*PAD];
short X[2*NFFT + 2*PAD];

q15 q15Window[2*BUFLEN_8KHZ] = {
#include "hann64.csv"
};

//TODO: Make sure somehow that if BUFLEN is not NFFT/2 that things dont break


// ---< Local function prototypes >---


// ---< Functions >---
/*****************************************************************************
 *	\brief		Updates the mic out signal with the noise reduced signal and vadded signal	
 *	\parameters	pq15x1		- noise mic
 * 				pq15x2		- Headset mic
 *	\return		None
 *****************************************************************************/
bool nr_damp(q15 *pq15x1, const q15 *pq15x2, nr_status_t *nr_status, bool bDisableVAD, short sVadSensitivity) {

	unsigned int n;	//loop variable

	q15 q15Mick[2*BUFLEN_8KHZ];
	q15 q15Noise[2*BUFLEN_8KHZ];
	//Calculate the windowed input from previous and this iteration
	memcpy(q15Mick, nr_status->q15Mick_old, BUFLEN_8KHZ*sizeof(q15));
	memcpy(q15Noise, nr_status->q15Noise_old, BUFLEN_8KHZ*sizeof(q15));
	memcpy(&q15Mick[BUFLEN_8KHZ], pq15x1, BUFLEN_8KHZ*sizeof(q15)); //Fix so that everything doesn't break if BUFLEN is not NFFT/2
	memcpy(&q15Noise[BUFLEN_8KHZ], pq15x2, BUFLEN_8KHZ*sizeof(q15));
	vecmultq15xq15(q15Mick, q15Mick, q15Window, 2*BUFLEN_8KHZ);
	vecmultq15xq15(q15Noise, q15Noise, q15Window, 2*BUFLEN_8KHZ);

	//Store old values to use in next iteration
	memcpy(nr_status->q15Mick_old, pq15x1, BUFLEN_8KHZ*sizeof(q15));
	memcpy(nr_status->q15Noise_old, pq15x2, BUFLEN_8KHZ*sizeof(q15));

	complex_q15 cq15X1[NFFT];
	complex_q15 cq15X2[NFFT];
	fftReal_q16xcq16(q15Mick, cq15X1);
	fftReal_q16xcq16(q15Noise, cq15X2);


	//Calculate FFT-magnitude
	q31 q31X1mag[NPSD];
	q31 q31X2mag_pre[NPSD];
	veccabs_square_shift_cq15oq31(q31X1mag, cq15X1, NPSD, 0);
	veccabs_square_shift_cq15oq31(q31X2mag_pre, cq15X2, NPSD, 0);

	//Calculate smoothed version of FFT-magnitude
	q31 q31X1magsmooth[NPSD];
	q31 q31X2magsmooth_pre[NPSD];
	MA_filter_q31_Asq15(q31X1magsmooth, q31X1mag, NMATAPS, NPSD);
	MA_filter_q31_Asq15(q31X2magsmooth_pre, q31X2mag_pre, NMATAPS, NPSD);

	//Inverse-PSD. To let noise PSD have same properties as mick PSD
	q31 q31X2mag[NPSD];
	q31 q31X2magsmooth[NPSD];
	vecmultq31xq31_Asq15(q31X2mag, nr_status->q31P, q31X2mag_pre, NPSD);
	vecmultq31xq31_Asq15(q31X2magsmooth, nr_status->q31Psmooth, q31X2magsmooth_pre, NPSD);

//		int hej = 0;
//		for(n=0; n<NPSD; n++){
//			if(q15X1mag[n]>MAX_16/2 || q15X2mag_pre[n]>MAX_16/2 || q15X2mag[n]>MAX_16/2 || q15X2magsmooth[n]>MAX_16/2){
//				hej = hej+1;
//			}
//		}

	//Updates PSD of micks and noise with use of FFT
	linearUpdate_q31(nr_status->q31PhiXX, q31X1mag, ALPHA1, NPSD);
	linearUpdate_q31(nr_status->q31PhiYY, q31X2mag, ALPHA1, NPSD);
	linearUpdate_q31(nr_status->q31PhiXXsmooth, q31X1magsmooth, ALPHA1, NPSD);
	linearUpdate_q31(nr_status->q31PhiYYsmooth, q31X2magsmooth, ALPHA1, NPSD);


	//Calculate PSD difference
	q31 q31PhiPLD[NPSD];
	vecsub_q31(q31PhiPLD, nr_status->q31PhiXX, nr_status->q31PhiYY, NPSD); //q15PhiPLD = nr_status->q15PhiXX - nr_status->q15PhiYY
	q31 q31PhiPLDsmooth[NPSD];
	vecsub_q31(q31PhiPLDsmooth, nr_status->q31PhiXXsmooth, nr_status->q31PhiYYsmooth, NPSD);
	//Make sure it is non-negative
	for(n=0; n<NPSD; n++){
		if(q31PhiPLD[n] < 0){
			q31PhiPLD[n] = 0;
		}
		if(q31PhiPLDsmooth[n] < 0){
			q31PhiPLDsmooth[n] = 0;
		}
	}

	//Calculate normalized difference in PSD
	q31 q31PhiPLDne[NPSD];
	q31 q31denom;
	for(n=0; n<NPSD; n++){
		q31denom = add_q31(nr_status->q31PhiXXsmooth[n], nr_status->q31PhiYYsmooth[n]);
		q31PhiPLDne[n] = div_pos_q31x31(q31PhiPLDsmooth[n], q31denom)>>16;
	}


	//Estimate PSD of noise on mick, linearly updating it element by element
	for(n=0; n<NPSD; n++){
		if(q31PhiPLDne[n] < PHI_MIN)
			nr_status->q31PhiNN[n] = mult_q15((long)ALPHA2, nr_status->q31PhiNN[n]) + mult_q15((long)sub_q15(MAX_16, ALPHA2), q31X1magsmooth[n]); //TODO: This subtraction can be made beforehand
		else if(q31PhiPLDne[n] < PHI_MAX)
			nr_status->q31PhiNN[n] = mult_q15((long)ALPHA3, nr_status->q31PhiNN[n]) + mult_q15((long)sub_q15(MAX_16, ALPHA3), q31X2magsmooth[n]); //TODO: This subtraction can be made beforehand
		//else do nothing
	}

	//Calculate if speech or not
	float fVoxscale;
	q15 q15Voxscale;
	bool speech;
	bool mute;
//		vox(&q15prob, &speech, q15Voxscale);
	voxRobin(&mute, &speech, &fVoxscale, nr_status->q31PhiXXsmooth, nr_status->q31PhiNN, &nr_status->vox_status, sVadSensitivity); //TODO: Float-based implementation of VAD, should be fixed point
	if (bDisableVAD)
		q15Voxscale = MAX_16;
	else
		q15Voxscale = fVoxscale*MAX_16;


	//Adapt new inverse PSD and scaling factor
	if(!speech){
		nr_status->bRescueAdapt = false;
		nr_status->uiSpeechCount = 0;
//		    q31 q31Pnew[NPSD];
//		    q31 q31PnewSmooth[NPSD];

		updateP(nr_status->q31P, q31X1mag, q31X2mag_pre, LAMBDA);
		updateP(nr_status->q31Psmooth, q31X1magsmooth, q31X2magsmooth_pre, LAMBDA);
	}
	else{
		if(nr_status->bRescueAdapt == true){
			nr_status->uiSpeechCount--;
			if(nr_status->uiSpeechCount <= 0)
				nr_status->bRescueAdapt = false;

			updateP(nr_status->q31P, q31X1mag, q31X2mag_pre, LAMBDA_SPEECH);
			updateP(nr_status->q31Psmooth, q31X1magsmooth, q31X2magsmooth_pre, LAMBDA_SPEECH);
		}
		else{
			nr_status->uiSpeechCount++;
			if(nr_status->uiSpeechCount >= BLOCK_RESCUE_COUNT)
				nr_status->bRescueAdapt = true;
		}
	}

	//Calculate gain filter, Gain = PhiPLD/(PhiPLD + GAMMA*PhiNN+EPS).
	q31 q31Term2[NPSD];
	q31 q31Denominator[NPSD];
	vecmultq31xScalar(q31Term2, nr_status->q31PhiNN, GAMMA, 0, NPSD);
	for(n=0; n<NPSD; n++){
		q31Denominator[n] = q31PhiPLD[n] + q31Term2[n];
	}

	q15 q15G[NPSD];
	for(n=0; n<NPSD; n++){
		q15G[n] = div_pos_q31x31(q31PhiPLD[n], q31Denominator[n])>>16;
	}

	for(n=0; n<NPSD; n++){
		if(nr_status->q15Gain[n] > q15G[n]){
			nr_status->q15Gain[n] = mult_q15(nr_status->q15Gain[n], FALL);
			nr_status->q15Gain[n] = MAX(q15G[n], nr_status->q15Gain[n]);
		}
		else{
			if(mult_q15((int)nr_status->q15Gain[n], RISE) << RISE_SHIFT  >  (int)MAX_16){//Check for overflow
				nr_status->q15Gain[n] = MAX_16;
			}
			else{
				nr_status->q15Gain[n] = mult_q15(nr_status->q15Gain[n], RISE) << RISE_SHIFT;
			}
			nr_status->q15Gain[n] = MIN(q15G[n], nr_status->q15Gain[n]);
		}
	}

	//Remove some high frequency musical noise
	musicSupp(nr_status->q15Gain, q31X1mag, ZETAthr, PSI);

	//If any values are below GAINMIN set it to GAINMIN
	//Only calculate on first NPSD values
	for(n=0; n<NPSD; n++){
		if(nr_status->q15Gain[n] < GAINMIN)
			nr_status->q15Gain[n] = GAINMIN;
	}

	//Make the gain symmetric
	for(n=1; n<NPSD; n++){
		nr_status->q15Gain[NFFT-n] = nr_status->q15Gain[n];
	}

	//Change gain on signal FFT
	vecmultq15xcq15(cq15X1, nr_status->q15Gain, cq15X1, NFFT);

	//Calculate IFFT
	q15 q15stmp[NFFT];
	ifft_cq16xcq16(cq15X1, q15stmp);
//		for(n=0; n<NFFT; n++){
//			q15stmp[n] = mult_q15(q15stmp[n], q15Voxscale);
//		}

	//Result is the first BUFLEN_8KHZ samples of cq15stmp.re
	//new s_hat = [s_tmp[1:BUFLEN_8KHZ]+s_hat[1:BUFLEN_8KHZ]; s_tmp[BUFLEN_8KHZ+1:2*BUFLEN_8KHZ]]
	//The conditional statements are to ensure no overflow
	for(n=0; n < BUFLEN_8KHZ; n++){//Only use real values from q15stmp
		if(q15stmp[n] > MAX_16-nr_status->q15Shat[BUFLEN_8KHZ + n])
			nr_status->q15Shat[n] = MAX_16;
		else if(q15stmp[n] < MIN_16-nr_status->q15Shat[BUFLEN_8KHZ + n])
			nr_status->q15Shat[n] = MIN_16;
		else
			nr_status->q15Shat[n] = nr_status->q15Shat[BUFLEN_8KHZ + n] + q15stmp[n]; 		//Half old no longer needed

		nr_status->q15Shat[n + BUFLEN_8KHZ] = q15stmp[BUFLEN_8KHZ + n];	//Half new is stored
		if (tvModules.bNoiseReduction) {
			pq15x1[n] = nr_status->q15Shat[n];										//Mick gets updated with last block that is now noise_damped
		}
	}

	if (bDisableVAD) {
		return true;
	}
	else {
		bool fadeDone = fade(!mute, pq15x1, &nr_status->hFade);
		return !(mute && fadeDone);
	}
}



//TODO: Add description
void updateP(q31 *q31P, q31 *q31X1, q31 *q31X2, q15 alpha){
	unsigned int n;
    q31 q31Pnew[NPSD];

//		    vecdiv_pos_q31xq31_Asq15(q31Pnew, q31X1mag, q31X2mag_pre, NPSD);
//		    linearUpdate_q31(nr_status->q31P, q31Pnew, LAMBDA, NPSD);

    vecdiv_pos_q31xq31_Asq15(q31Pnew, q31X1, q31X2, NPSD);
    for(n=0; n<NPSD; n++){
    	q31Pnew[n] = add_q31(mult_q31x15(q31P[n], MAX_16-alpha), mult_q31x15(q31Pnew[n], alpha));
    }
    for(n=0; n<NPSD; n++){
    	if(q31P[n] > q31Pnew[n]){
    		q31P[n] = MAX(q31Pnew[n], sub_q31(q31P[n], P_CHANGE)); //TODO: make this change constant, what happens if P=0?
    	}
    	else{
    		q31P[n] = MIN(q31Pnew[n], add_q31(q31P[n], P_CHANGE)); //TODO: make this change constant, what happens if P=0?
    	}
    }
}


//TODO: Finish VAD
/*****************************************************************************
 *	\brief		Calculates value for "how much speech" there is and estimated bool
 * 				if there is speech or not.
 *	\parameters	pq15prob		- value between 0 and 1 for "how much speech"
 * 				pq15speech		- Headset mic speech or not
 * 				q15Voxscale	- Scalar gain for smooth VAD
 *	\return		None
 *****************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Vectors needed for VAD


/*

q15 q15POld;
q15 arrq15GammaOld[NPSD];
q15 arrq15XiOld[NPSD];
q15 arrq15VOld[NPSD];


q15 arrq15MathTemp[NPSD];
q15 arrq15MathTempTwo[NPSD];
q15 arrq15MathTempThree[NPSD];

q15 arrq15OneThroughXiPlusOne[NPSD];

q15 arrq15Expi[NPSD];
q15 arrq15OldGain[NPSD];
q15 arrq15Gamma[NPSD];
q15 arrq15Ex[NPSD];
q15 arrq15Xi[NPSD];
q15 arrq15V[NPSD];
q15 arrq15Lambda[NPSD];

//////////////////
// VOX SCALING
q15 q15Vs = 0;

//////////////////////////////////////////////////////////////////////////
// Keep track of first frame or not

q15 first = 1;

//////////////////////////////////////////////////////////////////////////
// Parameters
//
// TODO: FIX SCALING OF ALL THESE VARIABLES ?

// alpha = 0.3;
q15 q15Alpha = 9830;

// (1 - alpha)
q15 q15AlphaRest = sub_q15(32768, q15Alpha);



// TODO: Not sure about these values... ?

// p_vox_begin = 0.01;
q15 q15PVoxBegin = 328;

// p_vox_end = 0.1;
q15 q15PVoxEnd= 3280;

// ss_p_h0 = p_vox_end / (p_vox_end + p_vox_begin);
q15 q15PH0 = div_pos_q15xq15(p_vox_end , add_q15(p_vox_end,p_vox_begin));

// ss_p_h1 = 1 - ss_p_h0;
q15 q15PH1 = sub_q15(32768,q15PH0);

// p_start_vox = 0.1;
q15 q15PStartVox = 3280;



// TODO: These values need to be checked in MATLAB, atm dependent on scale between PhiYY and PhiNN ???

// p_max = 5000;
//q15 q15PMax = mult_q15(5000,32768); // TODO: FIX

// p_thresh = 0.05;


// TODO: REmoved p_max, set this to reasonable value instead
q15 q15PThresh = 1000; // ???


*/
void voxRobin(bool *pbMute, bool *pq15speech, float *pq15Voxscale, const q31 *q31Pss, const q31 *q31Pnn, vox_status_t *vox_status, short sVadSensitivity) {
	unsigned int n;
	float alpha = 0.3;
	float p_vox_begin = 0.01;
	float p_vox_end = 0.1;
	float p_thresh = 5;

	switch (sVadSensitivity)
	{
	case 0:
		p_thresh = 4;
		break;
	case 1:
		p_thresh = 4.5f;
		break;
	case 2:
		p_thresh = 5;
		break;
	case 3:
		p_thresh = 5.5f;
		break;
	case 4:
		p_thresh = 6;
	}

	//Retrieving needed values from expexpint
	float fEx[NPSD];
	int idx;
	for(n=0; n<NPSD; n++){
		idx = (vox_status->fV[n]-EEI_EPSILON)/EEI_DELTA;
		if(idx < 0)
			fEx[n] = expexpint[0];
		else if(idx >= EEI_LENGTH)
			fEx[n] = expexpint[EEI_LENGTH-1];
		else
			fEx[n] = expexpint[idx];
	}

	//Calculate gain
	float fGain[NPSD];
	for(n=0; n<NPSD; n++){
		fGain[n] = vox_status->fXi[n]/(1+vox_status->fXi[n])*fEx[n];
	}

	//Calculate new value for Xi (A priori SNR) and new value for Gamma
	float newGamma;
	for(n=0; n<NPSD; n++){
		newGamma = (float)q31Pss[n]/(q31Pnn[n]+6.2); //TODO: Add std*something to denominator, make this more general
		vox_status->fXi[n] = alpha*fGain[n]*fGain[n]*vox_status->fGamma[n] + (1 - alpha)*MAX(newGamma - 1, 0);
		vox_status->fGamma[n] = newGamma;
	}

	//Calculate new value for V using new value for Xi and gamma
	for(n=0; n<NPSD; n++){
		vox_status->fV[n] = vox_status->fGamma[n]*vox_status->fXi[n]/(1+vox_status->fXi[n]);
		if(vox_status->fV[n] > 10)
			vox_status->fV[n] = 10;
	}

	//Calculate value for LambdaGM
	float fLambdaGM;
	float prodLambda = 1;
	for(n=0; n<NPSD; n++){
		prodLambda *= powf(exp(vox_status->fV[n])/(1+vox_status->fXi[n]), 1/(float)NPSD); //TODO: make a map for exp instead
	}
	fLambdaGM = prodLambda;//powf(prodLambda, 1/(float)NPSD); //TODO: Make a map for pow instead

	//Calculate value for probability of speech
	vox_status->fProb = fLambdaGM * (p_vox_begin + (1 - p_vox_end)*vox_status->fProb) / ((1 - p_vox_begin) + p_vox_end*vox_status->fProb);

	//Check if speech or not
	if(vox_status->fProb > p_thresh)
		*pq15speech = true;
	else
		*pq15speech = false;


	if (*pq15speech) {
		resetTimerHandle(&vox_status->hTimer, VAD_TIMER_NR);
		*pbMute = false;
	}
	else {
		*pbMute = isFinished(&vox_status->hTimer);
	}

	//Zero out signal or not
	vox_status->fVoxProb = vox_status->fVoxProb - 0.05*(1.4 - vox_status->fVoxProb) - 0.008;

	if(*pq15speech)
		vox_status->fVoxProb = 1.4;
	if(vox_status->fVoxProb<0)
		vox_status->fVoxProb = 0;

	*pq15Voxscale = vox_status->fVoxProb;
	if(*pq15Voxscale > 1)
		*pq15Voxscale = 1;

	return;
}



void vox(q15 *pq15prob, bool *pq15speech, q15 *pq15Voxscale) {
	/*
///////////////////////////////////////////////////////////////////////////
// Calculate arrq15Gamma, a posteriori SNR for every frequency bin
// gamma = Py ./ Pn;

// TODO: These (Py, Pn) should be scaled to same scale, store scale in variable, for best accuracy in division. How to use this?
// TODO: I think this needs to be done in noise_reduction algorithm, otherwise precision is already lost..


	vecdiv_pos_q15xq15(arrq15Gamma, nr_status->q15PhiYYsmooth, nr_status->q15PhiNN, NPSD);
	
///////////////////////////////////////////////////////////////////////////
// Init variables first frame, TODO:should be done in init function
	if(first == 1)
	{
		first = 0;

		// old_p = p_start_vox;
		q15POld = q15PStartVox;

		// Init arrq15GammOld, old value of a posteriori SNR
		// old_gamma = Py ./ Pn;
		memcpy(arrq15GammaOld, q15Gamma, sizeof(q15)*NPSD);

		// Init arrq15XiOld, old value for a priori SNR
		// old_xi = Py ./ Pn;
    	// old_xi(old_xi == 0) = 0.00001;
		memcpy(arrq15XiOld, q15Gamma, sizeof(q15)*NPSD); // TODO: FIX SO NEVER ZERO
	}

///////////////////////////////////////////////////////////////////////////
// Calculate 1 / (1 + xi), commonly used below

	// Temporary calculation
	vecaddScalar_q15(q15MathTemp, q15XiOld, 1, NPSD);
	vecreciprocal_q15(arrq15OneThroughXiPlusOne, q15MathTemp, NPSD);

///////////////////////////////////////////////////////////////////////////
// Continue initiate variables first frame
	if(first == 1)
	{
		// Temporary calculation
		vecmultq15xq15(arrq15MathTemp, arrq15GammaOld, arrq15XiOld, NPSD);

		// Init arrq15Vold, old value of exponential in likelihood calculation
		// old_v = old_gamma.*old_xi./(1+old_xi);
		vecmultq15xq15(arrq15VOld, arrq15MathTemp, arrq15OneThroughXiPlusOne, NPSD);
	}

///////////////////////////////////////////////////////////////////////////
// Calculate spectral gain function of Log Spectral Amplitude estimator when speech is surely present (Ephraim & Malah)
// exi = expint_sam(old_v);
// ex = exp(0.5*exi);
// old_gain = (old_xi ./ (1 + old_xi)) .* ex;

	// Calculate arrq15Expi, exponential integral of arrq15Vold
	arrq15Expi = expint(arrq15VOld);
	// Calculate arrq15Ex, exponential of arrq15Expi
	arrq15Ex = exp(arrq15Expi); // TODO: REPLACE WITH SOMETHING THAT WORKS FOR FIXED

	// Temporary calculation
	vecdiv_pos_q15xq15(arrq15MathTemp, arrq15XiOld, arrq15XiOldPlusOne, NPSD);

	vecmultq15xq15(arrq15OldGain, arrq15Ex, arrq15MathTemp, NPSD);

///////////////////////////////////////////////////////////////////////////
// Calculate a priori SNR by modified decision directed method (Cohen 2003):
// xi = alpha * old_gain.^2 .* old_gamma + (1 - alpha).*max(gamma - 1, 0);

	vecmultq15xq15(arrq15MathTemp, arrq15OldGain, arrq15OldGain, NPSD);
	vecmultq15xq15(arrq15MathTempTwo, arrq15MathTemp, arrq15GammaOld, NPSD);
	vecmultq15xScalar(arrq15MathTempThree, arrq15MathTempTwo, alpha, NPSD);

	vecaddScalar_q15(arrq15MathTemp, arrq15Gamma, -1, NPSD);
	
	for(q15 i = 0; i < NPSD; i++) 
		if(arrq15MathTemp[i] < 0)	arrq15MathTemp[i] = 0;

	vecmultq15xScalar(arrq15MathTempTwo, arrq15MathTemp, q15AlphaRest, NPSD);

	vecadd_q15(arrq15Xi, arrq15MathTempTwo, arrq15MathTemp, NPSD);

///////////////////////////////////////////////////////////////////////////
// Calculate v, used in likelihood calculation 
// v = gamma.*xi./(1+xi);
// v(v > 10) = 10;

	// Temporary calculation, gamma.*xi
	vecmultq15xq15(arrq15MathTemp, arrq15Gamma, arrq15Xi, NPSD);

	vecmultq15xq15(arrq15V, arrq15MathTemp, arrq15OneThroughXiPlusOne, NPSD);

///////////////////////////////////////////////////////////////////////////
// Calculate arrq15Lambda, likelihood ratio for every frequency bin
// lambda = (1 / 1 + xi).*exp(v);

	arrq15MathTemp = exp(arrq15V); // TODO: FIX
	
	vecmultq15xq15(arrq15Lambda, arrq15OneThroughXiPlusOne, arrq15MathTemp, NPSD);

//////////////////////////////////////////////////////////////////////////
// Calculate logarithmic mean of all lambdas
// lambda_gm = exp(mean(log(lambda)));

	q15 q15LambdaGM = veclogmean_q15(arrq15Lambda);

//////////////////////////////////////////////////////////////////////////
// Calculate p
// p = lambda_gm .* (p_vox_begin + (1 - p_vox_end).*old_p) ./ ((1 - p_vox_begin) + p_vox_end .*old_p);
	
	// TODO: Check which order to do these to preserve accuracy ?
	q15 tempOne = add_q15(q15PVoxBegin, mult_q15(sub_q15(32768,q15VoxEnd), q15POld));
	q15 tempTwo = add_q15(sub_q15(32768, q15PVoxBegin), mult_q15(q15PVoxEnd, q15POld));

	q15 p = mult_q15(q15LambdaGM, div_pos_q15xq15(tempOne, tempTwo));
	//q15 p = q15LambdaGM * (q15PVoxBegin + (32768 - q15PVoxEnd)*q15POld) / ((32768 - q15PVoxBegin) + q15PVoxEnd*q15POld));

//////////////////////////////////////////////////////////////////////////
// Store vectors for next iteration

	memcpy(arrq15GammaOld, arrq15Gamma, sizeof(q15) * NPSD);
	memcpy(arrq15XiOld, arrq15Xi, sizeof(q15) * NPSD);
	memcpy(arrq15VOld, arrq15V, sizeof(q15) * NPSD);
	q15POld = p;

//////////////////////////////////////////////////////////////////////////
// Calculate voice / no voice from threshold of p, and calculate scaling
// TODO: FIX THIS PART
	q15 scaleConstant = 100; // TODO: Choose this constant
	q15 scaledP = mult_q15(p, scaleConstant);

	if(scaledP > p_thresh) {
		v = 1;
	}
	else {
		v = 0;
	}

	// TODO: Do experiments and choose these. Atm linear release.

	q15Vs = sub_q15(q15Vs, 500);
	//vs = vs - 0.05 * (1.4 - vs) - 0.008;

	if(v) q15Vs = 32000;
	//if(v) vs = 1.4;

	if(q15Vs < 0) q15Vs = 0;

	q15 voxScale = vs;

	// 24000 = 1
	if(voxScale > 24000)
		voxScale = 24000;

	pq15prob* = 0; // TODO: Not used...
	pq15speech* = v;
	pq15Voxscale* = voxScale;
//////////////////////////////////////////////////////////////////////////
*/

	*pq15prob = 0;
	*pq15speech = 0;
	*pq15Voxscale = 0;

	return;
}

/*****************************************************************************
 *	\brief		Removes musical noise from gain filter.
 *	\parameters	q15G              	- is the filter to be smoothed, goes from 0 to nyquist-frequency in frequency.
 *				q15X1mag            - is the FFT of the input signal, goes from 0 to nyquist-frequency in frequency.
 *				usThresh			- Parameter that decides how to threshold the normalized power gain
 *				q15Scalingfactor  	- Parameter that decides how big the moving average filter will be
 *				uiSmoothThres    	- Sets a threshold for which frequencies the G filter should be smoothed (given in start sample)
 *	\return		None
 *****************************************************************************/
void musicSupp(q15 *q15G, const q31 *q31X1mag, unsigned short usThresh, unsigned int scalingfactor){
	unsigned int n;
	q15 zeta = 0;
	q31 denominator = 0;
	q31 numerator = 0;

	for(n=0; n<NPSD; n++){
		numerator += mult_q15(q15G[n], q31X1mag[n]);
		denominator += q31X1mag[n];
	}
	zeta = (div_pos_q31x31(numerator, denominator)>>16)-1; //Divide and store as q15
	short zetaT = 1;
	if(zeta < usThresh)
		zetaT = (short)zeta/MAX_16;

	short T = 1;
	if (zetaT != 1)
	    T = (short) 2*((1-zetaT/usThresh)*scalingfactor)+1; //TODO: Can I just ignore rounding?

	q15 tmp[NPSD-F0];
	memcpy(tmp, &q15G[F0], (NPSD-F0)*sizeof(q15)); //TODO: Do I really need to copy over this?
	MA_filter(&q15G[F0], tmp, T, NPSD-F0);
	return;
}


/*****************************************************************************
 *	\brief		Multiplies two vectors of type q15 element by element
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq15VecA		- vector to be multiplied elementwise with the other vector
 * 				pq15VecB		- vector to be multiplied elementwise with the other vector
 * 				arrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecmultq15xq15(q15 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = mult_q15(pq15VecA[i], pq15VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Multiplies two vectors of type q31 and q15. The result is in q15 and so
 *				there's a chance of overflow in which case the result is MAX_16,
 *				the q31 vectors values is interpreted as q15.
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq31VecA		- vector to be multiplied elementwise with the other vector
 * 				pq15VecB		- vector to be multiplied elementwise with the other vector
 * 				arrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
//TODO: Optimize this function
void vecmultq31xq15(q15 *pq15Output, const q31 *pq31VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	long result;
	for(i=0; i<uiArrayLength; i++){
		result = (pq31VecA[i]*pq15VecB[i]);
		if(result > 0x3FFF8000)
			pq15Output[i] = MAX_16;
		else
			pq15Output[i] = result>>15;
	}
	return;
}


/*****************************************************************************
 *	\brief		Multiplies two vectors of type q31 and q31. The result is in q31.
 *				The q31 vectors values is interpreted as q15.
 *	\parameters	pq31Output		- Vector to store the result in
 * 				pq31VecA		- vector to be multiplied elementwise with the other vector
 * 				pq31VecB		- vector to be multiplied elementwise with the other vector
 * 				arrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
//TODO: Optimize this function
void vecmultq31xq31_Asq15(q31 *pq31Output, const q31 *pq31VecA, const q31 *pq31VecB, const unsigned int uiArrayLength){
	unsigned int i;
	long long result;
	for(i=0; i<uiArrayLength; i++){
		result = mult_q15((long long)pq31VecA[i], pq31VecB[i]);
		if(result > MAX_32)
			pq31Output[i] = MAX_32;
		else
			pq31Output[i] = result;
	}
	return;
}

/*****************************************************************************
 *	\brief		Multiplies two vectors of type q15 and complex_q15 element by element
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq15VecA		- vector to be multiplied elementwise with the other vector
 * 				pq15VecB		- Complex vector to be multiplied elementwise with the other vector
 * 				arrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecmultq15xcq15(complex_q15 *pq15Output, const q15 *pq15VecA, const complex_q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i].re = mult_q15(pq15VecA[i], pq15VecB[i].re);
		pq15Output[i].im = mult_q15(pq15VecA[i], pq15VecB[i].im);
	}
	return;
}

/*****************************************************************************
 *	\brief		Divides one vector by another of type q15 element by element, all elements must be positive!
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq15VecA		- vector to be multiplied with reciprocal of the other vector
 * 				pq15VecB		- vector to divide the other vector
 * 				uiArrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecdiv_pos_q15xq15(q15 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = div_pos_q15xq15(pq15VecA[i], pq15VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Divides one vector by another of type q31 element by element, all elements must be positive!
 *	\parameters	pq31Output		- Vector to store the result in
 * 				pq31VecA		- vector to be multiplied with reciprocal of the other vector
 * 				pq31VecB		- vector to divide the other vector
 * 				uiArrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecdiv_pos_q31xq31(q31 *pq31Output, const q31 *pq31VecA, const q31 *pq31VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq31Output[i] = div_pos_q31x31(pq31VecA[i], pq31VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Divides one vector by another of type q31 element by element, all elements must be positive, result is interpreted as q15!
 *	\parameters	pq31Output		- Vector to store the result in
 * 				pq31VecA		- vector to be multiplied with reciprocal of the other vector
 * 				pq31VecB		- vector to divide the other vector
 * 				uiArrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecdiv_pos_q31xq31_Asq15(q31 *pq31Output, const q31 *pq31VecA, const q31 *pq31VecB, const unsigned int uiArrayLength){
	unsigned int i;
	unsigned short shift;
	for(i=0; i<uiArrayLength; i++){
		shift = 0;
		//pq31Output[i] = div_pos_q31x31(pq31VecA[i], pq31VecB[i])>>16;
		while(pq31VecA[i]>>shift > MAX_16 || pq31VecB[i]>>shift >MAX_16){
			shift++;
		}
		pq31Output[i] = div_pos_q15xq15oq31(pq31VecA[i]>>shift, pq31VecB[i]>>shift);
	}
	return;
}

/*****************************************************************************
 *	\brief		Divides one vector by another of type q15 element by element, all elements must be positive!
 *				Result can be greater than MAX_16 and is stored in a q31
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq15VecA		- vector to be multiplied with reciprocal of the other vector
 * 				pq15VecB		- vector to divide the other vector
 * 				arrayLength		- Length of arrays that are multiplied together
 *	\return		None
 *****************************************************************************/
void vecdiv_pos_q15xq15oq31(q31 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = div_pos_q15xq15oq31(pq15VecA[i], pq15VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Multiplies one vector of type q15 element by element with a scalar
 *	\parameters	pq15Output		- Vector to store the result in
 * 				pq15Vec			- vector to be multiplied elementwise with scalar
 * 				scalar		- scalar to multiply vector with
 * 				shift			- How much to shift down result
 * 				arrayLength		- Length of array
 *	\return		None
 *****************************************************************************/
void vecmultq15xScalar(q15 *pq15Output, const q15 *pq15Vec, int scalar, unsigned short shift, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = (pq15Vec[i]*scalar)>>shift;
	}
	return;
}

/*****************************************************************************
 *	\brief		Multiplies one vector of type q31 element by element with a scalar
 *	\parameters	pq31Output		- Vector to store the result in
 * 				pq31Vec			- vector to be multiplied elementwise with scalar
 * 				scalar		- scalar to multiply vector with
 * 				shift			- How much to shift down result
 * 				arrayLength		- Length of array
 *	\return		None
 *****************************************************************************/
void vecmultq31xScalar(q31 *pq31Output, const q31 *pq31Vec, int scalar, unsigned short shift, const unsigned int uiArrayLength){
	unsigned int i;
	long result;
	for(i=0; i<uiArrayLength; i++){
		result = (pq31Vec[i]*scalar)>>shift;
		if(result < MAX_32)
			pq31Output[i] = result;
		else
			pq31Output[i] = MAX_32;
	}
	return;
}


/*****************************************************************************
 *	\brief		Computes the magnitude squared
 *	\parameters	pq15Output		- Vector to store the results in
 * 				pcq15z			- vector of complex values to compute magnitude squared of
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void veccabs_square(q15 *pq15Output, const complex_q15 *pcq15z, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = cabs_square(pcq15z[i]);
	}
	return;
}

/*****************************************************************************
 *	\brief		Computes the magnitude squared but shifts down by s. result is (a^2 + b^2)>>uiShift, capped by
 *	\parameters	pq15Output		- Vector to store the results in
 * 				pcq15z			- vector of complex values to compute magnitude squared of
 * 				uiArrayLength	- Length of arrays
 * 				uiShift			- How much to shift
 *	\return		None
 *****************************************************************************/
void veccabs_square_shift_q15(q15 *pq15Output, const complex_q15 *pcq15z, const unsigned int uiArrayLength, unsigned int uiShift){
	unsigned int i;
	unsigned int tmp;
	for(i=0; i<uiArrayLength; i++){
		tmp = (pcq15z[i].re*pcq15z[i].re + pcq15z[i].im*pcq15z[i].im)>>uiShift;
		if(tmp > MAX_16)
			tmp = MAX_16;
		pq15Output[i] = (q15)tmp;
	}
	return;
}


/*****************************************************************************
 *	\brief		Computes the magnitude squared but shifts down by s. result is (a^2 + b^2)>>uiShift, capped by
 *	\parameters	pq15Output		- Vector to store the results in
 * 				pcq15z			- vector of complex values to compute magnitude squared of
 * 				uiArrayLength	- Length of arrays
 * 				uiShift			- How much to shift
 *	\return		None
 *****************************************************************************/
void veccabs_square_shift_cq15oq31(q31 *pq31Output, const complex_q15 *pcq15z, const unsigned int uiArrayLength, unsigned int uiShift){
	unsigned int i;
	unsigned int tmp;
	for(i=0; i<uiArrayLength; i++){
		tmp = (pcq15z[i].re*pcq15z[i].re + pcq15z[i].im*pcq15z[i].im)>>uiShift;
		if(tmp > MAX_32)
			tmp = MAX_32;
		pq31Output[i] = (q31)tmp;
	}
	return;
}

/*****************************************************************************
 *	\brief		Computes the magnitude squared
 *	\parameters	pq15Output		- Vector to store the results in
 * 				pq15Input		- vector of complex values to compute magnitude squared of
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecrecp1_q15(q15 *pq15Output, const q15 *pq15Input, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = recp1_q15(pq15Input[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Linearly updates one vector with another vector according to Update = (1-alpha)*Update + alpha*With;
 *	\parameters	pq15Update		- Vector to update
 * 				pq15With		- vector used to update
 * 				alpha			- speed of adaptation
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void linearUpdate_q15(q15 *pq15Update, const q15 *pq15With, const q15 alpha, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Update[i] = add_q15(mult_q15(pq15Update[i], MAX_16-alpha), mult_q15(pq15With[i], alpha)); //TODO: The subtraction can be made beforehand
	}
	return;
}


/*****************************************************************************
 *	\brief		Linearly updates one vector with another vector according to Update = (1-alpha)*Update + alpha*With;
 *	\parameters	pq31Update		- Vector to update
 * 				pq31With		- vector used to update
 * 				alpha			- speed of adaptation
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void linearUpdate_q31(q31 *pq31Update, const q31 *pq31With, const q15 alpha, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq31Update[i] = add_q31(mult_q31x15(pq31Update[i], MAX_16-alpha), mult_q31x15(pq31With[i], alpha)); //TODO: The subtraction can be made beforehand
	}
	return;
}


/*****************************************************************************
 *	\brief		Adds two vectors together element by element, using function for q15
 *	\parameters	pq15Output		- Vector to store result in
 * 				pq15VecA		- Vector to add together with the other vector
 * 				pq15VecB		- Vector to add together with the other vector
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecadd_q15(q15 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = add_q15(pq15VecA[i], pq15VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Adds two vectors together element by element
 *	\parameters	output		- Vector to store result in
 * 				vecA		- Vector to add together with the other vector
 * 				vecB		- Vector to add together with the other vector
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecadd(int *output, const int *vecA, const int *vecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		output[i] = vecA[i] + vecB[i];
	}
	return;
}


/*****************************************************************************
 *	\brief		Adds a vector with a scalar element by element
 *	\parameters	pq15Output		- Vector to store result in
 * 				pq15VecA		- Vector to add together with the other vector
 * 				q15Scalar		- Vector to add together with the other vector
 * 				uiArrayLength	- Length of array
 *	\return		None
 *****************************************************************************/
void vecaddScalar_q15(q15 *pq15Output, const q15 *pq15VecA, const q15 q15Scalar, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = add_q15(pq15VecA[i], q15Scalar);
	}
	return;
}


/*****************************************************************************
 *	\brief		Subtracts two vectors, element by element.
 *	\parameters	pq15Output		- Vector to store result in
 * 				pq15VecA		- Vector to add
 * 				pq15VecB		- Vector to subtract
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecsub_q15(q15 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq15Output[i] = sub_q15(pq15VecA[i], pq15VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Subtracts two vectors, element by element.
 *	\parameters	pq31Output		- Vector to store result in
 * 				pq31VecA		- Vector to add
 * 				pq31VecB		- Vector to subtract
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecsub_q31(q31 *pq31Output, const q31 *pq31VecA, const q31 *pq31VecB, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		pq31Output[i] = sub_q31(pq31VecA[i], pq31VecB[i]);
	}
	return;
}


/*****************************************************************************
 *	\brief		Shifts vector A with shift vector, element by element
 *	\parameters	pq15Output		- Vector to store result in
 * 				pq15VecA		- Vector to be shifted
 * 				pq15VecB		- Vector to with how much to shift, negative values mean right shift
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecshift_q15(q15 *pq15Output, const q15 *pq15VecA, const q15 *pq15VecShift, const unsigned int uiArrayLength){
	unsigned int i;
	for(i=0; i<uiArrayLength; i++){
		if(pq15VecShift>=0)
			pq15Output[i] = pq15VecA[i]<<pq15VecShift[i];
		else
			pq15Output[i] = pq15VecA[i]>>-pq15VecShift[i];
	}
	return;
}


/*****************************************************************************
 *	\brief		Shifts vector A with shift in every element
 *	\parameters	pq15Output		- Vector to store result in
 * 				pq15VecA		- Vector to be shifted
 * 				shift			- How much to shift every element of VecA, negative values mean right shift
 * 				uiArrayLength	- Length of arrays
 *	\return		None
 *****************************************************************************/
void vecshift_scalar(int *output, const int *vecA, const short shift, const unsigned int uiArrayLength){
	unsigned int i;
	if(shift>=0){
		for(i=0; i<uiArrayLength; i++){
			output[i] = vecA[i]<<shift;
		}
	}
	else{
		for(i=0; i<uiArrayLength; i++){
			output[i] = vecA[i]>>-shift;
		}
	}
	return;
}


/*****************************************************************************
 *	\brief		Calculates moving average of given length with equal weighting
 *	\parameters	output			- Vector to store result in
 * 				input			- input signal
 * 				maSize			- Size of moving average filter, weights will be 1/maSize, must be an odd integer
 * 				length			- length of signal array
 *	\return		-1 if maSize is even. Returns 1 on success.
 *****************************************************************************/
int MA_filter_q31_Asq15(q31 output[], q31 input[], int maSize, int length) {
	if(maSize%2==0)
		return -1;
	int n, m;
	q15 weight = (q15)MAX_16/maSize;
	long result = 0;
	long long filtered;

	short mHalf = maSize>>1;
	short startEnd = length-1-mHalf;

	//Initialize result
	for(m=mHalf; m > mHalf-maSize; m--){
		if(m < 0)
			result += input[0];
		else
			result += input[m];
	}
	filtered = mult_q15((long long)result, weight);
	if(filtered < MAX_32)
		output[0] = filtered;
	else
		output[0] = MAX_32;

	//Calculate beginning
	for(n=1; n<mHalf+1; n++){
		result -= input[0];
		result += input[n+mHalf];
		filtered = mult_q15((long long)result, weight);
		if(filtered < MAX_32)
			output[n] = filtered;
		else
			output[n] = MAX_32;
	}

	//Calculate middle
	for(n=mHalf+1; n<startEnd; n++){
		result -= input[n-mHalf-1];
		result += input[n+mHalf];
		filtered = mult_q15((long long)result, weight);
		if(filtered < MAX_32)
			output[n] = filtered;
		else
			output[n] = MAX_32;
	}

	//Calculate end
	for(n=startEnd; n<length; n++){
		result -= input[n-mHalf-1];
		result += input[length-1];
		filtered = mult_q15((long long)result, weight);
		if(filtered < MAX_32)
			output[n] = filtered;
		else
			output[n] = MAX_32;
	}

	return 1;
}


/*****************************************************************************
 *	\brief		Calculates moving average of given length with equal weighting
 *	\parameters	output			- Vector to store result in
 * 				input			- input signal
 * 				maSize			- Size of moving average filter, weights will be 1/maSize, must be an odd integer
 * 				length			- length of signal array
 *	\return		-1 if maSize is even. Returns 1 on success.
 *****************************************************************************/
int MA_filter(q15 output[], q15 input[], int maSize, int length) {
	if(maSize%2==0)
		return -1;
	int n, m;
	q15 weight = (q15)MAX_16/maSize;
	long result = 0;

	short mHalf = maSize>>1;
	short startEnd = length-1-mHalf;

	//Initialize result
	for(m=mHalf; m > mHalf-maSize; m--){
		if(m < 0)
			result += input[0];
		else
			result += input[m];
	}
	output[0] = mult_q15(result, weight);

	//Calculate beginning
	for(n=1; n<mHalf+1; n++){
		result -= input[0];
		result += input[n+mHalf];
		output[n] = mult_q15(result, weight);
	}

	//Calculate middle
	for(n=mHalf+1; n<startEnd; n++){
		result -= input[n-mHalf-1];
		result += input[n+mHalf];
		output[n] = mult_q15(result, weight);
	}

	//Calculate end
	for(n=startEnd; n<length; n++){
		result -= input[n-mHalf-1];
		result += input[length-1];
		output[n] = mult_q15(result, weight);
	}

	return 1;
}



///*****************************************************************************
// *	\brief		Utilizing the DSPlib fft, calculating fft of real input.
// *				Make sure the PAD define is at least 2.
// *	\parameters	twiddle			- Vector with the twiddle factors for the fft
// * 				pq15Input		- Input vector to be transformed
// * 				pcq15Output		- Vector to store the result in
// *	\return		None
// *****************************************************************************/
void fftReal_q16xcq16(const q15 *pq15Input, complex_q15 *pcq15Output){
	unsigned int n;
	unsigned short shiftby = 3; //This is sorta arbitrary

	for(n=0; n<NFFT; n++){
		input[2*n+PAD] = pq15Input[n]>>shiftby; //TODO: How much should I shift, 3 seems to work fine for sine signal
		input[2*n+1+PAD] = 0;
	}
	DSP_fft16x16(&twiddleFFT[PAD], NFFT, &input[PAD], &X[PAD]); //TODO: global variables input and output should be the same to save memory
	for(n=0; n<NFFT; n++){
		pcq15Output[n].re = X[2*n+PAD];
		pcq15Output[n].im = X[2*n+1+PAD];
	}


	//TODO: Algorithm for real ifft, finish this instead.
//	//Copy over the signal values, notice that real values are inserted att odd indeces where only
//	//imaginary numbers are supposed to be. So the X after FFT is wrong, we will fix that below.
//	for(n=0; n<2*BUFLEN_8KHZ; n++){
//		input[n + PAD] = pq15Input[n]>>shiftby; //shift to prevent overflow
//	}
//	//Truncate zeros at end
//	for(n=2*BUFLEN_8KHZ; n<NFFT; n++){
//		input[n + PAD] = 0;
//	}
//	//Insert value at PAD
//	for(n=0; n<PAD; n++){
//		input[n] = 0;
//		input[NFFT + 2*PAD-1-n] = 0;
//	}
//
//	DSP_fft16x16(&twiddleFFT[PAD], NFFT/2, &input[PAD], &X[PAD]);
//	X[NFFT+PAD] = X[0];
//	X[NFFT+1+PAD] = X[1];
//	//Fix the X according to http://processors.wiki.ti.com/index.php/Efficient_FFT_Computation_of_Real_Input
//	for(n=0; n<NFFT/2; n++){
//		pcq15Output[n].re = mult_q15(X[2*n+PAD], q15Afft[2*n]) - mult_q15(X[2*n+1+PAD], q15Afft[2*n+1]) + mult_q15(X[NFFT-2*n+PAD], q15Bfft[2*n]) + mult_q15(X[NFFT-2*n+1+PAD], q15Bfft[2*n+1]);
//		pcq15Output[n].im = mult_q15(X[2*n+1+PAD], q15Afft[2*n]) + mult_q15(X[2*n+PAD], q15Afft[2*n+1]) + mult_q15(X[NFFT-2*n+PAD], q15Bfft[2*n+1]) - mult_q15(X[NFFT-2*n+1+PAD], q15Bfft[2*n]);
//	}
//	for(n=1; n<NFFT/2; n++){
//		pcq15Output[NFFT-n].re = pcq15Output[n].re;
//		pcq15Output[NFFT-n].im = -pcq15Output[n].im;
//	}
//	pcq15Output[NFFT/2].re = X[PAD] - X[PAD+1];
//	pcq15Output[NFFT/2].im = 0;
	return;
}

/*****************************************************************************
 *	\brief		Utilizing the DSPlib ifft
 *	\parameters	twiddle			- Vector with the twiddle factors for the fft
 * 				pq15Input		- Input vector to be transformed
 * 				pcq15Output		- Vector to store the result in
 *	\return		None
 *****************************************************************************/
void ifft_cq16xcq16(const complex_q15 *pcq15Input, q15 *pq15Output){
	unsigned int n;
	unsigned short shiftby = 1;//This is sorta arbitrary
	for(n=0; n<NFFT; n++){
		X[2*n+PAD] = pcq15Input[n].re;
		X[2*n+1+PAD] = pcq15Input[n].im;
	}
	DSP_ifft16x16(&twiddleIFFT[PAD], NFFT, &X[PAD], &output[PAD]);
	for(n=0; n<NFFT; n++){
		pq15Output[n] = output[2*n+PAD]<<shiftby; //TODO: How much should I shift, 1 seems to work fine for sine signal, will it overflow ever though?
	}

	//TODO: Algorithm for real ifft, finish this instead.
//	for(n=1; n<NFFT/2; n++){
//		X[2*n + PAD] = mult_q15(pcq15Input[n].re, q15Afft[2*n]) + mult_q15(pcq15Input[n].im, q15Afft[2*n+1]) + mult_q15(pcq15Input[NFFT/2-n].re, q15Bfft[2*n]) - mult_q15(pcq15Input[NFFT/2-n].im, q15Bfft[2*n+1]);
//		X[2*n + 1 + PAD] = mult_q15(pcq15Input[n].im, q15Afft[2*n]) - mult_q15(pcq15Input[n].re, q15Afft[2*n+1]) - mult_q15(pcq15Input[NFFT/2-n].re, q15Bfft[2*n+1]) - mult_q15(pcq15Input[NFFT/2-n].im, q15Bfft[2*n]);
//	}
//	//n=0
//	X[PAD] = mult_q15(pcq15Input[0].re, q15Afft[0]) + mult_q15(pcq15Input[0].im, q15Afft[1]) + mult_q15(pcq15Input[0].re, q15Bfft[0]) - mult_q15(pcq15Input[0].im, q15Bfft[1]);
//	X[1 + PAD] = mult_q15(pcq15Input[0].im, q15Afft[0]) - mult_q15(pcq15Input[0].re, q15Afft[1]) - mult_q15(pcq15Input[0].re, q15Bfft[1]) - mult_q15(pcq15Input[0].im, q15Bfft[0]);
//
//	//Insert value at PAD
//	for(n=0; n<PAD; n++){
//		X[n] = 0;
//		X[NFFT + 2*PAD-1-n] = 0;
//	}
//
//
//	DSP_ifft16x16(&twiddleIFFT[PAD], NFFT/2, &X[PAD], &output[PAD]);
//
//	for(n=0; n<NFFT; n++){
//		pq15Output[2*n] = output[2*n + PAD]<<shiftby;
//		pq15Output[2*n+1] = output[2*n + 1 + PAD]<<shiftby;
//	}

	return;
}



/*****************************************************************************
 *	\brief		Calculating the twiddle factors for the DSPlib FFT
 *	\parameters	w				- Vector with the twiddle factors for the fft
 * 				n 				- Length of FFT
 *	\return		None
 *****************************************************************************/
int gen_twiddle_fft16x16(q15 *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] =  d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] =  d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] = -d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] = -d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] = -d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] = -d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] =  d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] =  d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}

/*****************************************************************************
 *	\brief		Calculating the twiddle factors for the DSPlib IFFT
 *	\parameters	w				- Vector with the twiddle factors for the fft
 * 				n 				- Length of FFT
 *	\return		None
 *****************************************************************************/
int gen_twiddle_ifft16x16(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] = -d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] = -d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] =  d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] = -d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] =  d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] = -d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] = -d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] = -d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}

int gen_twiddle_ifft16x16_sa(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] = -d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] = -d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] = -d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] =  d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] = -d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] =  d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] = -d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] = -d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}


/* ======================================================================== */
/*  D2S -- Truncate a 'double' to a 'short', with clamping.                 */
/* ======================================================================== */
static q15 d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (q15)d;
}


//Put this in use
/*****************************************************************************
 *	\brief		Initializes everything that is used in the noise_reduction
 *				algorithm
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void gen_fft_realfactors(){
	unsigned int i;
	double M = 32767.5;
	for (i = 0; i < NFFT/2; i++)
	{
		q15Afft[2 * i]     = d2s(M * 0.5 * (1.0 - sin (2 * PI / (double) (2 * NFFT) * (double) i)));//d2s(M * 0.5 * (1.0 - sin (2 * PI / (double) (2 * NFFT) * (double) i)));
		q15Afft[2 * i + 1] = d2s(M * 0.5 * (-1.0 * cos (2 * PI / (double) (2 * NFFT) * (double) i)));
		q15Bfft[2 * i]     = d2s(M * 0.5 * (1.0 + sin (2 * PI / (double) (2 * NFFT) * (double) i)));
		q15Bfft[2 * i + 1] = d2s(M * 0.5 * (1.0 * cos (2 * PI / (double) (2 * NFFT) * (double) i)));
	}
}




/*****************************************************************************
 *	\brief		Initializes everything that is used in the noise_reduction
 *				algorithm
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
//TODO: Make this more general to be able to handle several VADs and NRs
void nr_init(){
	unsigned int n;


	//TEST DSPlib
	gen_twiddle_fft16x16(&twiddleFFT[PAD], NFFT); //Generate twiddle factors for FFT
	gen_twiddle_ifft16x16_sa(&twiddleIFFT[PAD], NFFT); //Generate twiddle factors for FFT
	//////////////

	//Calculate/import window
//	q15 tmp[2*BUFLEN_8KHZ] = {
//		#include "hann64.csv"
//	};
//	for(n=0; n<2*BUFLEN_8KHZ; n++){
//		nr_status->q15Window[n] = tmp[n];
//	}

	//Generate factors needed to calculate real FFT and IFFT from DSPlib implementation
	gen_fft_realfactors();



}

void nr_status_init(nr_status_t *nr_status){
	unsigned int n;

	//Set nr_status->q15Mick_old to zeros
	//Set nr_status->q15Noise_old to zeros
	for(n=0; n<BUFLEN_8KHZ; n++){
		nr_status->q15Mick_old[n] = 0;
		nr_status->q15Noise_old[n] = 0;
	}

	//Set nr_status->q15PhiXX to zeros
	//Set nr_status->q15PhiYY to zeros
	//Set nr_status->q15PhiXXsmooth to zeros
	//Set nr_status->q15PhiYYsmooth to zeros
	//Set nr_status->q15PhiNN to zeros
	for(n=0; n<NPSD; n++){
		nr_status->q31PhiXX[n] = 0;
		nr_status->q31PhiYY[n] = 0;
		nr_status->q31PhiXXsmooth[n] = 0;
		nr_status->q31PhiYYsmooth[n] = 0;
		nr_status->q31PhiNN[n] = 0;
	}

	//Set q15P to a good starting value
	//Set q15Psmooth to a good starting value
	for(n=0; n<NPSD; n++){
		nr_status->q31P[n] = MAX_16;
		nr_status->q31Psmooth[n] = MAX_16;
	}

	//Set nr_status->q15Gain to ones
	for(n=0; n<NFFT; n++){
		nr_status->q15Gain[n] = MAX_16;
	}

	//Set nr_status->q15Shat to zeros[2*BUFLEN_8KHZ];
	for(n=0; n<2*BUFLEN_8KHZ; n++){
		nr_status->q15Shat[n] = 0;
	}
	nr_status->bRescueAdapt = false;
	nr_status->uiSpeechCount = 0;

	resetFadeHandle(&nr_status->hFade, false);

	//Initializing VAD
	nr_status->vox_status.fVoxProb = 0;
	nr_status->vox_status.fProb = 0.1;
	for(n=0; n<NPSD; n++){
		nr_status->vox_status.fGamma[n] = 1;
		nr_status->vox_status.fXi[n] = 1;
		nr_status->vox_status.fV[n] = nr_status->vox_status.fGamma[n]*nr_status->vox_status.fXi[n]/(1+nr_status->vox_status.fXi[n]);
	}
	resetTimerHandle(&nr_status->vox_status.hTimer, VAD_TIMER_NR);
	short i;
	for (i=0; i<2; i++)
	{
		vadSettings.sVadSensitivity[i] = 2;
		vadSettings.bVadStatus[i] = 0;
		vadSettings.bVadOverride[i] = 0;
	}
}
