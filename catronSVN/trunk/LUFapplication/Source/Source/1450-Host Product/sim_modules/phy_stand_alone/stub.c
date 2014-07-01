/*
 * stub.c
 *
 *  Created on: 7 mar 2013
 *      Author: ehiblg
 */


#include "rtos_stub.h"
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/knl/Queue.h>
#include <stdlib.h>
//#include "slot_timing.h"
#include "rni.h"
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h>

/************************IPC *****************/
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>
#include "spicom.h"
#include "trace.h"
//comile fix

//typedef uint8_t* RTOS_Mailbo_Handle_T;


extern Mailbox_Handle mailbox0;

MessageQ_QueueId common_q[3];
MessageQ_QueueId send_q;



char MemBuf[10][40];
uint8_t bufIndex=0;

MessageQ_QueueId remoteQueueId=NULL;
MessageQ_Handle messageQStub[2];
MessageQ_Params messageQStubParams[2];

String Stub_MESSAGEQNAME[2]={"stubq1","stubq2"};

void stubInit()
{
uint8_t i;


	send_q = rn_phy_send_queue_get();

	for(i=0;i<2;i++){
		MessageQ_Params_init(&messageQStubParams[i]);

		messageQStub[i] = MessageQ_create(Stub_MESSAGEQNAME[i], &messageQStubParams[i]);
	common_q[i] = MessageQ_getQueueId(messageQStub[i]);

//	Mailbox_create(SizeT msgSize, UInt numMsgs, const Mailbox_Params *params, Error_Block *eb);
	}
}




Void taskStub(UArg a0, UArg a1)
{
	MessageQ_Msg msg;

	 msg_audio_t *RX_data_message_Buf;
	System_printf("enter taskStub()\n");

	Int status;
	Int status2;


	while(1)
	{

		do {
			status=MessageQ_get(messageQStub[0],&msg,0);
			if(status==MessageQ_S_SUCCESS)
			{
				System_printf("Queue 0 event\n");


			}
			status2=MessageQ_get(messageQStub[1],&msg,0);
			if(status2==MessageQ_S_SUCCESS)
			{
				RX_data_message_Buf=(msg_audio_t*)msg;
				System_printf("Queue 1 event\n");
				System_printf("Pos 3 data: %x",RX_data_message_Buf->dataBuf[3]);


			}

		}while((status==MessageQ_S_SUCCESS)||(status2==MessageQ_S_SUCCESS));

		Task_sleep(10);
		Task_yield();
	}


}



MessageQ_QueueId RTOS_QUEUE_CREATE(uint32_t length, uint32_t size )
{
//	static uint8_t i;


return common_q[length];  //fix
}


void RTOS_QUEUE_SEND(MessageQ_QueueId queue,msg_buf_t* data,uint8_t a)  //todo move to slot implemenation
{
Int status;
	status = MessageQ_put(queue, &(data->qHeader));
	if (status < 0) {
			System_abort("MessageQ_put was not successful\n");
			System_flush();
	}

if(data->mt==TCCR_READY_TO_SEND)
{
	static char str[10];
	sprintf(str,"Tx data req \n");
	TRACE(1,str);
}


//	for(i=0;i<length;i++)
//		printf("%x",((data_item_t*)data->data)->dataBuf[i]);


}

msg_buf_t message_Buf;



msg_buf_t * msg_buf_alloc(size_t size)
{
	MessageQ_staticMsgInit(&message_Buf.qHeader, sizeof(msg_buf_t));
return (msg_buf_t *)&message_Buf;


}


uint8_t index=0;


void TRACE(uint8_t level, char str[]) {
	/*static Rec r1;
	 static uint16_t i;*/
Int status;
	/* msg_buf_t *outstr;
	 outstr=	msg_buf_alloc(40);*/

	if (level > 0) {
		System_printf(" %s \n",str);
					System_flush();
	}
	TRACE("ABC \r\n");
	/*	static textbuf_t msg[16];

		if (remoteQueueId == 0) {
			 Open the remote message queue. Spin until it is ready.
			do {
				status = MessageQ_open(CORE0_MESSAGEQNAME, &remoteQueueId);

			} while (status < 0);
		}

		MessageQ_staticMsgInit(&(msg[index&0x0f].msg), sizeof(textbuf_t));

		if (&(msg[index&0x0f].msg) == NULL) {
			System_abort("MessageQ_alloc failed\n");
			System_flush();
		}

		//MessageQ_setMsgId(&(msg.msg),i);//msg.msg, i);
		sprintf(msg[index&0x0f].buf, "%s ", str);


			Mailbox_post(mailbox0, str,0);
		status = MessageQ_put(remoteQueueId, &(msg[index&0x0f].msg));
		if (status < 0) {
			System_abort("MessageQ_put was not successful\n");
			System_flush();
		}
		index++;
	}*/
}


