/*
    Copyright (C) 2013 Catron
*/

//
// This file aims to map a few RTOS calls and definitions to FreeRTOS for execution in simulated environment
//

#ifndef RTOS_H
#define RTOS_H

/* FreeRTOS includes. */

#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "timers.h"

/* FreeRTOS+CLI includes. */

#include "FreeRTOS_CLI.h"
//#include "CLI.h"

typedef int bool;
#define false 0
#define true 1

typedef uint32_t                MessageQ_MsgHeader; // Dummy, not used in host environment  

//#define rn_buf_t msg_buf_t
#define msg_buf_alloc(a)    msg_buf_alloc_dbg(a,__FILE__,__LINE__)
#define msg_buf_copy(a)     msg_buf_copy_dbg(a,__FILE__,__LINE__)

//typedef void (*Task_FuncPtr)(UArg,UArg);

#define RTOS_ALLOC              pvPortMalloc
#define RTOS_FREE               vPortFree

#define RTOS_QUEUE_CREATE	        xQueueCreate
//#define rn_msg_queue_create(a,b,c)  xQueueCreate(b,c)

#define RTOS_TASK_CREATE	    xTaskCreate
#define RTOS_QUEUE_RECEIVE	    xQueueReceive
#define MESSAGEQ_GET            RTOS_QUEUE_RECEIVE  // Sysbios lookalike
#define RTOS_QUEUE_PEEK         xQueuePeek
#define RTOS_QUEUE_SEND(a,b,c)  xQueueSend(a,b,0U)
#define RTOS_QUEUE_HANDLE_T	    xQueueHandle
#define RTOS_QUEUE_ID_T         xQueueHandle

#define MessageQ_QueueId        RTOS_QUEUE_ID_T
#define MessageQ_Handle         RTOS_QUEUE_HANDLE_T

#define MessageQ_put            rtos_send           // Sysbios lookalike
#define MESSAGEQ_PUT            rtos_send           // Sysbios lookalike
#define MESSAGEQ_PUT_FRONT      rtos_return_msg_to_queue   

//#define RTOS_QUEUE_POLL(a,b)	xQueueReceive(a,b,0)

#define RTOS_GET_TIME           20*xTaskGetTickCount    // Tick experimental 20 ms

#define RTOS_TIMER_CREATE       xTimerCreate        
#define RTOS_TIMER_HANDLE_T     xTimerHandle

#define rn_msg_timer_create     xTimerCreate

#define RTOS_MS_PER_TICK                (4)
#define RN_BASE_TIME_TICKS             (20/RTOS_MS_PER_TICK)

typedef void (*tmrTIMER_CALLBACK)( xTimerHandle xTimer );

// Simulation tasks
#define RN_PHY_TASK_PRIORITY        ( tskIDLE_PRIORITY + 0 )
#define RN_CMD_TASK_PRIORITY        ( tskIDLE_PRIORITY + 5 )

// To become real tasks

#define RN_MAC_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1 )
#define RN_MANAGER_TASK_PRIORITY    ( tskIDLE_PRIORITY + 4 )
#define RN_LOG_TASK_PRIORITY        ( tskIDLE_PRIORITY + 5 )
#define RN_CMD_TASK_PRIORITY        ( tskIDLE_PRIORITY + 5 )

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


void rtos_send(xQueueHandle xQueue, void *buf);
void rtos_return_msg_to_queue(xQueueHandle xQueue, void *buf);
void rn_freertos_init(void);
void rn_freertos_start(uint8_t start_phase);

MessageQ_QueueId    rnm_queue_id(void);

MessageQ_Handle     rn_mac_common_queue(void);
MessageQ_QueueId    rn_mac_common_queue_id(void);

MessageQ_Handle     rn_mac_rcc_send_queue(void);
MessageQ_QueueId    rn_mac_rcc_send_queue_id(void);

MessageQ_QueueId    rn_mac_cc_input_queue_id(void);

MessageQ_QueueId    rn_ap_sim_queue_id(void);

void console_send(char *text);
void console_write(char *text);

void log_send(char *str);
#endif /* RTOS_H */



