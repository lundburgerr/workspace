/*
 *	\file		dumpdata.c
 *
 *	\authors	Magnus Berggren (magnus.berggren@limestechnology.com)
 *
 *	\date		2011.02.20
 *
 *	\brief		Dumping of data for Fenix.
 *
 *	\version	20110203, v0.1
 *
 *	\todo		
 *   
 *	Copyright	Limes Audio AB
 *	
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 */

/********************************************************************************/


/* ============  SETTINGS FOR MEMORY DUMP  ============
 * File Format:				TI Data Format (.dat)
 * Format:					Integer
 * Start Address:			dumpdata.headerInfo
 * Memory Page:				DATA
 *
 */

// ---< Includes >---

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "true_voice_internal.h"
#include "lec.h"

// ---< Defines >---


#ifdef USE_DUMPDATA

#define KEEP_TRUE

//#define END_OF_BUFFER_SILENCE 2000

// Size of plotting-buffers in SDRAM
#define SUBPLOT_BUFFER_SIZE (64)//64 //aproximately 10 sec if subbplot_intervall is 16.
#define SUBPLOT_INTERVALL 16
#define NR_SUBBANDS 32 //1

#define DUMP_BUFFER_SIZE (SUBPLOT_BUFFER_SIZE*NR_SUBBANDS) //ca 10sec 

#define START_SUBBAND 0//96//99//6//9//(1+6+6+6+6)
#define STRIDE_SUBBAND 1//8//16

#define NR_FULLPLOTS 2
#define NR_SUBPLOTS 25
// Number of booleans dumped
#define NR_BOOLPLOTS 1
#define NR_BOOLPLOTS_2 3 //subband boolplots
#define HEADER_SIZE 11
// Maximum length of variable name
#define NAME_LENGTH 20




unsigned long dump_tick = 0;
unsigned long dump_coefs_tick = 0;
unsigned long coefs_dump_index=0;

#pragma  DATA_SECTION (tempName, "extvar")
#pragma  DATA_SECTION (dumpdata, "extvar")

#pragma  DATA_ALIGN(tempName, 64)
#pragma  DATA_ALIGN(dumpdata, 64)

char tempName[NR_FULLPLOTS+NR_SUBPLOTS+NR_BOOLPLOTS+NR_BOOLPLOTS_2][NAME_LENGTH];
struct {
	int headerInfo[HEADER_SIZE];
	int names[NR_FULLPLOTS+NR_SUBPLOTS+NR_BOOLPLOTS+NR_BOOLPLOTS_2][NAME_LENGTH];
	int bool_plot[DUMP_BUFFER_SIZE];
	int bool_plot_2[DUMP_BUFFER_SIZE];
	int pPlot[NR_SUBPLOTS][DUMP_BUFFER_SIZE];
	int pPlotFullband[NR_FULLPLOTS][SUBPLOT_BUFFER_SIZE];
} dumpdata;

// Size of datadump buffer
int iDumpDataSize;


q15 mixer_out_ave[NUM_CH_MIXER_OUT];
q31 temp_step[NUM_ACTIVE_SUB];
//external variables
extern q31 pq31NLMS_energy[];
extern complex_q31 pcq31RX_fft[];
extern complex_q31 pcq31LS_fft[];
extern complex_q31 pcq31MIC_fft[];
extern complex_q31 pcq31TX_fft[];
extern complex_q31 pcq31MICE_bg_fft[];
extern complex_q31 pcq31MICE_fg_fft[];
extern complex_q31 pcq31AErr_bg_fft[];
extern complex_q31 pcq31AErr_fg_fft[];
extern q31 pq31LS_noi_est[];
extern q31 pq31MIC_noi_est[];

extern q31 pq31MIC_ave[];

// For testign/plotting
extern bool bAECUpdate[];
extern bool bCopyFilter[];
extern bool bResetFilter[];
extern complex_q31 pcq31NLMS_delay[NUM_ACTIVE_SUB][NLMS_FILTER_LENGTH];
extern int uiNLMS_delay_index;

// Estimated Return Loss Enhancement
extern q15 pq15AErle_bg[NUM_ACTIVE_SUB];
extern q15 pq15AErle_fg[NUM_ACTIVE_SUB];
extern q15 pq15AErle_fg_compensation[NUM_ACTIVE_SUB];

extern q31 pq31RX_ave[NUM_ACTIVE_SUB];
extern q31 pq31LS_ave[NUM_ACTIVE_SUB];
extern q31 pq31MIC_ave[NUM_ACTIVE_SUB];
extern q31 pq31TX_ave[NUM_ACTIVE_SUB];

extern q31 pq31MIChat_bg_ave[NUM_ACTIVE_SUB];
extern q31 pq31MIChat_fg_ave[NUM_ACTIVE_SUB];

extern q31 pq31AErr_bg_ave[NUM_ACTIVE_SUB];
extern q31 pq31AErr_fg_ave[NUM_ACTIVE_SUB];

// For ERLE estimation
extern q31 pq31MIC_ave_slow[NUM_ACTIVE_SUB];
extern q31 pq31AErr_bg_ave_slow[NUM_ACTIVE_SUB];
extern q31 pq31AErr_fg_ave_slow[NUM_ACTIVE_SUB];

extern q31 pq31LS_hold_ave[NUM_ACTIVE_SUB];
extern q31 pq31LS_tail_ave[NUM_ACTIVE_SUB];

extern q31 pq31GammaValue_bg[NUM_ACTIVE_SUB];
extern q15 pq15LastValue_bg[NUM_ACTIVE_SUB];
extern q31 pq31GammaValue_fg[NUM_ACTIVE_SUB];
extern q15 pq15LastValue_fg[NUM_ACTIVE_SUB];
extern q31 pq31Filter_div_bg[NUM_ACTIVE_SUB];
extern q31 pq31Filter_div_fg[NUM_ACTIVE_SUB];

extern q31 pq31echoEst[NUM_ACTIVE_SUB];


/*****************************************************************************/
/*
 *	\brief	Records or playback signals through plot buffers
 *
 *  \date	2009-09-04
 */ 
#define DEC_INDEX 32
void dumpdata_post_dump(void){

	int i,j;
	int idx_base;
	int idx;
	static int idx_base_old = 0;
	short temp16;
	static short temp16_hold=0;
	short temp16_2[DEC_INDEX];
	static short temp16_hold_2[DEC_INDEX];


	idx_base = floor((dump_tick)/SUBPLOT_INTERVALL); //current subband index
	//since we only save every SUBPLOT_INTERVALL samples we need to check this
	if(idx_base != idx_base_old) {
		temp16_hold = 0; // Reset boolean hold parameter
		for(i=0; i<DEC_INDEX; i++) {
	#ifdef KEEP_TRUE
			//keep true values.
			temp16_hold_2[i] = 0;
	#else
			//keep false values.
			temp16_hold_2[i] = MAX_16;
	#endif

		}
	}
	idx_base_old = idx_base;

	// *****Fullband - Booleans  ******
	temp16 = 0;
	temp16 += 0 <<  0; //first boolean


	// Keep maximum value of booleans during subplot_interval steps (otherwise we will miss it)
	temp16 |= temp16_hold;
	temp16_hold = temp16;

	// *****Fullband - Signals ****
	dumpdata.pPlotFullband[0][idx_base]=uiNLMS_delay_index;
	dumpdata.pPlotFullband[1][idx_base]=0;
	// ****** Subband plotting  ******
	for(i=0; i<NR_SUBBANDS; i++) {

		j = START_SUBBAND + i*STRIDE_SUBBAND; // Subband
		idx = idx_base*NR_SUBBANDS + i; // Index in dump struct

		dumpdata.headerInfo[8] = idx_base; // Current index

		// Subband booleans
		temp16_2[j] = 0;
		temp16_2[j] += bAECUpdate[j] << 0; //first boolean
		temp16_2[j] += bCopyFilter[j] << 1; //first booleanbAECUpdate
		temp16_2[j] += bResetFilter[j] << 2;


		// Keep maximum value of booleans during subplot_interval steps (otherwise we will miss it)
#ifdef KEEP_TRUE
		//keep true values.
		temp16_2[j] |= temp16_hold_2[j];
#else
		//keep false values.
		temp16_2[j] &= temp16_hold_2[j];
#endif
	
		temp16_hold_2[j] = temp16_2[j];

		dumpdata.bool_plot[idx] = temp16; //we save the fullband bool values in each subband
		dumpdata.bool_plot_2[idx] = temp16_2[j];

		// Mixer Averages
		/*
		if(j<NUM_CH_MIXER_IN){
			dumpdata.pPlot[0][idx] = limiter_Out[j].q15gain;
			dumpdata.pPlot[1][idx] = limiter_Out[j].q15gain_set;
			dumpdata.pPlot[2][idx] = limiter_Out[j].q15LIMIT_CLIP_LVL;
			dumpdata.pPlot[3][idx] = mixer_out_ave[j];
		}else{
			dumpdata.pPlot[0][idx] = 0;
			dumpdata.pPlot[1][idx] = 0;
			dumpdata.pPlot[2][idx] = 0;
			dumpdata.pPlot[3][idx] = 0;
		}
		*/
		dumpdata.pPlot[0][idx] = pq31LS_ave[j];
		dumpdata.pPlot[1][idx] = pq31MIC_ave[j];
		dumpdata.pPlot[2][idx] = pq31MIChat_bg_ave[j];
		dumpdata.pPlot[3][idx] = pq31MIChat_fg_ave[j];
		dumpdata.pPlot[4][idx] = pq31AErr_bg_ave[j];
		dumpdata.pPlot[5][idx] = pq31AErr_fg_ave[j];
		dumpdata.pPlot[6][idx] = pq31MIC_ave[j];
		dumpdata.pPlot[7][idx] = pq31MIC_ave_slow[j];
		dumpdata.pPlot[8][idx] = pq31AErr_bg_ave_slow[j];
		dumpdata.pPlot[9][idx] = pq31AErr_fg_ave_slow[j];
		dumpdata.pPlot[10][idx] = pq31LS_hold_ave[j];
		dumpdata.pPlot[11][idx] = pq31LS_tail_ave[j];
		dumpdata.pPlot[12][idx] = pq15AErle_bg[j];
		dumpdata.pPlot[13][idx] = pq15AErle_fg[j];
		dumpdata.pPlot[14][idx] = pq15AErle_fg_compensation[j];
		dumpdata.pPlot[15][idx] = pq31NLMS_energy[j];
		dumpdata.pPlot[16][idx] = temp_step[j];
		dumpdata.pPlot[17][idx] = pq31LS_noi_est[j];
		dumpdata.pPlot[18][idx] = pq31MIC_noi_est[j];
		dumpdata.pPlot[19][idx] =pq31GammaValue_bg[j];
		dumpdata.pPlot[20][idx] =pq31GammaValue_fg[j];
		dumpdata.pPlot[21][idx] =pq15LastValue_bg[j];
		dumpdata.pPlot[22][idx] =pq31Filter_div_fg[j];
		dumpdata.pPlot[23][idx] =pq31Filter_div_bg[j];
		dumpdata.pPlot[24][idx] = pq31echoEst[j];
	}

	// For debugging / plotting
	if(++dump_tick >= (SUBPLOT_BUFFER_SIZE)*SUBPLOT_INTERVALL)
		dump_tick = 0;
/*
	if(++dump_coefs_tick >=SUBPLOT_BUFFER_SIZE*SUBPLOT_INTERVALL/NR_COEFS_SAVE){
		dump_coefs_tick=0;
		coefs_dump_index++;
		if(coefs_dump_index>=NR_COEFS_SAVE)
			coefs_dump_index=0;
	}*/
}


/*****************************************************************************/
/*
 *	\brief	Adjusts playback volume and adds silence at the end of plot
 *			bufffers
 *
 *  \date	2009-09-09
 */
void dumpdata_dump_init(void){
	int i, j, tmp;
	
	// Size of whole dump buffer
	iDumpDataSize = sizeof(dumpdata);	

	dumpdata.headerInfo[0] = HEADER_SIZE;
	dumpdata.headerInfo[1] = NR_FULLPLOTS;
	dumpdata.headerInfo[2] = NR_SUBPLOTS;
	dumpdata.headerInfo[3] = NAME_LENGTH;
	dumpdata.headerInfo[4] = SUBPLOT_BUFFER_SIZE;
	dumpdata.headerInfo[5] = SUBPLOT_INTERVALL;
	dumpdata.headerInfo[6] = NR_BOOLPLOTS;
	dumpdata.headerInfo[7] = NR_BOOLPLOTS_2;
	dumpdata.headerInfo[8] = 0;//Current index
	dumpdata.headerInfo[9] = NR_SUBBANDS;
	dumpdata.headerInfo[10] = START_SUBBAND;

	for(i=0; i<NR_FULLPLOTS+NR_SUBPLOTS+NR_BOOLPLOTS+NR_BOOLPLOTS_2; i++) {

		for(j=0; j<NAME_LENGTH; j++) {
			tempName[i][j] = 0;
		}
	}

	// Bool plots (fullband)
	tmp = 0;
	strcpy(tempName[tmp+0], "EMPTY");


	// Bool plots (subband)
	tmp += NR_BOOLPLOTS; 
	strcpy(tempName[tmp+0],  "bAecUpdate");
	strcpy(tempName[tmp+1], "bCopy");
	strcpy(tempName[tmp+2],  "bReset");


	// "Signal" subplots
	tmp += NR_BOOLPLOTS_2;

	// Normal averages
	strcpy(tempName[tmp+0], "LSAve");
	strcpy(tempName[tmp+1], "MicAve");
	strcpy(tempName[tmp+2], "MicHat-bg-ave");
	strcpy(tempName[tmp+3], "MicHat-fg-ave");
	strcpy(tempName[tmp+4], "AErr-bg-ave");
	strcpy(tempName[tmp+5], "AErr-fg-ave");
	strcpy(tempName[tmp+6], "MicAve");
	strcpy(tempName[tmp+7], "MicAveSlow");
	strcpy(tempName[tmp+8], "AErr-bg-ave-slow");

	strcpy(tempName[tmp+9], "AErr-fg-ave-slow");
	strcpy(tempName[tmp+10], "LS-hold-ave");
	strcpy(tempName[tmp+11], "LStail-ave");
	strcpy(tempName[tmp+12], "AerleBG");
	strcpy(tempName[tmp+13], "AerleFG");
	strcpy(tempName[tmp+14], "AerleFG-Comp");
	strcpy(tempName[tmp+15], "NLMSEnergy");
	strcpy(tempName[tmp+16], "tempStep");
	strcpy(tempName[tmp+17], "LS-noi-est");
	strcpy(tempName[tmp+18], "MIC-noi-est");
	strcpy(tempName[tmp+19], "GammaValueBg");
	strcpy(tempName[tmp+20], "GammaValueBg");
	strcpy(tempName[tmp+21], "LastValueBg");
	strcpy(tempName[tmp+22], "FilterDivFg");
	strcpy(tempName[tmp+23], "FilterDivBg");
	strcpy(tempName[tmp+24], "EchoEst");


	// "Signal" Fullband
	tmp += NR_SUBPLOTS;

	strcpy(tempName[tmp+0], "NlmsDelayIndex");
	strcpy(tempName[tmp+1], "EMPTY");

	// Copy only the desired information from temporary buffer
	for(i=0; i<NR_FULLPLOTS+NR_SUBPLOTS+NR_BOOLPLOTS+NR_BOOLPLOTS_2; i++) {

		for(j=0; j<NAME_LENGTH; j++) {
			dumpdata.names[i][j] = (short)tempName[i][j];	
		}
	}
}

#else // USE_DUMPDATA

void dumpdata_post_dump() {}
void dumpdata_dump_init() {}

#endif // USE_DUMPDATA
