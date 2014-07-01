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
#define RN_PHY_QUEUE_LENGTH         ( 10 )
#define RN_PHY_MIN_SLOT             (1)
#define RN_PHY_MAX_SLOT             (15)

#define PHY_SLOT_FREQUENCY			( 10 / portTICK_RATE_MS )

#define PHY_MASTER_RECEIVE_PORT     (5010)
#define PHY_MASTER_SEND_PORT        (5011)
#define PHY_SLAVE_RECEIVE_PORT      (PHY_MASTER_SEND_PORT)
#define PHY_SLAVE_SEND_PORT         (PHY_MASTER_RECEIVE_PORT)

/*
-----------------------------------------------------------
Externals
-------------------------------------------------------------
*/
#include <WinSock2.h>
SOCKET prvOpenUDPSocket(uint16_t port);
SOCKET prvCloseUDPSocket(uint16_t port);
/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rtos.h"
#include "rni.h"
#include "rn_phy.h"
#include "rn_log.h"
#include "phy_sim.h"

extern bool rn_phy_UDP_register_tccr_message_receiver(MessageQ_Handle queue);
extern bool rn_phy_UDP_register_tuca_message_receiver(MessageQ_Handle queue);

extern void rn_phy_comm_task( void *pvParameters );
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/

typedef enum                
{
	PHY_IDLE,
    PHY_SLAVE_SEARCHING,
    PHY_SLAVE_NORMAL,
	PHY_MASTER
} phy_main_state_t;   

static phy_main_state_t phy_main_state          = PHY_IDLE;
static phy_main_state_t phy_main_state_order    = PHY_IDLE;
static phy_slot_conf_t phy_slot_conf[16];

static MessageQ_Handle      plc_queue	        = NULL; 

static MessageQ_Handle      phy_control_queue	= NULL; 
static MessageQ_Handle      phy_send_queue	    = NULL; 

static MessageQ_QueueId     tccr_sender_queue_id    = NULL; 
static MessageQ_QueueId     tccr_receiver_queue_id  = NULL; 

static MessageQ_QueueId     uct_sender_queue     = NULL; 
static MessageQ_QueueId     uct_receiver_queue   = NULL;

//static RTOS_QUEUE_HANDLE_T rni_receiver = NULL;

static char log_text[256];
/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static void phy_task( void *pvParameters);           // Execute PHY timing simulator task
static void phy_send_task( void *pvParameters);      // Execute PHY send simulator task

static void phy_slot_execute(uint8_t slot_cnt, uint16_t frame);

static void send_frame_synch(uint16_t frame);
static void send_state_info(uint8_t mt);

static msg_buf_receiver_t * tccr_msg_receiver   = NULL;    // Callback TBD
static msg_buf_receiver_t * tct_msg_receiver    = NULL;    // Callback TBD
static msg_buf_receiver_t * transport_receiver  = NULL;
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------*/
phy_result_t  rn_phy_register_receiver(msg_buf_receiver_t * receiver_cb)
{
    transport_receiver = receiver_cb;
    return PHY_OK;
}
phy_result_t rn_phy_register_tccr_message_receiver(MessageQ_QueueId queue)
{
    tccr_receiver_queue_id = queue;

    rn_phy_UDP_register_tccr_message_receiver(queue);

    return PHY_OK;
}

phy_result_t rn_phy_register_tccr_message_sender(MessageQ_QueueId queue)
{
    tccr_sender_queue_id = queue;
    return PHY_OK;
}

phy_result_t rn_phy_register_uct_message_receiver(MessageQ_QueueId queue)
{
    uct_receiver_queue = queue;

    rn_phy_UDP_register_tuca_message_receiver(uct_receiver_queue);

    return PHY_OK;
}

phy_result_t rn_phy_register_uct_message_sender(MessageQ_QueueId queue)
{
    uct_sender_queue = queue;
    return PHY_OK;
}

phy_result_t rn_phy_register_plc_receiver(MessageQ_QueueId queue)
{
    plc_queue = queue;
    return PHY_OK;
}

/*-----------------------------------------------------------*/
void rn_phy_idle_conf(void)
{
    phy_main_state_order = PHY_IDLE;
    rn_log_text("rn_phy_idle_conf()");

    //printf("rn_phy_idle_conf()\n\r");
}

void rn_phy_master_conf(uint8_t rnid)
{
    phy_main_state_order      = PHY_MASTER;
   
    sprintf(log_text,"rn_phy_master_conf(%d)",rnid);
    rn_log_vtext(log_text);
}

void rn_phy_slave_conf(uint8_t rnid)
{
    phy_main_state_order      = PHY_SLAVE_SEARCHING;
    sprintf(log_text,"rn_phy_slave_conf(%d)",rnid);
    rn_log_vtext(log_text);
}

/*-----------------------------------------------------------
Function: rn_phy_module_init
-------------------------------------------------------------*/

void rn_phy_module_init(void)
{
    uint8_t     slot = RN_PHY_MIN_SLOT;
    phy_main_state_order = PHY_IDLE;
    phy_slot_conf[0] = PHY_SLOT_INACTIVE; // Shall not be used, first slot is numbered "1"

    for (slot = RN_PHY_MIN_SLOT; slot <= RN_PHY_MAX_SLOT; slot++)
    {
        phy_slot_conf[slot] = PHY_SLOT_INACTIVE;
    }

	phy_control_queue   = RTOS_QUEUE_CREATE( RN_PHY_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    phy_send_queue      = RTOS_QUEUE_CREATE( RN_PHY_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

	RTOS_TASK_CREATE( phy_task, ( signed char * ) "PHYTIM", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
	RTOS_TASK_CREATE( phy_send_task, ( signed char * ) "PHYSND", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
    
    RTOS_TASK_CREATE( rn_phy_comm_task, ( signed char * ) "PHYUDP", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
    return;
}

/*-----------------------------------------------------------
Function: rn_phy_module_start
-------------------------------------------------------------
*/
void rn_phy_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}

/*------------------------------------------------------------------
Function: Get send queue.
--------------------------------------------------------------------*/

MessageQ_QueueId rn_phy_send_queue_get(void)
{
    return phy_send_queue;
}

/*------------------------------------------------------------------
Functions: Activate slot
--------------------------------------------------------------------*/
phy_result_t rn_phy_activate_slot(uint8_t slot,phy_slot_conf_t slot_conf)
{
    if (slot < RN_PHY_MIN_SLOT)
    {
        sprintf(log_text,"PHY_FAIL rn_phy_activate_slot(%d,%d)",slot,slot_conf);
        rn_log_vtext(log_text);

        return PHY_FAIL;
    }

    if (slot > RN_PHY_MAX_SLOT)
    {
        sprintf(log_text,"PHY_FAIL rn_phy_activate_slot(%d,%d)",slot,slot_conf);
        rn_log_vtext(log_text);
        return PHY_FAIL;
    }

    phy_slot_conf[slot] = slot_conf;

    sprintf(log_text,"PHY_OK rn_phy_activate_slot(%d,%d)",slot,slot_conf);
    rn_log_vtext(log_text);
    return PHY_OK;
}

/*------------------------------------------------------------------
Function: De-Activate slot
--------------------------------------------------------------------*/
phy_result_t rn_phy_deactivate_slot(uint8_t slot)
{
    if (slot < RN_PHY_MIN_SLOT)
    {
        return PHY_FAIL;
    }

    if (slot > RN_PHY_MAX_SLOT)
    {
        return PHY_FAIL;
    }

    phy_slot_conf[slot] = PHY_SLOT_INACTIVE;
    return PHY_OK;
}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/

/*
-----------------------------------------------------------
Function: phy_task
-------------------------------------------------------------
*/
static void phy_task( void *pvParameters)
{
    portTickType xNextWakeTime;
    uint8_t     slot_counter    =   0;  // Slot number is slot_counter + 1
    uint16_t    frame_counter   =   0;
    //uint16_t    search_counter  =   0;
    //int16_t    normal_counter  =   0;
    uint32_t        state_counter    =   0;
    uint32_t        random_time     =   0;
    ( void ) pvParameters;			/* Remove warning about unused parameters. */
    
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        //RTOS_QUEUE_RECEIVE(phy_control_queue, &msg_buf, portMAX_DELAY );
        
        //vTaskDelayUntil( &xNextWakeTime, PHY_SLOT_FREQUENCY );
        vTaskDelayUntil( &xNextWakeTime, 1 );
        state_counter++;

        if(phy_main_state != phy_main_state_order)
        {
            state_counter = 0;
        }
        phy_main_state = phy_main_state_order;

        switch (phy_main_state)
        {
        case PHY_IDLE:
            if (0 == state_counter)
            {
                send_state_info(PLC_PHY_IDLE);
                //rn_log_text("PHY IDLE");
            }

            break;
        case PHY_SLAVE_SEARCHING:
            if (0 == state_counter)
            {
                 send_state_info(PLC_PHY_SLAVE_SEARCHING);
                 random_time = 100;
                 //rn_log_text("PHY SEARCHING");
            }
           
            if(state_counter > random_time)
            {
                phy_main_state_order = PHY_SLAVE_NORMAL;
            }
            break;
        case PHY_SLAVE_NORMAL:
            if (0 == state_counter)
            {
                send_state_info(PLC_PHY_SLAVE_NORMAL);
                random_time = 15*(rand() % 500);
                //rn_log_text("PHY NORMAL");
            }

            send_frame_synch(frame_counter);
            for (slot_counter = 1;slot_counter <=15; slot_counter++)
            {
                phy_slot_execute(slot_counter, frame_counter);
            }
            
            /*if(normal_counter >random_time)
            {
                phy_main_state = PHY_SLAVE_SEARCHING;
                search_counter = 0;
                normal_counter = 0;
            }*/
            
           /* if(0 == (slot_counter % 15))
            {
                slot_counter = 0;
                frame_counter++;
                send_frame_synch(frame_counter);
            }
            
            phy_slot_execute(slot_counter+1, frame_counter);*/
            break;
        case PHY_MASTER:
            if (0 == state_counter)
            {
                send_state_info(PLC_PHY_MASTER);
            }
            
            send_frame_synch(frame_counter);
            for (slot_counter = 1;slot_counter <=15; slot_counter++)
            {
                phy_slot_execute(slot_counter, frame_counter);
            }

           /* if(0 == (slot_counter % 15))
            {
                slot_counter = 0;
                frame_counter++;
                send_frame_synch(frame_counter);
            }
            phy_slot_execute(slot_counter+1, frame_counter);*/
            break;
        default: 
            break;
        }
       /* slot_counter++;*/
        frame_counter++;
    }
}
/*
*/


static void phy_send_task( void *pvParameters)
{
    msg_buf_t *msg_buf = NULL;
    portBASE_TYPE   receive_result  = pdFALSE;

    ( void ) pvParameters;			/* Remove warning about unused parameters. */

    for( ;; )
    {
        receive_result = MESSAGEQ_GET(phy_send_queue, &msg_buf, portMAX_DELAY );

        if(receive_result ==  pdTRUE)
        {
            rn_log_msg_receive(RN_PHY, msg_buf);

            if(msg_buf->pd == PD_TCCR)
            {
                switch (msg_buf->mt)
                {
                case TCCR_DATA:     
                    send_to_port(msg_buf);
                    break;

                default:
                    msg_buf_free(msg_buf);
                    break;
                }
            } else if(msg_buf->pd == PD_TCI)
            {
                switch (msg_buf->mt)
                {
                case TUCA_DATA:
                    //rn_log_text_msg("",msg_buf);
                    send_to_port(msg_buf);
                    break;
                default:
                    msg_buf_free(msg_buf);
                    break;
                }
            }
            else
            {
                msg_buf_free(msg_buf);
            }
        }
    }
}
static void phy_slot_execute(uint8_t slot, uint16_t frame)
{
    msg_buf_t *msg_buf;

    //printf("Frame = %d, Slot = %d \n\r", frame, slot);

    switch (phy_slot_conf[slot])
    {
    case PHY_SLOT_DCC_SEND:
    case PHY_SLOT_UCC_SEND:

        msg_buf                         = msg_buf_alloc(TCCR_READY_TO_SEND_SIZE);
        msg_buf->pd                     = PD_TCCR;
        msg_buf->mt                     = TCCR_READY_TO_SEND;
        msg_buf->data[TCCR_TIMESLOT_POS]= slot;
        msg_buf->data[TCCR_FN_POS]      = (uint8_t) ((frame >> 8) & 0xff);
        msg_buf->data[TCCR_FN_POS+1]    = (uint8_t) ((frame >> 0) & 0xff);

        if (NULL != tccr_sender_queue_id)
        {
            rn_log_msg_send(RN_PHY, msg_buf);
            RTOS_QUEUE_SEND(tccr_sender_queue_id, &msg_buf, 0U ); 
        }
        break;

    case PHY_SLOT_UC_SEND:

        msg_buf                         = msg_buf_alloc(TUCA_READY_TO_SEND_SIZE);
        msg_buf->pd                     = PD_TCI;
        msg_buf->mt                     = TUCA_READY_TO_SEND;
        msg_buf->data[TCCR_TIMESLOT_POS]= slot;
        msg_buf->data[TCCR_FN_POS]      = (uint8_t) ((frame >> 8) & 0xff);
        msg_buf->data[TCCR_FN_POS+1]    = (uint8_t) ((frame >> 0) & 0xff);

        if (NULL != uct_sender_queue)
        {
            rn_log_msg_send(RN_PHY, msg_buf);
            RTOS_QUEUE_SEND(uct_sender_queue, &msg_buf, 0U ); 
        }

    default: 
        break;
    }
}
        
static void send_state_info(uint8_t mt)
{
    msg_buf_t *msg_buf;

  /*  if(PHY_IDLE == phy_main_state)
    {
        return;
    }*/
    msg_buf = msg_buf_alloc(0);
    
    msg_buf->pd                             = PD_PLC;
    msg_buf->mt                             = mt;
    
    rn_log_msg_send(RN_PHY, msg_buf);
    RTOS_QUEUE_SEND(plc_queue, &msg_buf, 0U );

    return;
}

static void send_frame_synch(uint16_t frame)
{
    msg_buf_t *msg_buf;

    if(PHY_IDLE == phy_main_state)
    {
        return;
    }
    msg_buf = msg_buf_alloc(2);
    
    msg_buf->pd                             = PD_SYNC;
    msg_buf->mt                             = SYNC_FRAME;
    
    msg_buf->data[0]    = (uint8_t) ((frame>>8)&0x00FF);
    msg_buf->data[1]    = (uint8_t) ((frame>>0)&0x00FF);

    rn_log_msg_send(RN_PHY, msg_buf);
    RTOS_QUEUE_SEND(plc_queue, &msg_buf, 0U );

    return;
}