/******************** Copyright (c) 2013 Catron Elektronik AB ******************
 * Filename          : rn_phy_slottiming.c
 * Author            : Björn Lindberg
 * Description       : Timing sequence for the slots
 ********************************************************************************
 * Revision History
 * 2013-03-21        : First Issue
 *******************************************************************************/

/*-----------------------------------------------------------
 Standard includes
 -------------------------------------------------------------
 */

#include <stdint.h>

#include "rtos.h"
//#include "rtos_stub.h"

/*
 -----------------------------------------------------------
 Macros
 -------------------------------------------------------------
 */
#define DELF 1
#define TX_WAKEFROMSLEEP_TIME 2720
#define RX_WAKEFROMSLEEP_TIME 2826
#define TX_WAKEFROMIDLE_TIME 320
#define RX_WAKEFROMIDLE_TIME 426

#define LAST_STATE_TX  1000*DELF
#define DELTA_SLOT 1280*DELF
#define TXDIF 100
#define RXBUFLENGTH 96
#define PHY_TX_QUEUE_LENGTH 10
#define FRAME_DETECT_WIN 200*DELF
#define FRAMECYLE_TIME	20480*DELF//DELTA_SLOT*(MAXNUMBEROFSLOTS+1)

#define MASK_TX 0x04
#define MASK_RX 0x02

//#define NULL 0
#define SLEEPLIMIT 5

/*
 -----------------------------------------------------------
 RTOS includes
 -------------------------------------------------------------
 */

#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>

#include <ti/sysbios/knl/Mailbox.h>

#include <ti/sysbios/hal/Timer.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/sysbios/knl/event.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
/*-----------------------------------------------------------
 Project includes
 -------------------------------------------------------------
 */
#include "adf7242.h"
#include "spi_driver.h"
#include "slot_timing.h"
#include "rn_phy.h"
#include "rni.h"
#include "trace.h"

/*
 -----------------------------------------------------------
 Static variables
 -------------------------------------------------------------
 */
enum source {
	irq = 0, timer
};

#define HEAP_NAME   "myHeapBuf"
#define HEAPID      0

Char TxQueueName[10] = "TXQueue";
bool slotTiming=true;

//static msg_buf_t TX_req_message_Buf;
//static msg_audio_t RX_data_message_Buf;

static MessageQ_QueueId tccr_message_receiver;
static MessageQ_QueueId tccr_message_sender;
static MessageQ_QueueId uct_message_receiver;
static MessageQ_QueueId uct_message_sender;
static MessageQ_QueueId plc_message_reciver;
static uint8_t newFrameStartSlave(uint8_t irq);
uint8_t dbgTrace=0;

//static Mailbox_Handle rn_phy_TX_Queue;  /*Todo change to IPC Mailbox_Handle = MessageQ_QueueId*/
static MessageQ_Handle rn_phy_TX_Queue;

static uint8_t rerun=0;

uint32_t recivePktCount=0;
uint32_t sendPktCount=0;
uint32_t loadPktCount=0;
uint32_t frameCount=0;
uint32_t spiUnavalible=0;
uint32_t reciveSlotCount=0;
uint32_t currentFrameTimeOffset;
slot_t slot_table[MAXNUMBEROFSLOTS];

states_t PHY_state = RN_UNDIFINED;
uint8_t PYH_currentSlot = 0;
uint8_t PYH_nextSlot = 0;
uint8_t PYH_nextTxSlot=0;
powerSaveStates_t PHY_radistate = PS_RX_FROMIDLE;  //slave
uint8_t lastTx;

static uint8_t dummyTxSeq[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0e,0x0f,
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1e,0x1f};


Event_Handle radioSyncEvent;


bool newframe = false;
bool isMaster = false;
/*
 -----------------------------------------------------------
 Static function declarations
 -------------------------------------------------------------
 */

void markeNewFrame(uint8_t source);
uint8_t getNextSlot(uint8_t currentslot);
static void load_timer(uint32_t interval);
powerSaveStates_t calcnextstate(void);
void powerSaveFST(void);
static int32_t getTXint(void);
static uint8_t rxReadout(void);
static uint8_t txLoad();

extern void genToggle(uint8_t channel);
extern void genMark(uint8_t channel);
static uint8_t setIdle(void);
static uint8_t setRR(void);

static int8_t waitRadio(uint8_t mask,bool msg);
static uint8_t setRXReady(void);


static uint8_t setSleep(void);

static bool verifyDataRecived(void);
static void createHeader(msg_buf_t *BxReq,bool empty);
static void load_delta_timer(uint32_t interval);
static void inline printState(void){
	char *PowerState[]={
		"PS_SLEEP",
		"PS_IDLE",
		"PS_RX_READOUT",
		"PS_TX_DELAY",
		"PS_TX_FROMSLEEP",
		"PS_RX_FROMSLEEP",
		"PS_TX_FROMIDLE",
		"PS_RX_FROMIDLE",
		"PS_TX",
		"PS_RX",
		"PS_NEWFRAME",
		"PS_NEWFRAME_PREP",
		"PS_FRAME_DETECT_WIN"};
	TRACE1("PHY state: %s \n\r", PowerState[PHY_radistate]);
}

/*-----------------------------------------------------------
 Command declarations
 -------------------------------------------------------------*/

void Phy_mem_init()
{
	static MessageQ_Params 		rn_phy_TX_Queue_params;
/*	HeapBufMP_Handle heapHandle;
	HeapBufMP_Params heapBufParams;

	HeapBufMP_Params_init(&heapBufParams);
	heapBufParams.regionId = 0;
	heapBufParams.name = HEAP_NAME;
	heapBufParams.numBlocks =5;
	heapBufParams.blockSize = sizeof(msg_buf_t)+100;
	heapHandle = HeapBufMP_create(&heapBufParams);
	if (heapHandle == NULL) {
		System_abort("HeapBufMP_create failed\n");
	}
	/*      Register this heap with MessageQ
/	MessageQ_registerHeap((IHeap_Handle) heapHandle, HEAPID);

	/*    Create the local message queue*/
	MessageQ_Params_init(&rn_phy_TX_Queue_params);
	rn_phy_TX_Queue = MessageQ_create(TxQueueName, &rn_phy_TX_Queue_params);
	if (rn_phy_TX_Queue == NULL) {
		System_abort("MessageQ_create failed\n");
	}


}





void PHY_Init_Phy(bool isMaster, uint8_t rnId) {
	uint8_t i;


	//	init_AD_generic()

	/*tx message q*/
	/*  Create the heap that will be used to allocate messages.*/




	SPI_SetRegister(REG_IRQ1_EN1, ~MASK_RX);	//add mask
	SPI_SetRegister(REG_IRQ1_SRC1, 0xFF);	//clear interrupt source
//	PHY_state = SLAVE_SEARCH;

	for (i = 0; i < MAXNUMBEROFSLOTS; i++) {
		slot_table[i].slotPos = i;
		slot_table[i].listenTo = false;
		slot_table[i].registred = false;
		slot_table[i].destbuf = NULL;
	}
	Timer_stop(FrameTimer);
	Timer_setPeriodMicroSecs(FrameTimer, FRAMECYLE_TIME); //Set frame timer to make it possible to test with slow speed


}

extern const ti_sysbios_hal_Timer_Handle  TXTimer;
//extern void startInit();


void radioTskFnc(UArg a0, UArg a1)
{
	int32_t TxInterval;
	//Error_Block eb;
	//Error_init(&eb);
	UInt events;
	/* Default instance configuration params */
	radioSyncEvent = Event_create(NULL, NULL);
	if (radioSyncEvent == NULL) {
		System_abort("Event create failed");
	}


	while(1)
	{
		events = Event_pend(radioSyncEvent,Event_Id_NONE, Event_Id_00+Event_Id_01+Event_Id_02+Event_Id_03,
				BIOS_WAIT_FOREVER);

		if (events & Event_Id_00) {
			//			genMark(0); //debug only instruments the code and adds delay!
			slotMainThread();
			if(rerun==1)
			{
				rerun=0;
				slotMainThread();
			}
		}
		if (events & Event_Id_01) {
			//	startInit();
		}
		if (events & Event_Id_02) {
			//restart from
#ifdef radio
			sendCmd(CMD_RC_TX);
#endif
			sendPktCount++;
			if((PHY_radistate!=PS_SLEEP)||(PHY_radistate!=PS_IDLE)||(PHY_radistate!=PS_NEWFRAME))
			{
				//abort
			}
			PHY_radistate=PS_NEWFRAME;
			PYH_nextSlot=0;  ////
			PYH_currentSlot=0;
			TxInterval = getTXint();
						if(TxInterval>0)
						{
							Timer_setPeriodMicroSecs(TXTimer, TxInterval);
							Timer_start(TXTimer);
							if(dbgTrace)
										{
											TRACE1("TX interval %ld\r\n",TxInterval);
										}

						}

			slotMainThread();
		}

		if (events & Event_Id_03) {
			if(dbgTrace)
					{
					TRACE("Radio send event\r \n ");
					}
#ifdef radio
			sendCmd(CMD_RC_TX);
#endif
		}
		//		Task_sleep(0);
		//		Task_yield();
	}


}


void slotMainThread() {

	//	uint32_t nextSuspendTime;  // todo put time to default one slot

	if (!((PHY_radistate == PS_RX_READOUT)||(PHY_radistate == PS_FRAME_DETECT_WIN)||(PHY_radistate == PS_TX_DELAY)||
			(PHY_radistate ==PS_NEWFRAME_PREP))) {
		PYH_currentSlot = PYH_nextSlot;
	}
	switch (PHY_state) {
	case RN_UNDIFINED:
	case MASTER_NO_REG:
		break;

	case SLAVE_UNREGISTER:
	case SLAVE_SEARCH:
		break;
	case SLAVE_RNFOUND:
		if (PYH_currentSlot > 2u) {

		} else if (PYH_currentSlot == 0u) {
			//Prepare to send acc
			//read
			PHY_state = SLAVE_REGISTERED; //todo invest if mac need to evaluate DCC to see if slave registered. this shall be done on a message from mac

		}

		break;
	case MASTER_RUNNIG:
		//fall through
	case SLAVE_REGISTERED:
	 switch( PHY_radistate){
	 	 case PS_NEWFRAME:
	 		 //wakeup radio enable IRQ
	 		break;
	 	case PS_NEWFRAME_PREP:

	 	case PS_TX_FROMSLEEP:
	 		//	Wakeup chip
	 		setIdle();
	 		//fall through
	 	case PS_TX_FROMIDLE:
		 		//	radio ready chip
	 		setRR();
	 		//fall through
	 	case PS_TX:
	 		txLoad();//load TX
	 		//radio
	 		break;
	 	case  PS_RX_FROMSLEEP:
	 		setIdle();
	 		//	Wakeup chip
	 		//fall through
	 	case  PS_RX_FROMIDLE:
	 		setRR();
	 		//fall through
	 	case PS_RX:
	 		setRXReady();
	 		break;
	 	case PS_RX_READOUT:
	 		 rxReadout();
	 		break;
	 	case PS_FRAME_DETECT_WIN:
	 		newFrameStartSlave(0);
	 		break;
	 }
/*
		//get rssi
		slot_table[PYH_currentSlot].lastRSSI = result; ////Write rssi
		*/
		PYH_nextSlot = getNextSlot(PYH_currentSlot); //find next slot to read/send;

		powerSaveFST();

		break;

	}

}

/****************RN functions************/

phy_result_t rn_phy_activate_slot(uint8_t slot, phy_slot_conf_t slot_conf) {
	phy_result_t retVal = PHY_OK;
	slot_table[slot-1].slotType = slot_conf;
	switch (slot_conf) {
	case PHY_SLOT_INACTIVE:      // Slot Not configured
		break;
	case PHY_SLOT_DCC_SEND:      // Slot activated for DCC send
		PHY_registerSlotlistener(slot, true, true, tccr_message_receiver);
		break;
	case PHY_SLOT_DCC_RECEIVE:   // Slot activated for DCC receive
		PHY_registerSlotlistener(slot, true, false, tccr_message_receiver);
		break;
	case PHY_SLOT_UCC_SEND:      // Slot activated for UCC send
		PHY_registerSlotlistener(slot, true, true, tccr_message_sender);
		break;
	case PHY_SLOT_UCC_RECEIVE:   // Slot activated for UCC receive
		PHY_registerSlotlistener(slot, true, false, tccr_message_receiver);
		break;
	case PHY_SLOT_UC_SEND:      // Slot activated for UC send
		PHY_registerSlotlistener(slot, true, true, uct_message_sender);
		break;
	case PHY_SLOT_UC_RECEIVE:    // Slot activated for UC receive
		PHY_registerSlotlistener(slot, true, false, uct_message_receiver);
		break;
	default:
		//shall not get here
		retVal = PHY_FAIL;
		break;
	}
	return retVal;
}

phy_result_t rn_phy_register_uct_message_sender(MessageQ_QueueId queue_p) {
	phy_result_t retVal = PHY_FAIL;
	uct_message_sender = queue_p;
	if (queue_p != 0) {
		retVal = PHY_OK;

	}
	return retVal;

}

phy_result_t rn_phy_register_uct_message_receiver(MessageQ_QueueId queue_p) {
	phy_result_t retVal = PHY_FAIL;
	uct_message_receiver = queue_p;
	if (queue_p != 0) {
		retVal = PHY_OK;

	}
	return retVal;

}

phy_result_t rn_phy_register_tccr_message_receiver(MessageQ_QueueId queue_p) {

	phy_result_t retVal = PHY_FAIL;
	tccr_message_receiver = queue_p;
	if (queue_p != 0) {
		retVal = PHY_OK;

	}
	return retVal;

}

phy_result_t rn_phy_register_tccr_message_sender(MessageQ_QueueId queue_p) {

	phy_result_t retVal = PHY_FAIL;
	tccr_message_sender = queue_p;
	if (queue_p != 0) {
		retVal = PHY_OK;

	}
	return retVal;

}

phy_result_t rn_phy_register_plc_receiver(MessageQ_QueueId queue_p) {

	phy_result_t retVal = PHY_FAIL;
	plc_message_reciver = queue_p;
	if (queue_p != 0) {
		retVal = PHY_OK;

	}
	return retVal;
}

void PHY_registerSlotlistener(uint8_t pos, bool active, bool sendSlot,
		MessageQ_QueueId queID)
{
	uint8_t tablepos=pos-1;
	slot_table[tablepos].destbuf = queID;

	slot_table[tablepos].listenTo = true;
	slot_table[tablepos].registred = true;
	slot_table[tablepos].registred = true;
	slot_table[tablepos].SendSlot = sendSlot;

}

void PHY_unRegisterSlotlistener(uint8_t pos)
{
	uint8_t tablepos=pos-1;
	slot_table[tablepos].destbuf = NULL;
	slot_table[tablepos].listenTo = false;
	slot_table[tablepos].registred = false;
}

MessageQ_QueueId rn_phy_send_queue_get() {

	return MessageQ_getQueueId(rn_phy_TX_Queue);
}

phy_result_t rn_phy_deactivate_slot(uint8_t slot)
{
	PHY_unRegisterSlotlistener(slot);
	return PHY_OK;
}

void    rn_phy_idle_conf(void)
{
	PHY_state = RN_UNDIFINED;
	Timer_stop(FrameTimer);
	Timer_stop(TXTimer);
}

void    rn_phy_master_conf(uint8_t rnid)
{
	PHY_state =MASTER_NO_REG;
	isMaster=true;
	PHY_state =	MASTER_RUNNIG;
	/*Timer_stop(TXTimer);*/
	Timer_start(FrameTimer);

}
void    rn_phy_slave_conf(uint8_t rnid)
{
	PHY_state = SLAVE_UNREGISTER;
	PHY_state=SLAVE_SEARCH;
	setRR();
	setRXReady();
}


void rn_phy_module_init()
{
	Phy_mem_init();
	PHY_Init_Phy(isMaster,0);
}

void rn_phy_module_start(uint8_t phase)
{
	if(isMaster)
	{
	/*/	PHY_state =	MASTER_RUNNIG;
		Timer_stop(TXTimer);
		Timer_start(FrameTimer);*/


	}
	else if(PHY_state==SLAVE_UNREGISTER)
	{
	/*	PHY_state=SLAVE_SEARCH;
		setRR();
		setRXReady();*/
	}
	else
		{

	}
}



/**************************internal functions***************************/

/* brief\  compute next state and calculate timer value*/

void powerSaveFST() {
	powerSaveStates_t nextState;

	switch (PHY_radistate) {
	case PS_SLEEP:
		//set timer
		if (PYH_nextSlot == 0) {
			if(dbgTrace)
			{
			TRACE("Sleep new frame \r \n ");
			}
			if (slot_table[PYH_nextSlot].SendSlot == true) {
				load_timer(TX_WAKEFROMSLEEP_TIME);/*
						(((MAXNUMBEROFSLOTS - PYH_currentSlot) - 1) * DELTA_SLOT)
								- TX_WAKEFROMSLEEP_TIME
								+ (LAST_STATE_TX) * lastTx);*/

			} else {
				load_timer(RX_WAKEFROMSLEEP_TIME);/*
						(((MAXNUMBEROFSLOTS - PYH_currentSlot) - 1) * DELTA_SLOT)
								- RX_WAKEFROMSLEEP_TIME
								+ (LAST_STATE_TX) * lastTx);*/

			}
			nextState =PS_NEWFRAME_PREP;
			newframe = true;
		} else if (slot_table[PYH_nextSlot].SendSlot == true) {
			//set timer
			if(dbgTrace)
					{
				TRACE("Sleep  \r\n ");
					}
			load_timer(TX_WAKEFROMSLEEP_TIME); /*
					(((PYH_nextSlot - PYH_currentSlot) - 1) * DELTA_SLOT)
							- TX_WAKEFROMSLEEP_TIME + (LAST_STATE_TX) * lastTx);*/
			nextState = PS_TX_FROMSLEEP;

		} else {

			//set timer
			if(dbgTrace)
					{
				TRACE("Sleep  \r\n");
					}
			load_timer(RX_WAKEFROMSLEEP_TIME); /*
					(((PYH_nextSlot - PYH_currentSlot) - 1) * DELTA_SLOT)
							- RX_WAKEFROMSLEEP_TIME + (LAST_STATE_TX) * lastTx);*/
			nextState = PS_RX_FROMSLEEP;

		}
		//sleep radio;
		 setSleep();
		break;
	case PS_IDLE:
		if (PYH_nextSlot == 0) {
			//set timer
			if(dbgTrace)
					{
			TRACE("Idle new frame\r\n");
					}
			if (slot_table[PYH_nextSlot].SendSlot == true) {
				load_timer(TX_WAKEFROMIDLE_TIME); /*
						(((MAXNUMBEROFSLOTS - PYH_currentSlot) - 1) * DELTA_SLOT)
								- TX_WAKEFROMIDLE_TIME
								+ (LAST_STATE_TX) * lastTx);/*/

			} else {
				load_timer(RX_WAKEFROMIDLE_TIME); /*
						(((MAXNUMBEROFSLOTS - PYH_currentSlot) - 1) * DELTA_SLOT)
								- RX_WAKEFROMIDLE_TIME
								+ (LAST_STATE_TX) * lastTx);/*/

			}
			newframe = true;
			nextState = PS_NEWFRAME_PREP;
		} else if (slot_table[PYH_nextSlot].SendSlot == true) {
			//set timer
			if(dbgTrace)
					{
			TRACE("Idle\r\n");
					}
			load_timer(TX_WAKEFROMIDLE_TIME); /*
					(((PYH_nextSlot - PYH_currentSlot) - 1) * DELTA_SLOT)
							- TX_WAKEFROMIDLE_TIME + (LAST_STATE_TX) * lastTx);/*/
			nextState = PS_TX_FROMIDLE;

		} else {

			//set timer
			if(dbgTrace)
					{
			TRACE("Idle\r\n");
					}
			load_timer(RX_WAKEFROMIDLE_TIME); /*
					(((PYH_nextSlot - PYH_currentSlot) - 1) * DELTA_SLOT)
							- RX_WAKEFROMIDLE_TIME + (LAST_STATE_TX) * lastTx);*/
			nextState = PS_RX_FROMIDLE;

		}
		//idle pradio;
		 setIdle();
		break;

	case PS_RX_READOUT:
		if(dbgTrace)
				{
		TRACE("Read out RX\r\n");
				}
		nextState = calcnextstate();

		break;
	case PS_TX_FROMSLEEP:
	case PS_TX_FROMIDLE:
	case PS_TX:
		load_delta_timer(DELTA_SLOT);
		nextState = PS_TX_DELAY;
		break;
	case PS_TX_DELAY:

		nextState = calcnextstate();

		break;
	case PS_RX_FROMSLEEP:
	case PS_RX:
	case PS_RX_FROMIDLE:
		lastTx = 0;
		//start slot timer
		//TRACE("Readout: ");
		load_delta_timer(DELTA_SLOT);
		nextState = PS_RX_READOUT;
		break;
	case PS_NEWFRAME:
		if(isMaster)
		{
			nextState = calcnextstate();
		}
		else
		{
			load_delta_timer(FRAME_DETECT_WIN); //add delta detect win
			nextState = PS_FRAME_DETECT_WIN;
		}
		break;
	case PS_FRAME_DETECT_WIN:
		if(isMaster)
			{
				nextState=PS_TX;
			}
			else
			{
				load_delta_timer(DELTA_SLOT-(FRAME_DETECT_WIN/2));
				nextState = PS_RX_READOUT;
			}


		break;
	case PS_NEWFRAME_PREP:
		nextState = PS_NEWFRAME;
		break;
	}


	PHY_radistate = nextState;
	if(dbgTrace==1)
			{
		printState();
			}
	if ((nextState == PS_IDLE) || (nextState == PS_SLEEP)) {
		powerSaveFST(); //recurrent run
	}

}

powerSaveStates_t calcnextstate(void) {
	powerSaveStates_t nextState;
	if (PYH_nextSlot == 0) {

		if (MAXNUMBEROFSLOTS - PYH_currentSlot < SLEEPLIMIT) {

			nextState = PS_IDLE;
		} else {
			nextState = PS_SLEEP;

		}
	} else if (PYH_nextSlot == PYH_currentSlot + 1) {
		if (slot_table[PYH_nextSlot].SendSlot == true) {
			nextState = PS_TX;
			//set tx
			if (PHY_radistate == PS_RX_READOUT || PHY_radistate == PS_TX_DELAY ) {
			//	load_timer(0);
				rerun=1;
			} else {
				load_delta_timer(DELTA_SLOT - 300);
			}

		} else {
			nextState = PS_RX;
			if (PHY_radistate == PS_RX_READOUT || PHY_radistate == PS_TX_DELAY) {
			//	load_timer(0);
				rerun=1;
			} else {
				load_delta_timer(DELTA_SLOT - 300);
			}
		}

	} else if (PYH_nextSlot - PYH_currentSlot < SLEEPLIMIT) {
		nextState = PS_IDLE;

	}

	else {
		nextState = PS_SLEEP;

	}

	return nextState;

}

/*todo move to Radio specific file                          */



static int32_t getTXint(void) {
	int16_t i=PYH_nextTxSlot+1;
	int8_t n=1;

	while ((i < MAXNUMBEROFSLOTS) && (slot_table[i].SendSlot==false)) {
		i++;
		n++;
	}

	if (i >= MAXNUMBEROFSLOTS) {
		i = -1;
		n=-1;
		PYH_nextTxSlot=0; //works for master
	}
	else
	{
		PYH_nextTxSlot=i;
	}
	return (n+1)*DELTA_SLOT;


}




uint8_t getNextSlot(uint8_t currentslot) {
	uint8_t i;
	i = currentslot + 1u;
	while ((i < MAXNUMBEROFSLOTS) && (slot_table[i].listenTo == false)) {
		i++;
	}
	if (i >= MAXNUMBEROFSLOTS) {
			i = 0u;
		}

	return i;
}

void markeNewFrame(uint8_t source) {
	//Send ready to send
	// save system time make time mark
	genToggle(0);
}

void timer_interupt() {
	/*
	 portBASE_TYPE xYieldRequired;

	 // Resume the suspended task.
	 xYieldRequired = xTaskResumeFromISR( xHandle );

	 if( xYieldRequired == pdTRUE )
	 {
	 // We should switch context so the ISR returns to a different task.
	 // NOTE:  How this is done depends on the port you are using.  Check
	 // the documentation and examples for your port.
	 portYIELD_FROM_ISR();
	 }

	 */
}

void maskInterupt(uint8_t type) {

}

extern const Timer_Handle slotTimer;
extern Event_Handle radioSyncEvent;




/*****************************interrupt contxt no spi call*********************************************/


// new frame triggering  interrupt for detection of new rni
//
//

void rx_interupt()
{

	uint8_t irqReg1 = SPI_GetRegister(REG_IRQ1_SRC0);

	SPI_SetRegister(REG_IRQ1_SRC1, 0xFF);	//clear interrupt source
	if (irqReg1 == MASK_RX) {

		//install timer to wait for message completion
		// or
		/*
		 portBASE_TYPE xYieldRequired;

		 // Resume the suspended task.
		 xYieldRequired = xTaskResumeFromISR( xHandle );
		 */

		//PHY_state = SLAVE_RNFOUND; // ? do we need to wait for upper layer to read response from master
	}
	markeNewFrame(irq);
	newFrameStartSlave(1);
	if(PHY_state==SLAVE_SEARCH)
		{
		PHY_state = SLAVE_RNFOUND;
		}
}

void tx_interupt() {
	int32_t TxInterval;

	Timer_stop(TXTimer);
	TxInterval = getTXint();
	if(TxInterval>0)
	{
		Timer_setPeriodMicroSecs(TXTimer, TxInterval);
		Timer_start(TXTimer);
		if(dbgTrace)
			{
				TRACE1("TX interval %ld\r\n",TxInterval);
			}

	}
	sendPktCount++;
	Event_post(radioSyncEvent, Event_Id_03);
	if(dbgTrace)
	{
		TRACE("TX send cmd\r\n");
	}

}



void frameTimerTimeoutMaster()
{
	if(isMaster)
	{
		currentFrameTimeOffset = Timer_getCount(FrameTimer);
	frameCount++;
	Event_post(radioSyncEvent, Event_Id_02);
//	markeNewFrame(0);
	}
	else
	{
		System_abort("Slave shall not post timing event");
	}
}


//extern tx_interupt();

void SendTxTimerIsr()
{
	Timer_stop(TXTimer);
	tx_interupt();
}


//extern Event_Handle radioSyncEvent;

extern	 const Timer_Handle slotTimer;

void myIsr()
{

	//System_printf("timer \n");
if(slotTiming==true)
	{
	Timer_stop(slotTimer);
	genMark(0);
	//Swi_post(swi0);
	Event_post(radioSyncEvent, Event_Id_00);
	}

}

void radiodelay()
{
	Semaphore_post(radiosem);
	//Event_post(radioCurcuitResponse, Event_Id_03);
}

/**************************************end interup contxt*****************************************/
#define RXDATARECIVE 100


static uint8_t txLoad()
{
msg_buf_t *TxReq;// = TX_req_message_buf;
msg_buf_t *msg_buf;
loadPktCount++;
#ifdef rnirecive
if(dbgTrace)
{
	TRACE("TX load\r\n");
}
TxReq =(msg_buf_t*) MessageQ_alloc(0, sizeof(msg_buf_t));// msg_buf_alloc(1);
//msg_buf =(msg_buf_t*) MessageQ_alloc(0, sizeof(msg_buf_t)+100);  //todo invest if necessery


TxReq->size = TCCR_READY_TO_SEND_SIZE;

createHeader(TxReq,false);
rtos_send(slot_table[PYH_currentSlot].destbuf,TxReq);


MessageQ_get( rn_phy_TX_Queue ,(MessageQ_Msg*)&msg_buf, RXDATARECIVE );

if(msg_buf->size>50)
{
	SPI_TXwrite(msg_buf->data,msg_buf->size);
	MessageQ_free((MessageQ_Msg)msg_buf);
}
else
#endif
{		//Phy standalone use dummy TX seq
	//TRACE("no tx data supply");
#ifdef radio
	SPI_TXwrite(dummyTxSeq,32);
#endif
}
#ifdef rnirecive
//MessageQ_free((MessageQ_Msg)TxReq);

#endif
//if (status < 0) {
//	System_abort("MessageQ_put was not successful\n");
//	System_flush();
//	}
if(dbgTrace)
{
	TRACE("TX load return\r\n");
}
return 0;
}




static uint8_t rxReadout() {

	msg_buf_t *RXReq;

	reciveSlotCount++;


	if (verifyDataRecived()) {
		RXReq = msg_buf_alloc(RXBUFLENGTH+2);
		recivePktCount++;
#ifdef radio
		SPI_RXread(&(RXReq->data[2]), RXBUFLENGTH + 2);

#endif
		createHeader(RXReq,false);


	}
	else
	{
		RXReq = msg_buf_alloc(TCCR_EMPTY_SIZE);
		createHeader(RXReq,true);
	}
	rtos_send(slot_table[PYH_currentSlot].destbuf, RXReq);
	return 0;
}


static void createHeader(msg_buf_t *BxReq,bool empty)
{

switch	(slot_table[PYH_currentSlot].slotType){
case PHY_SLOT_DCC_SEND:
case PHY_SLOT_UCC_SEND:
	BxReq->pd = PD_TCCR;
	BxReq->mt = TCCR_READY_TO_SEND;
	BxReq->size = TCCR_READY_TO_SEND_SIZE;
	break;
case PHY_SLOT_DCC_RECEIVE:
case PHY_SLOT_UCC_RECEIVE:
	BxReq->pd = PD_TCCR;
	if(empty)
	{
	BxReq->mt = TCCR_EMPTY;
	BxReq->size=TCCR_EMPTY_SIZE;
	}
	else
	{
	BxReq->mt = TCCR_DATA;
	}
break;




case PHY_SLOT_UC_SEND:
	BxReq->pd = PD_TCI;
	BxReq->mt = TCI_READY_TO_SEND;
//	BxReq->size = TCI_READY_TO_SEND_SIZE;
	break;
case PHY_SLOT_UC_RECEIVE:
	BxReq->pd = PD_TCI;

	if(empty)
	{
	BxReq->mt = TCI_EMPTY;
//	BxReq->size=TCI_EMPTY_SIZE;
	}
	else
	{
	BxReq->mt = TCI_DATA;
	}
break;

case PHY_SLOT_INACTIVE:

break;

}
BxReq->data[0] = PYH_currentSlot + 1;
BxReq->data[1] = (uint8_t) (0xff & frameCount);

}


#define MAX_ADF_DELAY 5

static uint8_t setIdle(void)
{
#ifdef radio

	while ((getStatus() & 0xa0) != 0xa0) {
		spiUnavalible++;
	}
	sendCmd(CMD_RC_IDLE);

	if (waitRadio(0x01, false) < 0) {
		while ((getStatus() & 0xa0) != 0xa0) {
			spiUnavalible++;
		}
		sendCmd(CMD_RC_IDLE);
		waitRadio(0x01, true);
	}
#endif
	if (dbgTrace) {
		TRACE("Set IDLE \n\r");
	}
	return 0;
}

static uint8_t setRR(void) {
#ifdef radio

	while ((getStatus() & 0xa0) != 0xa0){
		spiUnavalible++;
	}
	sendCmd(CMD_RC_PHY_RDY);
	if (waitRadio(0x03, false) < 0) {
		while ((getStatus() & 0xa0) != 0xa0) {
			spiUnavalible++;
		}
		sendCmd(CMD_RC_PHY_RDY);
		waitRadio(0x03, true);
	}

#endif
	if (dbgTrace) {
			TRACE("Set RADIO READY \n\r");
		}
	return 0;
}

static uint8_t setRXReady(void) {
#ifdef radio

	while ((getStatus() & 0xa0) != 0xa0){

		spiUnavalible++;
	}
	if((getStatus() & 0x0f)!= 0x03){
		setRR(); //needs to be radio ready
	}
	sendCmd(CMD_RC_RX);

	if (waitRadio(0x04, false) < 0) {
		while ((getStatus() & 0xa0) != 0xa0) {
			spiUnavalible++;
		}
		sendCmd(CMD_RC_RX);
		waitRadio(0x04, true);
	}
	SPI_SetRegister(REG_IRQ1_SRC1,0xff); //clears irq
	SPI_SetRegister(REG_IRQ1_SRC0,0xff); //clears irq
#endif
	if (dbgTrace) {
				TRACE("Set RADIO RX \n\r");
			}
	return 0;
}

static int8_t waitRadio(uint8_t mask, bool Msg) {
	char *RADIOState[] = { "UNKNOWN", "IDLE", "UNKNOWN", "RADIO READY",
			"RADIO RX" };
	uint32_t i = 0;
	uint8_t StatusOK = 0;
	int8_t resp = 0;
	uint8_t recivedStatus;

	Timer_start(RadioDelay);
	while (!StatusOK) {
		Semaphore_pend(radiosem, BIOS_WAIT_FOREVER);
		while ((getStatus() & 0xa0) != 0xa0) {
			spiUnavalible++;
		}
		recivedStatus = 0x0f & getStatus();
		if (recivedStatus == mask) {
			StatusOK = 1;
			Timer_stop(RadioDelay);
		} else {
			i++;
			if (i >= MAX_ADF_DELAY) {
				if (Msg) {
					TRACE1("NO RESP WHEN SWITCH TO  %s \n\r ",
							RADIOState[mask]);
					TRACE1("Recived status %s \n\r ",
							RADIOState[recivedStatus]);
				}
				resp = -1;
				break;

			}

		}
	}

	return resp;
}

static uint8_t setSleep(void) {
#ifdef radio
	// sendCmd(CMD_RC_SLEEP);
	while ((getStatus() & 0xa0) != 0xa0) {
		spiUnavalible++;
	}
	sendCmd(CMD_RC_IDLE);
#endif

	return 0;
}




static uint8_t newFrameStartSlave(uint8_t irq)
{
int32_t TxInterval = getTXint();
frameCount++;
		if(TxInterval>0)
			{
			if(!irq)
			{
				TxInterval-=FRAME_DETECT_WIN;

			}
			else
			{
				load_timer(DELTA_SLOT); //add delta detect win
				PHY_radistate = PS_RX_READOUT;
			}
			Timer_setPeriodMicroSecs(TXTimer, TxInterval);
			Timer_start(TXTimer);
			}
		markeNewFrame(0);
		if(dbgTrace)
		{
		TRACE("new frame \r\n");
		}
		//generate TX timer from slot table
		newframe = false;
return 0;
}

static bool verifyDataRecived(void)
{
	bool response=false;
	uint8_t status;
#ifdef radio
	while ((getStatus() & 0xa0) != 0xa0) {
		spiUnavalible++;
	}
	status=getStatus();
	if((status & 0x40) ==0x40)
	{
		//read
		if(SPI_GetRegister(REG_IRQ1_SRC1) &0x02==0x02)
			{
			//if rx recived
			response=true;
			}
		SPI_SetRegister(REG_IRQ1_SRC1,0xff); //clears irq
	}
#endif
	return response;
}


static void load_delta_timer(uint32_t interval)
{
	Timer_setPeriodMicroSecs(slotTimer,interval);
	Timer_start(slotTimer);

}

static void load_timer(uint32_t timeMargin) {

	uint32_t expectedFrameTime;
	uint32_t currentFrameTime;
	uint32_t interval;

	currentFrameTime = (Timer_getCount(FrameTimer)-currentFrameTimeOffset)/212; //todo invest in magic number
	if (PYH_nextSlot > 0) {
		expectedFrameTime = PYH_nextSlot * DELTA_SLOT - timeMargin;
		if (currentFrameTime < expectedFrameTime) {
			interval = expectedFrameTime - currentFrameTime;
		} else {
			TRACE("TimeOut \r\n");
			//todo break
		}



	}
	else
	{
		//TRACE("new frame? \r\n");
		expectedFrameTime = (MAXNUMBEROFSLOTS) * DELTA_SLOT - timeMargin;
			if (currentFrameTime < expectedFrameTime) {
				interval = expectedFrameTime - currentFrameTime;
			} else {
				TRACE("TimeOut \r\n");
				//todo break
			}


	}
	if (dbgTrace == 1) {
				TRACE1("timer start int: %ld  \r\n", interval);
			}
	Timer_setPeriodMicroSecs(slotTimer, interval);
	Timer_start(slotTimer);
}






