/*****************************************************************************
 *	\file		timers.c
 *
 *	\date		2013-Aug-27
 *
 *	\brief		Time dependent functions, for example fading.
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
#include "timers.h"
#include "true_voice.h"
#include <string.h>

// ---< Defines >---

#define MUTE_TIME ((short)100) //ms
#define MUTE_STEP ((q15)(MAX_16/(MUTE_TIME*8)))

#define DEMUTE_TIME ((short)10) //ms
#define DEMUTE_STEP ((q15)(MAX_16/(DEMUTE_TIME*8)))

// ---< Global variabels >---

// ---< Local function prototypes >---

// ---< Functions >---

/*****************************************************************************
 *	\brief Resets a timer handle. This must be done before a call to isFinished
 *	\parameters	handle	- The handle to be reseted
 * 				duration- The amount of times isFinished() returns false.
 *	\return		None
 *****************************************************************************/
void resetTimerHandle(timer_handle_t* handle, short duration) {
	handle->bFinished = false;
	handle->sCurrent = 0;
	handle->sDuration = duration;
}

/*****************************************************************************
 *	\brief A timer function. It returns false a number of times, before it start to return true.
 *	\parameters	handle	- A handle that once was reseted with resetTimerHandle()
 *	\return		false the first "duration" times (see resetTimerHandle()), then true
 *****************************************************************************/
bool isFinished(timer_handle_t* handle) {
	if (handle->bFinished)
		return true;

	handle->bFinished = (handle->sCurrent++) == handle->sDuration;

	return handle->bFinished;
}

//TODO fixa kommentaren
/*****************************************************************************
 *	\brief Resets a fade handle. This must be done before a call to fade()
 *	\parameters	handle	- The handle to be reseted
 * 				high 	- if the algortim should start at maximum/high (no damping) or minimum/low (-inf dB)
 *	\return		None
 *****************************************************************************/
void resetFadeHandle(fade_handle_t* handle, bool high) {
	handle->sCurrent = high ? MAX_16 : 0;
	handle->eFinished = high ? TOP : BOTTOM;
}

/*****************************************************************************
 *	\brief Fade the buffer.
 *	\parameters	rising	- true if the sound level should increase
 *				buf		- The buffer to be faded
 *				handle	- A handle that once was reseted with resetFadeHandle()
 *	\return		true if a extreme (maximum or minimum) is reached
 *****************************************************************************/
bool fade(bool rising, short* buf, fade_handle_t* handle) {
	short i;
	short v = handle->sCurrent;
	if (rising && handle->eFinished != TOP) {
		for (i=0; i<BUFLEN_8KHZ; i++) {
			buf[i] = mult_q15(buf[i],v);
			v += DEMUTE_STEP;
			if (v<0) {
				v = MAX_16;
				handle->eFinished = TOP;
				handle->sCurrent = v;
				return true;
			}
		}
	}
	else if (!rising && handle->eFinished != BOTTOM) {
		for (i=0; i<BUFLEN_8KHZ; i++) {
			buf[i] = mult_q15(buf[i],v);
			v -= MUTE_STEP;
			if (v<0) {
				v = 0;
				memset(&buf[i+1], 0, (BUFLEN_8KHZ-i-1)*sizeof(short));
				handle->eFinished = BOTTOM;
				handle->sCurrent = v;
				return true;
			}
		}
	}
	else if (rising && handle->eFinished == TOP) {
		return true;
	}
	else if (!rising && handle->eFinished == BOTTOM) {
		memset(buf, 0, BUFLEN_8KHZ*sizeof(short));
		return true;
	}


	handle->sCurrent = v;
	handle->eFinished = RUNNING;
	return false;
}
