/*****************************************************************************
 *	\file		agc.c
 *
 *	\date		2013-June-12
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
#include "true_voice_internal.h"
#if USE_DEBUG_PLOT
#include "../../true_voice/include/plot.h" // Should not be included here but used for testing of limiter
#endif
// ---< Defines >---
#define RXTX_LIM_DELAY_LENGTH	(1*BUFLEN_8KHZ) // Length of delay buffer in limiter
#define RXTX_AGC_DELAY_LENGTH	(1*BUFLEN_8KHZ) // Length of delay buffer in AGC
#define SHIFT 1

// ---< Global variabels >---
static q31 pq31TX_limit_delay[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_16[NUM_CH_MIXER_OUT][RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_pre_mixer[NUM_CH_PRE_MIXER_OUT][RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust

static q15 pq15TX_limit_delay_agc_tech[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_agc_pilot[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_agc_lmr[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust

// ls out with DTMF
static q15 pq15TX_limit_delay_ls_techMono[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_pilotMono[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_techLeft[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_techRight[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust

// ls out without DTMF
static q15 pq15TX_limit_delay_ls_techMono_CA[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_pilotMono_CA[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_techLeft_CA[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust
static q15 pq15TX_limit_delay_ls_techRight_CA[RXTX_LIM_DELAY_LENGTH]; // TX-limiter delay buffer to let dampings adjust

// Limiter settings
limitSet_t txLimSet;
limitSet16_t txLimSet16;
limitSet16_t limiter_Out[NUM_CH_MIXER_OUT];
limitSet16_t limiter_pre_mixer_Out[NUM_CH_PRE_MIXER_OUT];

// limiter settings for ls-out on BS and PT
limitSet16_t limiterLsLeft;
limitSet16_t limiterLsRight;
limitSet16_t limiterLsTechMono;
limitSet16_t limiterLsPilotMono;

// limiter settings ls-out on CA
limitSet16_t limiterLsLeft_CA;
limitSet16_t limiterLsRight_CA;
limitSet16_t limiterLsTechMono_CA;
limitSet16_t limiterLsPilotMono_CA;

// AGC setting
limitSet16_t agcTech;
limitSet16_t agcPilot;
limitSet16_t agcLmr;


// ---< Local function prototypes >---
void limiter_init_32(void);
void limit_and_delay_32(const q15 *pq15in, q15 *out, limitSet_t *lset);


// ---< Functions >---
/*****************************************************************************
 *	\brief		Initialization of agc settings
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void agc_init() {

	q15 q15gammafallAGC = 32000; // -6.59dB in 32 samples
	//q15 q15gammafallAGC = 31320; // -12.59dB in 32 samples
	q15 q15deltaRiseAGC = 16;

	//agc settings TechHeadSet
	memset(pq15TX_limit_delay_agc_tech, 0, sizeof(q15)*RXTX_AGC_DELAY_LENGTH);
	
	agcTech.iCounter			= 0;
	agcTech.iDelayIndex			= 0;
	agcTech.q15gain_set			= MAX_16;
	agcTech.q15gain_set2		= MAX_16;
	agcTech.q15gain				= MAX_16;
	agcTech.q15LIMIT_CLIP_LVL	= mult_q15(shr_q15(MAX_16,SHIFT),0x7a00);
	agcTech.pq15Delay			= pq15TX_limit_delay_agc_tech;
	agcTech.q15deltaRise		= q15deltaRiseAGC;
	agcTech.q15gammaFall		= q15gammafallAGC; //0.9766 = -6.59 dB in 32 samples (20*log10(0.9766^32)=-6.59)
	
	//agc settings PilotHeadSet
	memset(pq15TX_limit_delay_agc_pilot, 0, sizeof(q15)*RXTX_AGC_DELAY_LENGTH);
	
	agcPilot.iCounter			= 0;
	agcPilot.iDelayIndex		= 0;
	agcPilot.q15gain_set		= MAX_16;
	agcPilot.q15gain_set2		= MAX_16;
	agcPilot.q15gain			= MAX_16;
	agcPilot.q15LIMIT_CLIP_LVL	= mult_q15(shr_q15(MAX_16,SHIFT),0x7a00);
	agcPilot.pq15Delay			= pq15TX_limit_delay_agc_pilot;
	agcPilot.q15deltaRise		= q15deltaRiseAGC;
	agcPilot.q15gammaFall		= q15gammafallAGC; //0.9766 = -6.59 dB in 32 samples (20*log10(0.9766^32)=-6.59)
	
	//agc settings Lmr
	memset(pq15TX_limit_delay_agc_lmr, 0, sizeof(q15)*RXTX_AGC_DELAY_LENGTH);
	
	agcLmr.iCounter				= 0;
	agcLmr.iDelayIndex			= 0;
	agcLmr.q15gain_set			= MAX_16;
	agcLmr.q15gain_set2			= MAX_16;
	agcLmr.q15gain				= MAX_16;
	agcLmr.q15LIMIT_CLIP_LVL	= mult_q15(shr_q15(MAX_16,SHIFT),0x7a00);
	agcLmr.pq15Delay			= pq15TX_limit_delay_agc_lmr;
	agcLmr.q15deltaRise			= q15deltaRiseAGC; //0x0010; //0.001
	agcLmr.q15gammaFall			= q15gammafallAGC; //0.95
		
}

// ---< Functions >---
/*****************************************************************************
 *	\brief		
 *	\parameters	input	- input buffer
 * 				agcSet	- struct with agc settings for this channel
 *	\return		None
 *****************************************************************************/
void agc(q15 pq15input[], limitSet16_t *agcSet) {
	int k;
	
	if(tvModules.bAgc) {
		// limit and delay signals
		limit_and_delay_16(pq15input, pq15input, agcSet);
		for(k=0; k<BUFLEN_8KHZ; k++) {
				//amplify signal
				pq15input[k] = shl_q15(pq15input[k], SHIFT);
		}
	}

	return;
}


/*****************************************************************************
 *	\brief		Initialization of limiter settings 16-bit.
 *  			Used to limit output signals from pre mixer.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void limiter_init_pre_mixer() {
	int i;
	q15 q15gammafallMixer = 30650; // -18.57 dB in 32 samples TODO: different limiter setting for different mixer channels?
	//q15 q15gammafallLs 	= 31320; // -12.56 dB in 32 samples
	//q15 q15gammafallLs_CA = 31610; // -10 dB in 32 samples

	q15 q15deltaRiseMixer = 16;

	// Limiter settings for output signals from mixer
	for (i=0; i<NUM_CH_PRE_MIXER_OUT; i++){
		// Clear fullband limiter buffers
		memset(pq15TX_limit_delay_pre_mixer[i], 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);
		// Limiter settings
		limiter_pre_mixer_Out[i].iCounter			= 0;
		limiter_pre_mixer_Out[i].iDelayIndex		= 0;
		limiter_pre_mixer_Out[i].q15gain_set		= MAX_16;
		limiter_pre_mixer_Out[i].q15gain_set2		= MAX_16;
		limiter_pre_mixer_Out[i].q15gain			= MAX_16;
		//limiter_pre_mixer_Out[i].q15LIMIT_CLIP_LVL	= mult_q15(pq15PreMixerNormFactor[i],0x7a00); //in pre_mixer_init
		limiter_pre_mixer_Out[i].pq15Delay			= pq15TX_limit_delay_pre_mixer[i];
		limiter_pre_mixer_Out[i].q15deltaRise		= q15deltaRiseMixer;
		limiter_pre_mixer_Out[i].q15gammaFall		= q15gammafallMixer; //0.935=-18.6 dB in 32 samples
	}
}

/*****************************************************************************
 *	\brief		Initialization of limiter settings 16-bit.
 *  			Used to limit output signals from mixer.
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void limiter_init_16() {
	int i;
	q15 q15gammafallMixer = 30650; // -18.57 dB in 32 samples TODO: different limiter setting for different mixer channels?
	q15 q15gammafallLs 	  = 31320; // -12.56 dB in 32 samples
	q15 q15gammafallLs_CA = 31610; // -10 dB in 32 samples

	q15 q15deltaRiseMixer = 16;
	q15 q15deltaRiseLs 	  = 16;
	q15 q15deltaRiseLs_CA = 16;


	// Limiter settings for output signals from mixer	
	for (i=0; i<NUM_CH_MIXER_OUT; i++){
		// Clear fullband limiter buffers
		memset(pq15TX_limit_delay_16[i], 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);
		// Limiter settings
		limiter_Out[i].iCounter			= 0;
		limiter_Out[i].iDelayIndex		= 0;
		limiter_Out[i].q15gain_set		= MAX_16;
		limiter_Out[i].q15gain_set2		= MAX_16;
		limiter_Out[i].q15gain			= MAX_16;
		//limiter_Out[i].q15LIMIT_CLIP_LVL= mult_q15(pq15MixerNormFactor[i],0x7a00); //in mixer_init
		limiter_Out[i].pq15Delay		= pq15TX_limit_delay_16[i];
		limiter_Out[i].q15deltaRise		= q15deltaRiseMixer;
		limiter_Out[i].q15gammaFall		= q15gammafallMixer; //0.935=-18.6 dB in 32 samples
	}

	// Limiter setting for LS-out
	// with DTMF
	// Tech headset left
	memset(pq15TX_limit_delay_ls_techLeft, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsLeft.iCounter			= 0;
	limiterLsLeft.iDelayIndex		= 0;
	limiterLsLeft.q15gain_set		= MAX_16;
	limiterLsLeft.q15gain_set2		= MAX_16;
	limiterLsLeft.q15gain			= MAX_16;
	limiterLsLeft.q15LIMIT_CLIP_LVL	= mult_q15(shr_q15(MAX_16,2),0x7a00); //0.95*0.25
	limiterLsLeft.pq15Delay			= pq15TX_limit_delay_ls_techLeft;
	limiterLsLeft.q15deltaRise		= q15deltaRiseLs;
	limiterLsLeft.q15gammaFall		= q15gammafallLs; //-12.56 dB in 32 samples

	// Tech headset right
	memset(pq15TX_limit_delay_ls_techRight, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsRight.iCounter			= 0;
	limiterLsRight.iDelayIndex		= 0;
	limiterLsRight.q15gain_set		= MAX_16;
	limiterLsRight.q15gain_set2		= MAX_16;
	limiterLsRight.q15gain			= MAX_16;
	limiterLsRight.q15LIMIT_CLIP_LVL= mult_q15(shr_q15(MAX_16,2),0x7a00); //0.95*0.25
	limiterLsRight.pq15Delay		= pq15TX_limit_delay_ls_techRight;
	limiterLsRight.q15deltaRise		= q15deltaRiseLs;
	limiterLsRight.q15gammaFall		= q15gammafallLs;//0.955; //-12.56 dB in 32 samples

	// Tech mono
	memset(pq15TX_limit_delay_ls_techMono, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsTechMono.iCounter			= 0;
	limiterLsTechMono.iDelayIndex		= 0;
	limiterLsTechMono.q15gain_set		= MAX_16;
	limiterLsTechMono.q15gain_set2		= MAX_16;
	limiterLsTechMono.q15gain			= MAX_16;
	limiterLsTechMono.q15LIMIT_CLIP_LVL	= mult_q15(shr_q15(MAX_16,2),0x7a00); //0.95*0.25
	limiterLsTechMono.pq15Delay			= pq15TX_limit_delay_ls_techMono;
	limiterLsTechMono.q15deltaRise		= q15deltaRiseLs;
	limiterLsTechMono.q15gammaFall		= q15gammafallLs;  //-12.56 dB in 32 samples

	// Pilot mono
	memset(pq15TX_limit_delay_ls_pilotMono, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsPilotMono.iCounter			= 0;
	limiterLsPilotMono.iDelayIndex		= 0;
	limiterLsPilotMono.q15gain_set		= MAX_16;
	limiterLsPilotMono.q15gain_set2		= MAX_16;
	limiterLsPilotMono.q15gain			= MAX_16;
	limiterLsPilotMono.q15LIMIT_CLIP_LVL= mult_q15(shr_q15(MAX_16,2),0x7a00); //0.95*0.25
	limiterLsPilotMono.pq15Delay		= pq15TX_limit_delay_ls_pilotMono;
	limiterLsPilotMono.q15deltaRise		= q15deltaRiseLs;
	limiterLsPilotMono.q15gammaFall		= q15gammafallLs; //-12.56 dB in 32 samples

	//Without DTMF
	// Tech headset left
	memset(pq15TX_limit_delay_ls_techLeft_CA, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsLeft_CA.iCounter			= 0;
	limiterLsLeft_CA.iDelayIndex		= 0;
	limiterLsLeft_CA.q15gain_set		= MAX_16;
	limiterLsLeft_CA.q15gain_set2		= MAX_16;
	limiterLsLeft_CA.q15gain			= MAX_16;
	limiterLsLeft_CA.q15LIMIT_CLIP_LVL	= 10377; //0.95*MAX_16/3
	limiterLsLeft_CA.pq15Delay			= pq15TX_limit_delay_ls_techLeft;
	limiterLsLeft_CA.q15deltaRise		= q15deltaRiseLs_CA;
	limiterLsLeft_CA.q15gammaFall		= q15gammafallLs_CA; //-10 dB in 32 samples

	// Tech headset right
	memset(pq15TX_limit_delay_ls_techRight_CA, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsRight_CA.iCounter			= 0;
	limiterLsRight_CA.iDelayIndex		= 0;
	limiterLsRight_CA.q15gain_set		= MAX_16;
	limiterLsRight_CA.q15gain_set2		= MAX_16;
	limiterLsRight_CA.q15gain			= MAX_16;
	limiterLsRight_CA.q15LIMIT_CLIP_LVL = 10377; //0.95*MAX_16/3
	limiterLsRight_CA.pq15Delay			= pq15TX_limit_delay_ls_techRight;
	limiterLsRight_CA.q15deltaRise		= q15deltaRiseLs_CA;
	limiterLsRight_CA.q15gammaFall		= q15gammafallLs_CA; //-10 dB in 32 samples

	// Tech mono
	memset(pq15TX_limit_delay_ls_techMono_CA, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsTechMono_CA.iCounter			= 0;
	limiterLsTechMono_CA.iDelayIndex		= 0;
	limiterLsTechMono_CA.q15gain_set		= MAX_16;
	limiterLsTechMono_CA.q15gain_set2		= MAX_16;
	limiterLsTechMono_CA.q15gain			= MAX_16;
	limiterLsTechMono_CA.q15LIMIT_CLIP_LVL	= 10377; //0.95*MAX_16/3
	limiterLsTechMono_CA.pq15Delay			= pq15TX_limit_delay_ls_techMono;
	limiterLsTechMono_CA.q15deltaRise		= q15deltaRiseLs_CA;
	limiterLsTechMono_CA.q15gammaFall		= q15gammafallLs_CA; //-10 dB in 32 samples

	// Pilot mono
	memset(pq15TX_limit_delay_ls_pilotMono_CA, 0, sizeof(q15)*RXTX_LIM_DELAY_LENGTH);

	limiterLsPilotMono_CA.iCounter			= 0;
	limiterLsPilotMono_CA.iDelayIndex		= 0;
	limiterLsPilotMono_CA.q15gain_set		= MAX_16;
	limiterLsPilotMono_CA.q15gain_set2		= MAX_16;
	limiterLsPilotMono_CA.q15gain			= MAX_16;
	limiterLsPilotMono_CA.q15LIMIT_CLIP_LVL = 10377; //0.95*MAX_16/3
	limiterLsPilotMono_CA.pq15Delay			= pq15TX_limit_delay_ls_pilotMono;
	limiterLsPilotMono_CA.q15deltaRise		= q15deltaRiseLs_CA;
	limiterLsPilotMono_CA.q15gammaFall		= q15gammafallLs_CA; //-10. dB in 32 samples

}

/*****************************************************************************
 *	\brief		Generalized limiter with delay in fullband 16-bit
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void limit_and_delay_16(const q15 *pq15in, q15 *pq15out, limitSet16_t *lset) {
	const int ciLimitHold = 5;
	unsigned int i;
	q15 q15buffMax, q15damp, q15temp;
	q15 pq15tempBuf[BUFLEN_8KHZ];
	q31 q31temp;
	
	// Find MAX in signal
	q15buffMax = abs_q15(pq15in[0]);					//Is abs_q15=abs_q31 correct?
	for(i=1;i<BUFLEN_8KHZ;i++) {
		q15temp = abs_q15(pq15in[i]);
		q15buffMax = MAX(q15buffMax, q15temp);
	}
	
	// Will signal clip?
	if(q15buffMax > lset->q15LIMIT_CLIP_LVL) {
		// Yes it will! Calculate appropriate damping
		q31temp = div_q31x31(lset->q15LIMIT_CLIP_LVL, q15buffMax); //Is div_q31x31 correct to use on 16-bit?
		q15damp = q31_to_q15(q31temp);
		// Lower than possible previous set dampings?		
		lset->q15gain_set = MIN(q15damp, lset->q15gain_set);
		lset->q15gain_set2= MIN(q15damp, lset->q15gain_set2);		
	}
	
	if(--(lset->iCounter) <= 0) {
		// Reset set damping
		lset->q15gain_set = lset->q15gain_set2; // Let the gain rise at most to previous gain_set
		lset->q15gain_set2 = MAX_16;
		lset->iCounter = ciLimitHold; // Reset limiter hold time
	}
	
	// Use delay vector
	for(i=0;i<BUFLEN_8KHZ;i++) {
		pq15tempBuf[i] = lset->pq15Delay[lset->iDelayIndex];
		lset->pq15Delay[lset->iDelayIndex] = pq15in[i];
		lset->iDelayIndex = circindex(lset->iDelayIndex, 1, RXTX_LIM_DELAY_LENGTH);
	}
	
	// Apply gain
	if(lset->q15gain < lset->q15gain_set) {//rise
		for(i=0;i<BUFLEN_8KHZ;i++) {
			// Damping smoothing
			lset->q15gain = add_q15(lset->q15gain, mult_q15(lset->q15deltaRise, lset->q15gain));
			lset->q15gain = MIN(lset->q15gain, lset->q15gain_set);

			// Apply gain
			pq15out[i] = (mult_q15(lset->q15gain, pq15tempBuf[i]));
		}
	} else { //fall
		for(i=0;i<BUFLEN_8KHZ;i++) {
			// Damping smoothing
			//lset->gain =lset->gain_set;
			lset->q15gain = MAX(lset->q15gain_set, mult_q15(lset->q15gammaFall, lset->q15gain)); //fall down to the target value.
			
			// Apply gain
			pq15out[i] = (mult_q15(lset->q15gain, pq15tempBuf[i]));
		}
	}
}

/*****************************************************************************
 *	\brief		Initialization of limiter settings, 32-bit
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void limiter_init_32() {
	// Clear fullband limiter buffers
	
	memset(pq31TX_limit_delay, 0, sizeof(q31)*RXTX_LIM_DELAY_LENGTH);
	
	// Limiter settings
	txLimSet.iCounter			= 0;
	txLimSet.iDelayIndex		= 0;
	txLimSet.q31gain_set		= MAX_32;
	txLimSet.q31gain_set2		= MAX_32;
	txLimSet.q31gain			= MAX_32;
	txLimSet.q15LIMIT_CLIP_LVL	= 29000; // -1.06 dB=20*log10(29000/32767)
	txLimSet.pq31Delay			= pq31TX_limit_delay;
	txLimSet.q31deltaRise		= 0x00100000; //0.001
	txLimSet.q31gammaFall		= 0x7a000000; //0.95
	
}

/*****************************************************************************
 *	\brief		Generalized limiter with delay in fullband 16->32->16-bit
 *	\parameters	None
 *	\return		None
 *****************************************************************************/
void limit_and_delay_32(const q15 *pq15in, q15 *pq15out, limitSet_t *lset) {
	const int ciLimitHold = 5;
	unsigned int i;
	q31 q31buffMax, q31damp, q31temp;
	q31 pq31tempBuf[BUFLEN_8KHZ];
	q31 pq31in[BUFLEN_8KHZ];
	// for plots
	//q15 pq15tempbuff1[BUFLEN_8KHZ];
	//q15 pq15tempbuff2[BUFLEN_8KHZ];
	//q15 pq15tempbuff3[BUFLEN_8KHZ];
	
	for(i=0;i<BUFLEN_8KHZ;i++) {
		//pq31in[i] = shr_q31(q15_to_q31(pq15in[i]),16);
		pq31in[i] = pq15in[i];
	}
	
	// Find MAX in signal
	q31buffMax = abs_q31(pq31in[0]);
	for(i=1;i<BUFLEN_8KHZ;i++) {
		q31temp = abs_q31(pq31in[i]);
		q31buffMax = MAX(q31buffMax, q31temp);			
	}
	
	// Will signal clip?
	if(q31buffMax > lset->q15LIMIT_CLIP_LVL) {
		// Yes it will! Calculate appropriate damping
		q31damp = div_q31x31(lset->q15LIMIT_CLIP_LVL, q31buffMax);
		// Lower than possible previous set dampings?		
		lset->q31gain_set = MIN(q31damp, lset->q31gain_set);
		lset->q31gain_set2= MIN(q31damp, lset->q31gain_set2);	
	}
	
	if(--(lset->iCounter) <= 0) {
		// Reset set damping
		lset->q31gain_set = lset->q31gain_set2; // Let the gain rise at most to previous gain_set
		lset->q31gain_set2 = MAX_32;
		lset->iCounter = ciLimitHold; // Reset limiter hold time
	}
	
	// Use delay vector
	for(i=0;i<BUFLEN_8KHZ;i++) {
		lset->pq31Delay[lset->iDelayIndex] = pq31in[i];
		lset->iDelayIndex = circindex(lset->iDelayIndex, 1, RXTX_LIM_DELAY_LENGTH);
		pq31tempBuf[i] = lset->pq31Delay[lset->iDelayIndex];		
	}
	
	// Apply gain
	if(lset->q31gain < lset->q31gain_set) {//rise
		for(i=0;i<BUFLEN_8KHZ;i++) {
			// Damping smoothing
			lset->q31gain = add_q31(lset->q31gain, mult_q31x31(lset->q31deltaRise, lset->q31gain));
			lset->q31gain = MIN(lset->q31gain, lset->q31gain_set);
			//pq15tempbuff1[i]=(q15)q31_to_q15(lset->q31gain);  //for plots
		
			// Apply gain
			pq15out[i] = (q15)q31_to_q15(shl_q31(mult_q31x31(lset->q31gain, pq31tempBuf[i]), 16)); // Truncate before converting to q15
		}
	} else { //fall
		for(i=0;i<BUFLEN_8KHZ;i++) {
			// Damping smoothing
			//lset->gain =lset->gain_set;
			lset->q31gain = MAX(lset->q31gain_set, mult_q31x31(lset->q31gammaFall, lset->q31gain)); //fall down to the target value.
			//pq15tempbuff1[i]=(q15)q31_to_q15(lset->q31gain); // for plots
		
			// Apply gain
			pq15out[i] = (q15)q31_to_q15(shl_q31(mult_q31x31(lset->q31gain, pq31tempBuf[i]), 16)); // Truncate before converting to q15
		}
	}

}

