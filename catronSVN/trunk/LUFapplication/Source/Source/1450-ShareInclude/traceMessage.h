/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : traceMessage.h
* Author            : Roger Vilmunen ÅF
* Description       : Trace message
*                   :
********************************************************************************
* Revision History
* 2013-06-03        : First Issue
*******************************************************************************/

#include <xdc/std.h>
#include <ti/ipc/MessageQ.h>

#define TRACE_QUEUE_NAME "MAINTQ"
#define TRACE_MESSAGE_ID (1)

struct traceMessage {
	MessageQ_MsgHeader head;
	char traceStr[1];
};
