/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : dma.h
* Author            : Roger Vilmunen ÅF
* Description       : Handle DMA
*                   :
********************************************************************************
* Revision History
* 2013-04-09        : First Issue
*******************************************************************************/

#include "edma.h"

void EDMA3Initialize(void);

/*
 * Callback functions to be called when DMA finished
 */
extern void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);

