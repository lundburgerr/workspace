// Copyright 2013

/*-----------------------------------------------------------
Standard includes
-------------------------------------------------------------
*/

#include <stdint.h>
#include <stdlib.h>

/*
-----------------------------------------------------------
Macros
-------------------------------------------------------------
*/

#define HEAPID (0)

#define RN_MAC_TASK_PRIORITY        ( tskIDLE_PRIORITY + 8 )
#define RN_MANAGER_TASK_PRIORITY    ( tskIDLE_PRIORITY + 6 )
#define RN_LOG_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 )
#define RN_CMD_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 )

#define AP_SIM_TASK_PRIORITY		RN_MAC_TASK_PRIORITY // Simulates TCI

#define RN_STANDARD_STACK_SIZE		(4096)
/*
-----------------------------------------------------------
SysBIOS includes
-------------------------------------------------------------
*/

/*
-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/
#include "rtos.h"
#include "rni.h"
#include "rn_phy.h"
#include "rn_log.h"
#include "rn_mac_cc.h"
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
static uint32_t rn_tick = 0;        // Used for RN timing

/* IPC Heap */
HeapBufMP_Params   heapBufMPParams;
HeapBufMP_Handle   heapHandle;

/* Queues */
static MessageQ_QueueId     mac_common_send_queue_id;
static MessageQ_Handle 	    mac_common_send_queue;
static MessageQ_Params 		mac_common_send_queue_params;

static MessageQ_QueueId     mac_rcc_send_queue_id;
static MessageQ_Handle      mac_rcc_send_queue;
static MessageQ_Params 		mac_rcc_send_queue_params;

static MessageQ_QueueId     mac_cc_input_queue_id;
static MessageQ_Handle      mac_cc_input_queue_handle;
static MessageQ_Params 		mac_cc_input_queue_params;

static MessageQ_QueueId     log_queue_id;
static MessageQ_Handle      log_queue;
static MessageQ_Params 		log_queue_params;

static MessageQ_QueueId     manager_queue_id;
static MessageQ_Handle 	    manager_queue_handle;
static MessageQ_Params 		manager_queue_params;

static MessageQ_QueueId     cmd_queue_id;
static MessageQ_Handle 	    cmd_queue_handle;
static MessageQ_Params 		cmd_queue_params;

static MessageQ_QueueId     ap_sim_queue_id;
static MessageQ_Handle 	ap_sim_queue_handle;
static MessageQ_Params 		ap_sim_queue_params;


/* Tasks */
static Task_Params rn_manager_task_params;
static Task_Handle rn_manager_task_handle;
static void	rn_manager_task( UArg arg0, UArg arg1);

static Task_Params rn_log_task_params;
static Task_Handle rn_log_task_handle;
static void rn_log_task( UArg arg0, UArg arg1);

static Task_Params rn_cmd_task_params;
static Task_Handle rn_cmd_task_handle;
static void rn_cmd_task( UArg arg0, UArg arg1);

static Task_Params mac_cc_task_params;
static Task_Handle mac_cc_task_handle;
static void mac_cc_task( UArg arg0, UArg arg1);

static Task_Params ap_sim_task_params;
static Task_Handle ap_sim_task_handle;
static void ap_sim_task( UArg arg0, UArg arg1);

static char fail_text[64];
/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/
static void rn_manager_timer(UArg arg);
static void rn_manager_timer_create(void);
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/
void rn_sysbios_init()
{

	Int status;

	/* Create the heap that will be used to allocate messages. */
#if 0
	HeapBufMP_Params_init(&heapBufMPParams);
	heapBufMPParams.regionId = 0; /* use default region */
	heapBufMPParams.name = "myHeap";
	heapBufMPParams.align = 256;
	heapBufMPParams.numBlocks = 40;
	heapBufMPParams.blockSize = 1024;
	heapBufMPParams.gate = NULL; /* use system gate */
	heapHandle = HeapBufMP_create(&heapBufMPParams);

	if (heapHandle == NULL)
	{
	System_abort("HeapBufMP_create failed\n");
	}

	status = HeapBufMP_open("myHeap", &heapHandle);

	//MessageQ_registerHeap(HeapBufMP_Handle_upCast(heapHandle), HEAPID);
	MessageQ_registerHeap((IHeap_Handle) heapHandle, HEAPID);

#endif
	// -------------------------------------------------------------
	// Create and open mac_common_send_queue, common queue for logical channels except LRCCR
	// -------------------------------------------------------------
	MessageQ_Params_init(&mac_common_send_queue_params);
	mac_common_send_queue = MessageQ_create("mac_common_send_queue",&mac_common_send_queue_params);

	if (NULL == mac_common_send_queue)
	{
		System_abort("Could not create mac_common_send_queue\n");
	}

	status = MessageQ_open("mac_common_send_queue", &mac_common_send_queue_id);
	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open mac_common_send_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open mac_rcc_send_queue, for LRCCR
	// -------------------------------------------------------------
	MessageQ_Params_init(&mac_rcc_send_queue_params);
	mac_rcc_send_queue = MessageQ_create("mac_rcc_send_queue", &mac_rcc_send_queue_params);

	if (NULL == mac_rcc_send_queue)
	{
		System_abort("Could not create mac_rcc_send_queue\n");
	}

	status = MessageQ_open("mac_rcc_send_queue", &mac_rcc_send_queue_id);
	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open mac_rcc_send_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open mac_cc_input_queue
	// -------------------------------------------------------------
	MessageQ_Params_init(&mac_cc_input_queue_params);
	mac_cc_input_queue_handle = MessageQ_create("mac_cc_input_queue", &mac_cc_input_queue_params);

	if (NULL == mac_cc_input_queue_handle)
	{
		System_abort("Could not create mac_cc_input_queue\n");
	}

	status = MessageQ_open("mac_cc_input_queue", &mac_cc_input_queue_id);
	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open mac_cc_input_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open log_queue
	// -------------------------------------------------------------
	MessageQ_Params_init(&log_queue_params);
	log_queue = MessageQ_create("log_queue", &log_queue_params);

	if (NULL == log_queue)
	{
		System_abort("Could not create log_queue\n");
	}

	status = MessageQ_open("log_queue", &log_queue_id);

	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open log_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open manager_queue
	// -------------------------------------------------------------
	MessageQ_Params_init(&manager_queue_params);
	manager_queue_handle = MessageQ_create("manager_queue", &manager_queue_params);

	if (NULL == manager_queue_handle)
	{
		System_abort("Could not create manager_queue\n");
	}

	status = MessageQ_open("manager_queue", &manager_queue_id);

	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open manager_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open cmd_queue
	// -------------------------------------------------------------
	MessageQ_Params_init(&cmd_queue_params);
	cmd_queue_handle = MessageQ_create("cmd_queue", &cmd_queue_params);

	if (NULL == cmd_queue_handle)
	{
		System_abort("Could not create cmd_queue\n");
	}

	status = MessageQ_open("cmd_queue", &cmd_queue_id);

	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open cmd_queue\n");
	}
	// -------------------------------------------------------------
	// Create and open cmd_queue
	// -------------------------------------------------------------
	MessageQ_Params_init(&ap_sim_queue_params);
	ap_sim_queue_handle = MessageQ_create("ap_sim_queue", &ap_sim_queue_params);

	if (NULL == ap_sim_queue_handle)
	{
		System_abort("Could not create ap_sim_queue\n");
	}

	status = MessageQ_open("ap_sim_queue", &ap_sim_queue_id);

	if (MessageQ_E_NOTFOUND == status)
	{
		System_abort("Could not open ap_sim_queue\n");
	}

	/***************************************************************************************************/
	/* Create the RN-manager task. */
	Task_Params_init(&rn_manager_task_params);
	rn_manager_task_params.stackSize = RN_STANDARD_STACK_SIZE;
	rn_manager_task_params.priority = RN_MANAGER_TASK_PRIORITY;
	rn_manager_task_handle = Task_create((Task_FuncPtr)rn_manager_task, &rn_manager_task_params, NULL);
	//rn_manager_task_handle = Task_create((Task_FuncPtr)rn_manager_task, NULL, NULL);

	if (rn_manager_task_handle == NULL)
	{
		System_abort("rn_manager_task create failed");
	}
	/***************************************************************************************************/
	/* Create the RN-cmd task. */
	Task_Params_init(&rn_cmd_task_params);
	rn_cmd_task_params.stackSize = RN_STANDARD_STACK_SIZE;
	rn_cmd_task_params.priority = RN_CMD_TASK_PRIORITY;
	rn_cmd_task_handle = Task_create((Task_FuncPtr)rn_cmd_task, &rn_cmd_task_params, NULL);
	//rn_cmd_task_handle = Task_create((Task_FuncPtr)rn_cmd_task, NULL, NULL);
	if (rn_cmd_task_handle == NULL)
	{
		System_abort("rn_cmd_task create failed");
	}
	/***************************************************************************************************/
	/* Create the RN-log task. */
	Task_Params_init(&rn_log_task_params);
	rn_log_task_params.stackSize = RN_STANDARD_STACK_SIZE;
	rn_log_task_params.priority = RN_LOG_TASK_PRIORITY;
	rn_log_task_handle = Task_create((Task_FuncPtr)rn_log_task, &rn_log_task_params, NULL);
	//rn_log_task_handle = Task_create((Task_FuncPtr)rn_log_task, NULL, NULL);

	if (rn_log_task_handle == NULL)
	{
		System_abort("rn_log_task create failed");
	}
	/***************************************************************************************************/
	/* Create the mac_cc_task task. */
	Task_Params_init(&mac_cc_task_params);
	mac_cc_task_params.stackSize = RN_STANDARD_STACK_SIZE;
	mac_cc_task_params.priority = RN_MAC_TASK_PRIORITY;
    mac_cc_task_handle = Task_create((Task_FuncPtr)mac_cc_task, &mac_cc_task_params, NULL);
	//mac_cc_task_handle = Task_create((Task_FuncPtr)mac_cc_task, NULL, NULL);
	if (mac_cc_task_handle == NULL)
	{
	System_abort("mac_cc_task create failed");
	}

	/***************************************************************************************************/
	/* Create the mac_cc_task task. */Task_Params_init(&ap_sim_task_params);
	ap_sim_task_params.stackSize = RN_STANDARD_STACK_SIZE;
	ap_sim_task_params.priority = AP_SIM_TASK_PRIORITY;
	ap_sim_task_handle = Task_create((Task_FuncPtr) ap_sim_task, &ap_sim_task_params, NULL);
	//ap_sim_task_handle = Task_create((Task_FuncPtr) ap_sim_task,NULL, NULL);

	if (ap_sim_task_handle == NULL)
	{
		System_abort("ap_sim_task create failed");
	}
	/***************************************************************************************************/



}
MessageQ_QueueId rn_mac_cc_input_queue_id(void)
{
	return mac_cc_input_queue_id;
}
MessageQ_QueueId rn_log_queue_id(void)
{
	return log_queue_id;
}

MessageQ_QueueId rnm_queue_id(void)
{
	return manager_queue_id;
}
MessageQ_QueueId rn_ap_sim_queue_id(void)
{
	return ap_sim_queue_id;
}

/*bool rn_msg_queue_receive(RTOS_QUEUE_HANDLE_T xQueue, void * const pvBuffer, portTickType xTicksToWait)
{
	Int status;

	status = MessageQ_get(xQueue, pvBuffer, xTicksToWait);

	if (status < 0)
	{
	System_abort("Should not happen; timeout is forever\n");
	}
}*/

/* RTOS_TIMER_HANDLE_T rn_manager_timer_create(
		const char * pcTimerName,
		portTickType xTimerPeriodInTicks,
		unsigned portBASE_TYPE uxAutoReload,
		void * pvTimerID,
		tmrTIMER_CALLBACK pxCallbackFunction )*/

static void rn_manager_timer_create(void)
{
	Clock_Params clockParams;
	Clock_Handle myClock;
	//Error_Block eb;
	//Error_init(&eb);
	Clock_Params_init(&clockParams);
	clockParams.period = RN_BASE_TIME_TICKS;
	clockParams.startFlag = TRUE;
	clockParams.arg = (UArg)0x5555;

	myClock = Clock_create(rn_manager_timer,RN_BASE_TIME_TICKS, &clockParams, NULL);
	if (myClock == NULL) {
	System_abort("Clock create failed");
	}
	return;
}
static void rn_manager_timer(xdc_UArg arg)
{
	msg_buf_t *timer_msg;

	rn_tick++;

	timer_msg = msg_buf_alloc(4);

	timer_msg->pd = PD_SYNC;
	timer_msg->mt = SYNC_RN_TIMER;

	msg_put_uint32(timer_msg,0,rn_tick);

	rtos_send(rnm_queue_id(),timer_msg);

}
/*static RTOS_QUEUE_HANDLE_T rn_msg_queue_create(
		char *  name,
		unsigned portBASE_TYPE uxQueueLength,
		unsigned portBASE_TYPE uxItemSize)
{
	RTOS_QUEUE_HANDLE_T q_handle = NULL;

	return q_handle;

}*/
void * rtos_alloc(size_t size)
{
	MessageQ_Msg msg;

	msg = MessageQ_alloc(HEAPID, size);

	if (msg == NULL)
	{
		System_abort("MessageQ_alloc failed\n");
	}
	return msg;
}

void rtos_free(void * msg)
{
	MessageQ_free(msg);
}
/*void taskENTER_CRITICAL(void)
{

}
void taskEXIT_CRITICAL(void)
{

}
*/
void taskDISABLE_INTERRUPTS(void)
{

}

int rtos_send(MessageQ_QueueId queue_id,void *msg_buf)
{
	int result;

	MessageQ_setMsgPri(msg_buf,MessageQ_NORMALPRI);

	result = MessageQ_put(queue_id,(MessageQ_Msg)msg_buf);

	if(MessageQ_S_SUCCESS != result)
	{
		sprintf(fail_text,"rtos_send: MessageQ_put failed, queue_id = %d, result = %d",queue_id,result);
		System_abort(fail_text);
	}

	return result;
}


int rtos_return_msg_to_queue(MessageQ_Handle queue_handle,void *msg_buf)
{
	int result;
	MessageQ_QueueId queue_id;

	MessageQ_setReplyQueue(queue_handle,msg_buf);
	queue_id = MessageQ_getReplyQueue(msg_buf);

	MessageQ_setMsgPri(msg_buf,MessageQ_HIGHPRI);

	result = MessageQ_put(queue_id,(MessageQ_Msg)msg_buf);

	if(MessageQ_S_SUCCESS != result)
	{
		sprintf(fail_text,"rtos_return_msg_to_queue: MessageQ_put failed, queue_id = %d, result = %d",queue_id,result);
		System_abort(fail_text);
	}

	return result;
}


RTOS_QUEUE_HANDLE_T rn_mac_rcc_send_queue(void)
{
	return mac_rcc_send_queue;
}
MessageQ_QueueId rn_mac_rcc_send_queue_id(void)
{
	return mac_rcc_send_queue_id;
}

RTOS_QUEUE_HANDLE_T rn_mac_common_queue(void)
{
    return mac_common_send_queue;

}

MessageQ_QueueId rn_mac_common_queue_id(void)
{
    return mac_common_send_queue_id;

}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/
static void rn_log_task( UArg arg0, UArg arg1)
{
    log_buf_t   *log_buf =  NULL;

    ( void ) arg0;			/* Remove warning about unused parameters. */
    ( void ) arg1;			/* Remove warning about unused parameters. */

    for( ;; )
    {
        RTOS_QUEUE_RECEIVE(log_queue, &log_buf, portMAX_DELAY );
        rn_log_msg_print(log_buf);
        RTOS_FREE(log_buf);
    }
}

static void rn_cmd_task(UArg arg0, UArg arg1)
{
    msg_buf_t *msg_buf;

    //( void ) pvParameters;			/* Remove warning about unused parameters. */

    for( ;; )
    {
        RTOS_QUEUE_RECEIVE(cmd_queue_handle, &msg_buf, portMAX_DELAY );
        rn_log_msg_receive(RN_CMD, msg_buf);

      /*  switch(msg_buf->pd)
        {
        case PD_RNI:  printf("AP received RNI message %02X (hex)\n\r",msg_buf->mt); break;
        case PD_CCI:  printf("AP received CCI message %02X (hex)\n\r",msg_buf->mt); break;
        case PD_TCI:  printf("AP received TCI message %02X (hex)\n\r",msg_buf->mt); break;
        case PD_SYNC: break;
        default:printf("AP received unknown PD %d\n\r",msg_buf->pd); break;
        }*/

        msg_buf_free(msg_buf);
    }
}

MessageQ_QueueId rn_manager_queue_id()
{
    return manager_queue_id;
}

/*-----------------------------------------------------------*/
static void rn_manager_task( UArg arg0, UArg arg1)
{
	 ( void ) arg0;			/* Remove warning about unused parameters. */
	 ( void ) arg1;			/* Remove warning about unused parameters. */
 	/* Create the RNM input queue, used to pass messages to the RN-manager task. */

	//rnm_queue = RTOS_QUEUE_CREATE( RNM_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

	rn_manager_timer_create();
    rn_manager_task_fkn(manager_queue_handle);
}

static void mac_cc_task(UArg arg0, UArg arg1)
{
	 ( void ) arg0;			/* Remove warning about unused parameters. */
	 ( void ) arg1;			/* Remove warning about unused parameters. */
    //mac_cc_input_queue      = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    rn_mac_cc_task_fkn(mac_cc_input_queue_handle);

}


static void ap_sim_task(UArg arg0, UArg arg1)
{
	msg_buf_t *msg_buf;

	(void) arg0; /* Remove warning about unused parameters. */
	(void) arg1; /* Remove warning about unused parameters. */

	ap_sim_module_start(2);

	for (;;)
	{
		RTOS_QUEUE_RECEIVE(ap_sim_queue_handle, &msg_buf, portMAX_DELAY);
		ap_sim_msg(msg_buf);
		msg_buf_free(msg_buf);
	}

}
/*
-----------------------------------------------------------
*/
/*static void rn_timer_cb( RTOS_TIMER_HANDLE_T xTimer )
{
    msg_buf_t *timer_msg;
    (void) xTimer;

    timer_msg = msg_buf_alloc(0);

    timer_msg->pd = PD_SYNC;
    timer_msg->mt = SYNC_RNM_TIMER;

    // RTOS_QUEUE_SEND( rnm_queue, &timer_msg, 0U );
    rtos_send(rnm_queue_id(),timer_msg);
}*/
void vAssertCalled( void )
{
const unsigned long ulLongSleep = 1000UL;

	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		Task_sleep(200);
		//Sleep( ulLongSleep );
	}
}
void console_send(char *str)
{
	trace(str);
}
void log_send(char *str)
{
	trace(str);
}
