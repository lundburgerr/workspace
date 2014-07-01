/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : maintenance.c
* Author            : Roger Vilmunen ÅF
* Description       : Maintenance functions/misc functions
*                   :
********************************************************************************
* Revision History
* 2013-05-28        : First Issue
*******************************************************************************/

#include <xdc/std.h>

#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>

#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

#include <interrupt.h>

#include <trace/trace.h>
#include <traceMessage.h>

#define HEAP_NAME   "shareHeap"
#define HEAPID      0
#define MAINARMQUEUENAME TRACE_QUEUE_NAME

void testIRQ(void){
	static volatile int i;
	i++;
}

void maintenanceMainTask()
{
    MessageQ_Msg     msg;
    MessageQ_Handle  messageQ;
    MessageQ_QueueId remoteQueueId;
    Int              status;
    HeapBufMP_Handle              heapHandle;

    /* Wait for heap to be created */
	do {
		status = HeapBufMP_open(HEAP_NAME, &heapHandle);
		/*
		 *  Sleep for 1 clock tick to avoid inundating remote processor
		 *  with interrupts if open failed
		 */
		if (status < 0) {
			Task_sleep(1000);
		}
	} while (status < 0);

    /* Register this heap with MessageQ */
    MessageQ_registerHeap((IHeap_Handle)heapHandle, HEAPID);

    /* Create the local message queue */
    messageQ = MessageQ_create("DSPQueue", NULL);
    if (messageQ == NULL) {
        System_abort("MessageQ_create failed\n" );
    }

    /* Open the remote (ARM) message queue. Spin until it is ready. */
    do {
        status = MessageQ_open(MAINARMQUEUENAME, &remoteQueueId);
        /*
         *  Sleep for 1 clock tick to avoid inundating remote processor
         *  with interrupts if open failed
         */
        if (status < 0) {
            Task_sleep(1);
        }
    } while (status < 0);

    /* TEST fix IRQ */
    IntEventMap(15, SYS_INT_EDMA3_0_CC0_INT1);

    /* Init trace */
    trace_init();

    /* Maintenance main loop */
	while (TRUE) {

		/* Get a message */
		status = MessageQ_get(messageQ, &msg, 0);
		if (status >= 0) {
			/* got a message, make a dummy reply to indicate alive */
			/* send the message to the remote processor */
			MessageQ_setMsgId(msg, MessageQ_getMsgId(msg)+1);
			status = MessageQ_put(remoteQueueId, msg);
			if (status < 0) {
				System_abort("MessageQ_put had a failure/error\n");
			}
			TRACE("DSP got a connection test message\r\n");
		}

		/* Do something more in maintenance? */

		/* Slip for a while, 200 tics = 0.2 sec */
		Task_sleep(200);
	}
}
