/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : main.c
* Author            : Roger Vilmunen ÅF
* Description       : Starting up the sys/bios
*                   :
********************************************************************************
* Revision History
* 2013-05-28        : First Issue
*******************************************************************************/


#include <xdc/std.h>
#include <string.h>

/*  -----------------------------------XDC.RUNTIME module Headers    */
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>

/*  ----------------------------------- IPC module Headers           */
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>

/*  ----------------------------------- BIOS6 module Headers         */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/*  ----------------------------------- To get globals from .cfg Header */
#include <xdc/cfg/global.h>

#include "maintenance/maintenance.h"

#define HEAPID      0
#define NUMLOOPS    10

void maintenanceTask()
{
	/* Some init might be needed here*/

	/* Last thing to do, call maintenancemain */
	maintenanceMainTask();
}

/*
 *  ======== main ========
 *  Synchronizes all processors (in Ipc_start) and calls BIOS_start
 */
Int main(Int argc, Char* argv[])
{
    Int status;
    
    /*  
     *  Ipc_start()
     */
    status = Ipc_start();
    if (status < 0) {
        System_abort("Ipc_start failed\n");
    }

    /* Attach to ARM */
	while (Ipc_attach(0) < 0) {
		Task_sleep(1000);
	}
 
    BIOS_start();

    return (0);
}
