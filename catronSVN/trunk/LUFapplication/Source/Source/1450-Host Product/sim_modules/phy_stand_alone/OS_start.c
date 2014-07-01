
#include <xdc/std.h>
#include <stdlib.h>
#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Timer.h>

#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/IHeap.h>

/************************IPC *****************/
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>

#include "rtos_stub.h"
#include "rn_phy_api.h"
#include "slot_timing.h"
#include "SPIcom/spicom.h"
#include "trace/trace.h"
extern Swi_Handle swi0;
//extern void slotMainThread();

extern void	rx_interupt();
extern Mailbox_Handle mailbox0;


String CORE0_MESSAGEQNAME="qEtt";

Void taskFxn(UArg a0, UArg a1);
void myIsr();
void mySWI();

#define HEAP_NAME1   "myHeapBuf1"
#define MSGSIZE 256
#define HEAPID1 1
char textStrBuf[15];




void startInit() //todo move to stub
{
   /* test code move to stub*/
	uartStart();
	PHY_Init_Phy(false, 3);

	/*uartStart();*/
	//	trace_init();

	slotMainThread();

	rx_interupt();

	slotMainThread();

	rn_phy_register_tccr_message_receiver(RTOS_QUEUE_CREATE(1, 10 )); /*length fixed to buff*/
	rn_phy_register_uct_message_receiver(RTOS_QUEUE_CREATE(1, 10 )); /*length fixed to buff*/
	rn_phy_register_uct_message_sender(RTOS_QUEUE_CREATE(0, 10 )); /*length fixed to buff*/
	rn_phy_activate_slot(1,PHY_SLOT_DCC_RECEIVE);


	rn_phy_activate_slot(4, PHY_SLOT_UC_RECEIVE);


	rn_phy_activate_slot(5, PHY_SLOT_UC_RECEIVE);

	rn_phy_activate_slot(7, PHY_SLOT_UC_SEND);

//    Mytimer=Timer_create(0, myIsr, &timerParams,NULL);

}


MessageQ_Handle messageQ;
MessageQ_Params messageQParams;



//MessageQ_QueueId remoteQueueId=NULL;

HeapBufMP_Handle              heapHandle;
HeapBufMP_Params              heapBufParams;


Void taskFxn(UArg a0, UArg a1)
{

	MessageQ_Msg msgPt;
	System_printf("enter taskFxn()\n");

	Int status;
	MessageQ_Params_init(&messageQParams);

	messageQ = MessageQ_create(CORE0_MESSAGEQNAME, &messageQParams);
	HeapBufMP_Params_init(&heapBufParams);
	heapBufParams.regionId       = 0;
	heapBufParams.name           = HEAP_NAME1;
	heapBufParams.numBlocks      = 1;
	heapBufParams.blockSize      = sizeof(MessageQ_MsgHeader);
	heapHandle = HeapBufMP_create(&heapBufParams);
	if (heapHandle == NULL) {
		System_abort("HeapBufMP_create failed\n" );
	}


	 MessageQ_registerHeap( (IHeap_Handle)heapHandle, HEAPID1);



	while(1)
	{

		do {
			status=MessageQ_get(messageQ,&msgPt,0);
			if(status==MessageQ_S_SUCCESS)
			{

			//	msgId=MessageQ_getMsgId(msgPt);

				System_printf(" %s \n",(char*)(((textbuf_t*)msgPt)->buf));
				System_flush();
			}
			//   	Memory_free(NULL, tp, 40);
		}while(status==MessageQ_S_SUCCESS);

		Task_sleep(10);
		Task_yield();
	}
	// System_printf("exit taskFxn()\n");

}


extern tx_interupt();

void SendTxTimerIsr()
{
	tx_interupt();
}






void myIsr()
{

	//System_printf("timer \n");

	Swi_post(swi0);



}


Bool firstTime=true;
extern Event_Handle radioSyncEvent;

void mySWI()
{


if(firstTime==true)
{
	firstTime=false;
	Event_post(radioSyncEvent, Event_Id_01);

}
else
{
	Event_post(radioSyncEvent, Event_Id_00);
}
}

