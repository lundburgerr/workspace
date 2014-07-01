/*****************************************************************************
 *	\file		plot.c
 *
 *	\date		2013-May-15
 *
 *	\brief		Plot buffering for main application
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
#include "plot.h"
#if USE_DEBUG_PLOT
#include <string.h>
#include <stdio.h>

// ---< Defines >---
// Circular indexing for buffer
//		i		- Current position
//		step	- Desired increase/decrease
//		len		- Buffer length
#define circindex(i, step, len) (((i)+(step)+(len))%(len))

// ---< Global variabels >---
#pragma  DATA_SECTION (plot1, "extvar")
#pragma  DATA_SECTION (plot2, "extvar")
#pragma  DATA_SECTION (plot3, "extvar")
#pragma  DATA_SECTION (plot4, "extvar")
#pragma  DATA_SECTION (plot5, "extvar")
#pragma  DATA_SECTION (plot6, "extvar")
#pragma  DATA_SECTION (plot7, "extvar")
#pragma  DATA_SECTION (plot8, "extvar")
#pragma  DATA_SECTION (plot9, "extvar")
#pragma  DATA_SECTION (plot10, "extvar")
#pragma  DATA_SECTION (plot11, "extvar")
#pragma  DATA_SECTION (plot12, "extvar")
#pragma  DATA_SECTION (plotVar1, "extvar")
#pragma  DATA_SECTION (plotVar2, "extvar")
#pragma  DATA_SECTION (plotVar3, "extvar")
#pragma  DATA_SECTION (psTemp, "extvar")

#pragma  DATA_ALIGN(plot1, 64)
#pragma  DATA_ALIGN(plot2, 64)
#pragma  DATA_ALIGN(plot3, 64)
#pragma  DATA_ALIGN(plot4, 64)
#pragma  DATA_ALIGN(plot5, 64)
#pragma  DATA_ALIGN(plot6, 64)
#pragma  DATA_ALIGN(plot7, 64)
#pragma  DATA_ALIGN(plot8, 64)
#pragma  DATA_ALIGN(plot9, 64)
#pragma  DATA_ALIGN(plot10, 64)
#pragma  DATA_ALIGN(plot11, 64)
#pragma  DATA_ALIGN(plot12, 64)
#pragma  DATA_ALIGN(plotVar1, 64)
#pragma  DATA_ALIGN(plotVar2, 64)
#pragma  DATA_ALIGN(plotVar3, 64)
#pragma  DATA_ALIGN(psTemp, 64)

// Plot buffer and index
plot_t plot1;
plot_t plot2;
plot_t plot3;
plot_t plot4;
plot_t plot5;
plot_t plot6;
plot_t plot7;
plot_t plot8;
plot_t plot9;
plot_t plot10;
plot_t plot11;
plot_t plot12;
plot_t plotVar1;
plot_t plotVar2;
plot_t plotVar3;
short psTemp[TV_PLOT_LENGTH];


// ---< Function prototypes >---

// ---< Functions >---
/*****************************************************************************/
/*	\brief		Initialize plot buffers
 *	\parameters	None
 *	\return		None
 */
void plot_init() {
	// Circular buffer indices
	plot1.uiTick = 0;
	plot2.uiTick = 0;
	plot3.uiTick = 0;
	plot4.uiTick = 0;
	plot5.uiTick = 0;
	plot6.uiTick = 0;
	plot7.uiTick = 0;
	plot8.uiTick = 0;
	plot9.uiTick = 0;
	plot10.uiTick = 0;
	plot11.uiTick = 0;
	plot12.uiTick = 0;
	plotVar1.uiTick = 0;
	plotVar2.uiTick = 0;
	plotVar3.uiTick = 0;
	
	// Clear buffers
	memset(plot1.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot2.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot3.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot4.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot5.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot6.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot7.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot8.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot9.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot10.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot11.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plot12.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plotVar1.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plotVar2.psData, 0, TV_PLOT_LENGTH*sizeof(short));
	memset(plotVar3.psData, 0, TV_PLOT_LENGTH*sizeof(short));
}

/*****************************************************************************/
/*
 *	\brief		Put data into circular plot buffer
 *	\parameters	*x		- Pointer to new data
 * 				*p		- Pointer to plot buffer
 * 				N		- Length of new data
 *	\return		None
 *	\date		2010-03-05
 *	\author		Markus Borgh
 */
void write_plot_data(const short *x, plot_t *p, const unsigned int N) {
	unsigned int i;
	for(i=0; i<N; i++) {
		p->psData[p->uiTick] = x[i];
		p->uiTick = circindex(p->uiTick, 1, TV_PLOT_LENGTH);
	}
}


/*****************************************************************************/
/*
 *	\brief		Get data from circular plot buffer
 *	\parameters	*x		- Pointer to output data
 * 				*p		- Pointer to plot buffer
 * 				N		- Length of data to be read
 *	\return		None
 *	\date		2010-03-05
 *	\author		Markus Borgh
 */
void read_plot_data(short *x, plot_t *p, const unsigned int N) {
	unsigned int i;
	for(i=0; i<N; i++) {
		x[i] = p->psData[p->uiTick];
		p->uiTick = circindex(p->uiTick, 1, TV_PLOT_LENGTH);
	}
}


/*****************************************************************************/
/*	\brief		Writes plot buffer to file
 *	\parameters	filename[]	- File name for data dumping file
 * 				*plotX		- Plot buffer to read from
 *	\return		None
 */
void write_plot_buffer_to_file(char filename[20], plot_t *plotX) {
	FILE *fp;
	unsigned int ui;
	
	printf("Writing plot data to file %s\n", filename);
	fp = fopen(filename, "w"); // Open file for dumping
	
	// Read data from plot buffer
	read_plot_data(psTemp, plotX, TV_PLOT_LENGTH);
	
	// Data is in the order of oldest sample first since the circular buffer index is increased every write
	for(ui=0; ui<TV_PLOT_LENGTH; ui++) {
		fprintf(fp, "%d\n", psTemp[ui]);
	}
	fclose(fp);
}


/*****************************************************************************/
/*	\brief		Appends plot buffer to file
 *	\parameters	filename[]	- File name for data dumping file
 * 				*plotX		- Plot buffer to read from
 *	\return		None
 */
void append_plot_buffer_to_file(char filename[20], plot_t *plotX) {
	FILE *fp;
	unsigned int ui;
	short psTemp[TV_PLOT_LENGTH];

	printf("Appending plot data to file %s\n", filename);
	fp = fopen(filename, "a"); // Open file for dumping

	// Read data from plot buffer
	read_plot_data(psTemp, plotX, TV_PLOT_LENGTH);

	// Data is in the order of oldest sample first since the circular buffer index is increased every write
	for(ui=0; ui<TV_PLOT_LENGTH; ui++) {
		fprintf(fp, "%d\n", psTemp[ui]);
	}
	fclose(fp);
}


/*****************************************************************************/
/*	\brief		Read plot buffer from file
 *	\parameters	filename[]	- File name for data reading file
 * 				*plotX		- Plot buffer to write to
 *	\return		None
 */
/*void read_plot_buffer_from_file(char filename[20], plot_t *plotX) {
	FILE *fp;
	unsigned int ui;
	short psTemp[TV_PLOT_LENGTH];
	
	fp = fopen(filename, "r"); // Open file for dumping
	
	// Data is in the order of oldest sample first since the circular buffer index is increased every write
	for(ui=0; ui<TV_PLOT_LENGTH; ui++) {
		// TODO: Fix this using fscanf??
		//fprintf(fp, "%d\n", psTemp[ui]);
	}
	// Write data to plot buffer
	write_plot_data(psTemp, plotX, TV_PLOT_LENGTH);
	
	fclose(fp);
}*/

#endif // USE_DEBUG_PLOT
