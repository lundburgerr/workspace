/*****************************************************************************
 *	\file		timers.h
 *
 *	\date		2013-Aug-27
 *
 *	\brief		Header for time dependent functions.
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef TIMERS_H
#define TIMERS_H

// ---< Include >---

#include "fract_math.h"

// ---< Defines >---

// ---< Composites declaration >---

typedef struct {
	bool bFinished;
	short sDuration;
	short sCurrent;
} timer_handle_t;

typedef enum {
	TOP,
	BOTTOM,
	RUNNING
} fade_hande_state_t;

typedef struct {
	short sCurrent;
	fade_hande_state_t eFinished;
} fade_handle_t;

// ---< Function prototypes >---

void resetTimerHandle(timer_handle_t* handle, short duration);
bool isFinished(timer_handle_t* handle);

void resetFadeHandle(fade_handle_t* handle, bool high);
bool fade(bool rising, short* buf, fade_handle_t* handle);

// ---< External variables >---

#endif // TIMERS_H
