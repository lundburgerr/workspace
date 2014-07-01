/*****************************************************************************
 *	\file		plot.h
 *
 *	\date		2013-May-15
 *
 *	\brief		Header for main plot buffers
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef TV_PLOT_H
#define TV_PLOT_H

// ---< Include >---

// ---< Defines >---
// Should we have plot buffers in main
#define USE_DEBUG_PLOT 0
#if USE_DEBUG_PLOT
// Length of plot buffers
#define TV_PLOT_LENGTH	11000

// ---< Composites declaration >---
// Plot buffers
typedef struct {
	short psData[TV_PLOT_LENGTH];
	unsigned int uiTick;
} plot_t;

// ---< Function prototypes >---
void plot_init(void);
void write_plot_data(const short *, plot_t *, const unsigned int);
void read_plot_data(short *, plot_t *, const unsigned int);
void write_plot_buffer_to_file(char [20], plot_t *);
void append_plot_buffer_to_file(char [20], plot_t *);

// ---< External variables >---
extern plot_t plot1;
extern plot_t plot2;
extern plot_t plot3;
extern plot_t plot4;
extern plot_t plot5;
extern plot_t plot6;
extern plot_t plot7;
extern plot_t plot8;
extern plot_t plot9;
extern plot_t plot10;
extern plot_t plot11;
extern plot_t plot12;
extern plot_t plotVar1;
extern plot_t plotVar2;
extern plot_t plotVar3;

#endif // USE_DEBUG_PLOT
#endif // TV_PLOT_H

