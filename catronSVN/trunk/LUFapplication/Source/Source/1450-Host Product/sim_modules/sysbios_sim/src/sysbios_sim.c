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
#define RN_LOG_QUEUE_LENGTH     (20)
#define RNM_QUEUE_LENGTH		(10)
#define RN_MAC_QUEUE_LENGTH     (20)
#define AP_QUEUE_LENGTH	        (10)
#define MAC_TASK_STACK          (1000)
/*
-----------------------------------------------------------
SysBIOS includes
-------------------------------------------------------------
*/
/* FreeRTOS includes. */

#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
/*
-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rtos.h"
#include "rni.h"
#include "rn_log.h"
#include "rn_mac_cc.h"
#include "rn_phy.h"
#include "ap_sim.h"

/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
static MessageQ_Handle  rn_log_queue	    = NULL; 
static MessageQ_Handle  rn_cmd_queue	    = NULL;

static MessageQ_Handle  rnm_queue	        = NULL;
// MAC control queue provides control and timing for the MAC thread
static MessageQ_Handle mac_cc_input_queue   = NULL;

// Logical Channel send queues

static MessageQ_Handle  mac_common_send_queue       = NULL;  // Common Logical Channel Queue
static MessageQ_Handle  mac_rcc_send_queue          = NULL;  // Registration Control Channel (RRC)

static MessageQ_Handle  ap_sim_queue	= NULL; 

static RTOS_TIMER_HANDLE_T rn_timer_handle;

static uint32_t rn_tick = 0;        // Used for RN timing

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static void rn_log_task( void *pvParameters); 
static void rn_cmd_task(void *p);
static void rn_manager_task( void *pvParameters);
static void mac_cc_task(void *pvParameters);

static void ap_sim_task( void *pvParameters);

static void rn_timer_cb( RTOS_TIMER_HANDLE_T xTimer );

/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/
void rn_freertos_init()
{
   
    RTOS_TASK_CREATE( rn_log_task,      (signed char *)"RN-LOG", configMINIMAL_STACK_SIZE, NULL, RN_LOG_TASK_PRIORITY, NULL );  
	RTOS_TASK_CREATE( rn_cmd_task,      (signed char *)"RN-CMD", configMINIMAL_STACK_SIZE, NULL, RN_CMD_TASK_PRIORITY, NULL );
	RTOS_TASK_CREATE( rn_manager_task,  (signed char *)"RNM",    configMINIMAL_STACK_SIZE, NULL, RN_MANAGER_TASK_PRIORITY, NULL );
    RTOS_TASK_CREATE( mac_cc_task,      (signed char *)"MAC",    MAC_TASK_STACK, NULL, RN_MAC_TASK_PRIORITY, NULL );

	RTOS_TASK_CREATE( ap_sim_task, ( signed char * ) "AP-SIM", configMINIMAL_STACK_SIZE, NULL, RN_CMD_TASK_PRIORITY, NULL );
}

void rn_freertos_start(uint8_t start_phase)
{
     if (1 == start_phase)
    {
        //rccr_register_message_receiver(rnm_queue);           // Registers for reception of Radio Bearer CC Messages, RN
        //rn_mac_cc_register_manager_queue(rnm_queue);
        //rn_phy_register_plc_receiver(rnm_queue);

        //rn_msg_timer_create

        //rn_timer_handle = RTOS_TIMER_CREATE(

        rn_timer_handle =
        		rn_msg_timer_create(
        				"RNMTIM",           // Just a text name, not used by the kernel.
        				RN_BASE_TIME_TICKS,  // The rn timer period in ticks.
        				pdTRUE,             // The timers will auto-reload themselves when they expire.
        				( void * ) 1,       // Assign each timer a unique id equal to its array index.
        				rn_timer_cb         // Each timer calls the same callback when it expires.
        		);
 
        xTimerStart( rn_timer_handle, 0 );
    }
}

RTOS_QUEUE_ID_T rn_mac_cc_input_queue_id(void)
{
    return mac_cc_input_queue;
}
MessageQ_Handle rn_log_queue_id(void)
{
    return rn_log_queue;
}

RTOS_QUEUE_ID_T rn_mac_rcc_send_queue(void)
{
	return mac_rcc_send_queue;
}
RTOS_QUEUE_ID_T rn_mac_rcc_send_queue_id(void)
{
	return mac_rcc_send_queue;
}

MessageQ_Handle rn_mac_common_queue(void)
{
    return mac_common_send_queue;

}

RTOS_QUEUE_ID_T rn_mac_common_queue_id(void)
{
    return mac_common_send_queue;

}

void rtos_send(xQueueHandle xQueue, void *buf)
{
    msg_buf_t *ulValueToSend = (msg_buf_t *) buf;

    xQueueSend( xQueue, &ulValueToSend, 0U );
}

void rtos_return_msg_to_queue(xQueueHandle xQueue, void *buf)
{
    msg_buf_t *ulValueToSend = (msg_buf_t *) buf;

    xQueueSendToFront( xQueue, &ulValueToSend, 0U );
}


RTOS_QUEUE_ID_T rn_ap_sim_queue_id(void)
{
    return ap_sim_queue;
}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/

static void rn_log_task( void *pvParameters)
{
    log_buf_t   *log_buf =  NULL;
    
    ( void ) pvParameters;			/* Remove warning about unused parameters. */
    
    rn_log_queue        = RTOS_QUEUE_CREATE( RN_LOG_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    for( ;; )
    {
        MESSAGEQ_GET(rn_log_queue, &log_buf, portMAX_DELAY );
        rn_log_msg_print(log_buf);
        RTOS_FREE(log_buf);
    }
}

static void rn_cmd_task(void *p)
{
    msg_buf_t *msg_buf;

    ( void ) p;			/* Remove warning about unused parameters. */
      
    rn_cmd_queue = RTOS_QUEUE_CREATE(RNM_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    for( ;; )
    {
        MESSAGEQ_GET(rn_cmd_queue, &msg_buf, portMAX_DELAY );
        rn_log_msg_receive(RN_CMD, msg_buf);
        msg_buf_free(msg_buf);
    }
}

RTOS_QUEUE_ID_T rnm_queue_id()
{
    return rnm_queue;
}

/*-----------------------------------------------------------*/
static void rn_manager_task( void *pvParameters)
{
  	( void )    pvParameters;			/* Remove warning about unused parameters. */
 	/* Create the RNM input queue, used to pass messages to the RN-manager task. */

	rnm_queue = RTOS_QUEUE_CREATE( RNM_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    
    rn_manager_task_fkn(rnm_queue);
}

static void mac_cc_task(void *pvParameters)
{
    ( void ) pvParameters;			/* Remove warning about unused parameters. */

    mac_common_send_queue   = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    mac_rcc_send_queue      = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    mac_cc_input_queue      = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    rn_mac_cc_task_fkn(mac_cc_input_queue);
}

static void ap_sim_task( void *pvParameters)
{
    msg_buf_t *msg_buf;

    ( void ) pvParameters;			/* Remove warning about unused parameters. */

    ap_sim_queue = RTOS_QUEUE_CREATE( AP_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    ap_sim_module_start(2);

    for( ;; )
    {
        // RTOS_QUEUE_RECEIVE(ap_sim_queue, &msg_buf, portMAX_DELAY );
        MESSAGEQ_GET(ap_sim_queue, &msg_buf, portMAX_DELAY );
        ap_sim_msg(msg_buf);
        msg_buf_free(msg_buf);
    }
}
/*
-----------------------------------------------------------
*/
static void rn_timer_cb( RTOS_TIMER_HANDLE_T xTimer )
{
    msg_buf_t *timer_msg;
    (void) xTimer;

    rn_tick++;

    timer_msg = msg_buf_alloc(4);
    
    timer_msg->pd = PD_SYNC;
    timer_msg->mt = SYNC_RN_TIMER;

    msg_put_uint32(timer_msg,0,rn_tick);
    //msg_set_data_size(timer_msg,2); 
    // RTOS_QUEUE_SEND( rnm_queue, &timer_msg, 0U );
    rtos_send(rnm_queue_id(),timer_msg);
}

void log_send(char *str)
{
	printf("%s",str);
}
