/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : main.c
* Author            : Roger Vilmunen ÅF
* Description       : Starting up the sys/bios
*                   :
********************************************************************************
* Revision History
* 2013-04-03        : First Issue
*******************************************************************************/

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include "spicom/spicom.h"
#include "trace/trace.h"
#include "maintenance/maintenance.h"
#include "DigitalIO/digitalIO.h"

#include <ti/ipc/HeapBufMP.h>

#include <ti/ipc/Ipc.h>

#define RN_ACTIVE
#ifdef RN_ACTIVE
#include "rn_manager.h"
#endif

void taskFxn(UArg a0, UArg a1);

/*
 *  ======== main ========
 */
Void main()
{
	Int status;
	struct HeapBufMP_Params heapBufMPParams;
	HeapBufMP_Handle heapHandle;

    System_printf("Enter main()\n");
    System_flush();

    /* Call Ipc_start() */
    status = Ipc_start();
    if (status < 0) {
    	System_abort("Ipc_start failed\n");
    	}

#ifdef DSP_ACTIVE
    /* Attach to DSP */
	while (Ipc_attach(1) < 0) {
		Task_sleep(1000);
	};
#endif

#if 1
	/* Create the heap that will be used to allocate messages. */
	HeapBufMP_Params_init(&heapBufMPParams);
	heapBufMPParams.regionId = 0; /* use default region */
	heapBufMPParams.name = "shareHeap";
	heapBufMPParams.align = 4;
	heapBufMPParams.numBlocks = 40;
	heapBufMPParams.blockSize = 256;
	heapBufMPParams.gate = NULL; /* use system gate */
	heapHandle = HeapBufMP_create(&heapBufMPParams);
	if (heapHandle == NULL) {
		System_abort("HeapBufMP_create failed\n");
		while(1);
	}
	/* Register this heap with MessageQ */
	if(MessageQ_registerHeap(heapHandle, 0) != MessageQ_S_SUCCESS) while(1);

#endif

    BIOS_start();     /* enable interrupts and start SYS/BIOS */
}


void maintenanceTask() {

	System_printf("Main Task started\n");
	System_flush();

#if 1
	/* Enable radio SPI */
	spicom_init();
#endif

#if 1
	/* Enable Trace */
	trace_init();
#endif

#if 1
	/* Digital io */
	dio_init();
#endif


#ifdef RN_ACTIVE
	rn_sysbios_init();
	rn_module_init();
	ap_sim_module_init();

	//rn_sysbios_start();

	rn_module_start(1);
	ap_sim_module_start(1);

	rn_module_start(2);
	ap_sim_module_start(2);

#endif

	maintenanceMainTask();
	/* Should never return but in case*/
	Task_exit();
}
