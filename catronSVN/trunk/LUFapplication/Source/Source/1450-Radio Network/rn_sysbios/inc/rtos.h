/*
    Copyright (C) 2013 Catron
*/

//
// This file aims to map a few RTOS calls and definitions to FreeRTOS for execution in simulated environment
//

#ifndef RTOS_H
#define RTOS_H

/* FreeRTOS includes. */

// #include <FreeRTOS.h>
// #include "task.h"
// #include "queue.h"
// #include "timers.h"

#include <xdc/std.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
/* ---- XDC.RUNTIME module Headers */
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>
/* ----- IPC module Headers */
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>
/* ---- BIOS6 module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Mailbox.h>
/* ---- Get globals from .cfg Header */
#include <xdc/cfg/global.h>
/* FreeRTOS+CLI includes. */

#define tskIDLE_PRIORITY			( ( unsigned portBASE_TYPE ) 0U )
#define portMAX_DELAY     0xFFFFFFFF
#include "CLI.h"
//#include "FreeRTOSConfig.h"
//#include "Error.h"


//typedef int bool;
//#define false 0
//#define true 1

//typedef uint32_t                MessageQ_MsgHeader; // Dummy, not used in simulated environment

#define max(a,b) ((a<b)? b:a)
#define rn_buf_t msg_buf_t
#define msg_buf_alloc(a)    msg_buf_alloc_dbg(a,__FILE__,__LINE__)
#define msg_buf_copy(a)    msg_buf_copy_dbg(a,__FILE__,__LINE__)

//#define	taskENTER_CRITICAL
//typedef void (*Task_FuncPtr)(UArg,UArg);

#define RTOS_ALLOC              	rtos_alloc
#define RTOS_FREE               	rtos_free

#define RTOS_QUEUE_CREATE(a,b)	    rn_msg_queue_create("test",a,b)

#define RTOS_TASK_CREATE	    	xTaskCreate
#define RTOS_QUEUE_RECEIVE(a,b,c)	MessageQ_get(a,(MessageQ_Msg*)b,c)
#define MESSAGEQ_GET            	RTOS_QUEUE_RECEIVE
//#define RTOS_QUEUE_RECEIVE			MessageQ_get
//#define RTOS_QUEUE_PEEK         	xQueuePeek
//#define RTOS_QUEUE_SEND(a,b,c)      MessageQ_put(a,(MessageQ_Msg)b)
#define RTOS_QUEUE_SEND(a,b,c)		MessageQ_put(a,b)
//#define RTOS_QUEUE_HANDLE_T	    	MessageQ_Handle
#define RTOS_QUEUE_HANDLE_T			MessageQ_Handle
#define RTOS_QUEUE_ID_T	    		MessageQ_QueueId

//#define RTOS_QUEUE_POLL(a,b)	xQueueReceive(a,b,0)

#define RTOS_GET_TIME           	Clock_getTicks    // Tick for logging etc.

#define RTOS_TIMER_CREATE       	rn_msg_timer_create
#define RTOS_TIMER_HANDLE_T     	Clock_Handle

#define RTOS_MS_PER_TICK                (1)                         // Milliseconds per tick
#define RN_BASE_TIME_TICKS             (20/RTOS_MS_PER_TICK)        // RN time base is 20 ms

//#define RN_MANAGER_TIMER_20_ms (20)

typedef void (*tmrTIMER_CALLBACK)( RTOS_TIMER_HANDLE_T xTimer );



RTOS_QUEUE_HANDLE_T rn_msg_queue_create(
		char *  name,
		unsigned portBASE_TYPE uxQueueLength,
		unsigned portBASE_TYPE uxItemSize);

RTOS_TIMER_HANDLE_T rn_msg_timer_create(
		const char * pcTimerName,
		uint32_t xTimerPeriodInTicks,
		unsigned portBASE_TYPE uxAutoReload,
		void * pvTimerID,
		tmrTIMER_CALLBACK pxCallbackFunction );


typedef struct
{
    MessageQ_MsgHeader  header;             /* Required by IPC*/
    uint8_t             pd;                 // Protocol Discriminator, defines interpretation of message
    uint8_t             mt;                 // Message Type
    uint8_t             rn_receiver;        // RN receiver address, when applicable
    uint8_t             rn_sender;          // RN sender address, when applicable
    uint32_t		    size;               // Size of data field, requested by caller
    uint32_t		    alloc_size;         // Size allocated for buffer
    uint8_t			    *data;              // Pointer to where message data begins
    char                *file;
    int                 line;
} msg_buf_t;

MessageQ_QueueId     rnm_queue_id(void);

RTOS_QUEUE_HANDLE_T rn_mac_common_queue(void);
MessageQ_QueueId 	rn_mac_common_queue_id(void);

RTOS_QUEUE_HANDLE_T	rn_mac_rcc_send_queue(void);
MessageQ_QueueId     rn_mac_rcc_send_queue_id(void);

MessageQ_QueueId 	rn_mac_cc_input_queue_id(void);

void * rtos_alloc(size_t size);
int rtos_send(RTOS_QUEUE_ID_T receiver,void *msg_buf);
int rtos_return_msg_to_queue(MessageQ_Handle queue_handle,void *msg_buf);

void rtos_free(void * msg);
void console_write(char * text);
void taskENTER_CRITICAL(void);
void taskEXIT_CRITICAL(void);
void taskDISABLE_INTERRUPTS(void);
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled()
void vAssertCalled( void );
#endif /* RTOS_H */



