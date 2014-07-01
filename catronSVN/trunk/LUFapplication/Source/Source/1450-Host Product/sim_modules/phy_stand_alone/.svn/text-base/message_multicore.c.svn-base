/* --COPYRIGHT--,BSD
 * Copyright (c) $(CPYYEAR), Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 *  ======== message_multicore.c ========
 *  Multiprocessor MessageQ example
 *
 *  This is an example program that uses MessageQ to pass a message
 *  from one processor to another.
 *
 *  Each processor creates its own MessageQ first and then will try to open
 *  a remote processor's MessageQ.  
 *
 *  See message_[CORENAME].k file for expected output.
 */

#include <xdc/std.h>
#include <string.h>
//#include "spicom/spicom.h"
#include "trace/trace.h"
/*  -----------------------------------XDC.RUNTIME module Headers    */
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/knl/event.h>
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

#define HEAP_NAME   "myHeapBuf"
#define HEAPID      0
#define NUMLOOPS    10

Char localQueueName[10];
Char nextQueueName[10];
UInt16 nextProcId;

extern startInit();

/*
 *  ======== tsk0_func ========
 *  Allocates a message and ping-pongs the message around the processors.
 *  A local message queue is created and a remote message queue is opened.
 *  Messages are sent to the remote message queue and retrieved from the
 *  local MessageQ.
 */
Void tsk0_func(UArg arg0, UArg arg1)
{
  /*  MessageQ_Msg     msg;*/

	  while(1);
    BIOS_exit(0);
}

/*
 *  ======== main ========
 *  Synchronizes all processors (in Ipc_start) and calls BIOS_start
 */
Int main(Int argc, Char* argv[])
{
    Int status;

    nextProcId = (MultiProc_self() + 1) % MultiProc_getNumProcessors();
    
    /* Generate queue names based on own proc ID and total number of procs */
    System_sprintf(localQueueName, "%s", MultiProc_getName(MultiProc_self()));
    System_sprintf(nextQueueName, "%s",  MultiProc_getName(nextProcId));
    
    /*  
     *  Ipc_start() calls Ipc_attach() to synchronize all remote processors
     *  because 'Ipc.procSync' is set to 'Ipc.ProcSync_ALL' in *.cfg
     */

    //Ipc.ProcSync_NONE;
 status = Ipc_start();
    if (status < 0) {
        System_abort("Ipc_start failed\n");
    }
    stubInit();
    Phy_mem_init();
    BIOS_start();
    while(1);
    return (0);
}



#define testloop 0
void maintenanceTask() {

	System_printf("Maintenance Task started\n");
	System_flush();
#if testloop
	Uint8 stro[7] = {'A','B','C','D','E','F','G'};
	Uint8 stri[10];
	int cnt = 0;
#endif


#if 1
	/* Enable radio SPI */
	spicom_init();
#endif

#if 1
	trace_init();
#endif

	TRACE("Maintenance Task Started\r\n");
	while(1) {
#if testloop
		cnt++;
		System_printf("Task client: SPI Write in 2 sec\n");
		System_flush();
		TRACE1("Maintenance Task loop:%d\r\n", cnt);
#endif
		Task_sleep(2000);
#if testloop
		radio_exchangeData(5,(UInt8 *)stro, (UInt8 *)stri, 0);
		TRACE5("Radiodata 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", stri[0],stri[1],stri[2],stri[3],stri[4]);
		System_printf("Task client: SPI Write Done\n");
		System_flush();
#endif
		Task_sleep(2000);
	}
}
