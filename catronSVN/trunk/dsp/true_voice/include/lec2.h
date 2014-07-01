/*****************************************************************************
 *	\file		lec2.h
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
#ifndef LEC2_H
#define LEC2_H

// ---< Include >---

#include "lec_general.h"

// ---< Defines >---

// ---< Composites declaration >---

// ---< Function prototypes >---

void internal_lec_2(q15* line_in, q15* line_out, q15* line_in_processed);
void internal_lec_2_init_2(bool reset_all);

// ---< External variables >---


#endif // LEC2_H
