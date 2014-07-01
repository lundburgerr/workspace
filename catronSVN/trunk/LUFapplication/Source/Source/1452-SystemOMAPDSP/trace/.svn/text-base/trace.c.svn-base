/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : trace.c
* Author            : Roger Vilmunen ÅF
* Description       : Trace functions. Traces is string messages that are stored
*                   : in memory or flushed out over a bus without locking the client.
*                   :
********************************************************************************
* Revision History
* 2013-04-03        : First Issue
*******************************************************************************/

#include <trace/trace.h>
#include <traceMessage.h>
#include <xdc/std.h>
#include <string.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>

#define INITDONE (1)
#define INITNOTDONE (0)
#define INITINPROGRESS (-1)
static int initState = 0;

static MessageQ_QueueId traceQueueId;

char fstr[200]; /* Used by macro */

union maintMessage {
	MessageQ_MsgHeader head;
	struct traceMessage trace;
};

/******************************************************************************
* Function Name    : trace_init
*
******************************************************************************/
int trace_init(void) {

	int status;

	/* Init only once */
	if(initState != INITNOTDONE) return -1;
	initState = INITINPROGRESS;

    /* Open the remote (ARM) message queue. Spin until it is ready. */
    do {
    	Task_sleep(1);
        status = MessageQ_open(TRACE_QUEUE_NAME, &traceQueueId);
    } while (status < 0);

	initState = INITDONE;
	return 0;
}

/******************************************************************************
* Function Name    : trace
* Function Purpose : Create a message and send it to trace queue
* Input parameters : String
******************************************************************************/

void trace(char *str) {
	union maintMessage *msg;
	int status;

	/* Abort if init not done */
	if(initState != INITDONE) return;

	/* Allocate message */
	msg = (union maintMessage *) MessageQ_alloc(0, sizeof(struct traceMessage) + strlen(str));

	/* Set message ID */
	MessageQ_setMsgId(msg, TRACE_MESSAGE_ID);

	/* Copy string into the message */
	strcpy(msg->trace.traceStr, str);

	/* send the message to the remote processor */
	status = MessageQ_put(traceQueueId, (MessageQ_Msg) msg);
	if(status != MessageQ_S_SUCCESS) System_printf("Send msg faild\n");
}

