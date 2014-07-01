/*****************************************************************************
 *	\file		subband.h
 *
 *	\date		2013-Aug-06
 *
 *	\brief		Header for Subband library
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef SUBBAND_H
#define SUBBAND_H

// ---< Include >---
#include "lec.h"

// ---< Defines >---



// ---< Composites declaration >---

// ---< Function prototypes >---

void subband_init();

void subband_analysis(q15[], complex_q31[], q15[], int*);

void subband_synthesis(complex_q31[], q31[], q31[], int*, short);

// ---< External variables >---


#endif // SUBBAND_H
