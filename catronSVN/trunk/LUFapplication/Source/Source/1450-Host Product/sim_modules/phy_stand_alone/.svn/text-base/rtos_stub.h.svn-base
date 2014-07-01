/*
 * rtos_stub.h
 *
 *  Created on: 7 mar 2013
 *      Author: ehiblg
 */

#ifndef RTOS_STUB_H_
#define RTOS_STUB_H_




#include "rtos_stub.h"
#include "rni.h"
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>

//extern String CORE0_MESSAGEQNAME;

typedef struct textbuf
{
MessageQ_MsgHeader msg;
char buf[15];
}textbuf_t;



void stubInit();

//typedef uint8_t* RTOS_Mailbo_Handle_T;

//MessageQ_QueueId RTOS_QUEUE_CREATE(uint32_t length, uint32_t size );



//void RTOS_QUEUE_SEND(MessageQ_QueueId queue, msg_buf_t* data,uint8_t a);

#endif /* RTOS_STUB_H_ */
