// Copyright 2013

/*-----------------------------------------------------------
Standard includes
-------------------------------------------------------------
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
-----------------------------------------------------------
Macros
-------------------------------------------------------------
*/
#define RN_PHY_QUEUE_LENGTH         ( 10 )
#define RN_PHY_MIN_SLOT             (1)
#define RN_PHY_MAX_SLOT             (15)

#define PHY_SLOT_FREQUENCY			( 1 / portTICK_RATE_MS )

#define PHY_MASTER_RECEIVE_PORT     (5010)
#define PHY_MASTER_SEND_PORT        (5011)
#define PHY_SLAVE_RECEIVE_PORT      (PHY_MASTER_SEND_PORT)
#define PHY_SLAVE_SEND_PORT         (PHY_MASTER_RECEIVE_PORT)

/*
-----------------------------------------------------------
Externals
-------------------------------------------------------------
*/
//#include <WinSock2.h>
//SOCKET prvOpenUDPSocket(uint16_t port);
//SOCKET prvCloseUDPSocket(uint16_t port);
/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rtos.h"
#include "rni.h"
#include "rn_phy.h"
#include "rn_log.h"
//#include "phy_sim.h"

//extern bool rn_phy_UDP_register_tccr_message_receiver(RTOS_QUEUE_HANDLE_T queue);
//extern bool rn_phy_UDP_register_tuca_message_receiver(RTOS_QUEUE_HANDLE_T queue);

//extern void rn_phy_comm_task( void *pvParameters );
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

static RTOS_QUEUE_ID_T 		phy_control_queue_id;
static RTOS_QUEUE_HANDLE_T 	phy_control_queue_handle;
static MessageQ_Params 		phy_control_queue_params;

//static RTOS_QUEUE_HANDLE_T phy_control_queue		= NULL;

static RTOS_QUEUE_HANDLE_T 	phy_send_queue_handle	= NULL;
static RTOS_QUEUE_ID_T 		phy_send_queue_id;
static MessageQ_Params 		phy_send_queue_params;

//static RTOS_QUEUE_HANDLE_T 	tccr_sender_queue    = NULL;

// External queue identities
static RTOS_QUEUE_ID_T 		plc_queue_id 			= 0;
static RTOS_QUEUE_ID_T 		tccr_sender_queue_id 	= 0;
static RTOS_QUEUE_ID_T		tccr_receiver_queue_id  = 0;
static RTOS_QUEUE_ID_T		uct_sender_queue_id		= 0;
static RTOS_QUEUE_ID_T 		uct_receiver_queue_id	= 0;

//static RTOS_QUEUE_HANDLE_T rni_receiver = NULL;
static Task_Params phy_task_params;
static Task_Params phy_send_task_params;
//static Task_Handle task0;


/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/
static void phy_task(UArg arg0, UArg arg1);

//static void phy_task( void *pvParameters);           // Execute PHY timing simulator task
static void phy_send_task(UArg arg0, UArg arg1);      // Execute PHY send simulator task

static void phy_slot_execute(uint8_t slot_cnt, uint16_t frame);

static void send_frame_synch(uint16_t frame);
static void send_state_info(uint8_t mt);

//static msg_buf_receiver_t * tccr_msg_receiver = NULL;    // Callback TBD
//static msg_buf_receiver_t * tct_msg_receiver = NULL;    // Callback TBD

/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------*/

phy_result_t rn_phy_register_tccr_message_receiver(RTOS_QUEUE_ID_T queue)
{
    tccr_receiver_queue_id = queue;

    //rn_phy_UDP_register_tccr_message_receiver(queue);

    return PHY_OK;
}

phy_result_t rn_phy_register_tccr_message_sender(RTOS_QUEUE_ID_T queue)
{
    tccr_sender_queue_id = queue;
    return PHY_OK;
}

phy_result_t rn_phy_register_uct_message_receiver(RTOS_QUEUE_ID_T queue)
{
    uct_receiver_queue_id = queue;

    //rn_phy_UDP_register_tuca_message_receiver(uct_receiver_queue);

    return PHY_OK;
}

phy_result_t rn_phy_register_uct_message_sender(RTOS_QUEUE_ID_T queue)
{
    uct_sender_queue_id = queue;
    return PHY_OK;
}

phy_result_t rn_phy_register_plc_receiver(RTOS_QUEUE_ID_T queue)
{
    plc_queue_id = queue;
    return PHY_OK;
}

/*-----------------------------------------------------------*/
void rn_phy_idle_conf(void)
{
    phy_main_state_order = PHY_IDLE;
    printf("rn_phy_idle_conf()\n\r");
}

void rn_phy_master_conf(uint8_t rnid)
{
    phy_main_state_order      = PHY_MASTER;
   
    printf("rn_phy_master_conf(%d)\n\r",rnid);
}

void rn_phy_slave_conf(uint8_t rnid)
{
    phy_main_state_order      = PHY_SLAVE_SEARCHING;
  
    printf("rn_phy_slave_conf(%d)\n\r",rnid);
}

//void rn_phy_slave_normal()
//{
//    phy_main_state_order      = PHY_SLAVE_NORMAL;
//  
//    printf("rn_phy_slave_conf(%d)\n\r",rnid);
//}

/*-----------------------------------------------------------
Function: rn_phy_module_init
-------------------------------------------------------------*/

void rn_phy_module_init(void)
{
	Int status;
	//MessageQ_Params messageQParams;
	//Error_Block eb;

    uint8_t     slot = RN_PHY_MIN_SLOT;
    rn_phy_idle_conf();

    phy_slot_conf[0] = PHY_SLOT_INACTIVE; // Shall not be used, first slot is numbered "1"

    for (slot = RN_PHY_MIN_SLOT; slot <= RN_PHY_MAX_SLOT; slot++)
    {
        phy_slot_conf[slot] = PHY_SLOT_INACTIVE;
    }

    // Create and open phy_control_queue
    MessageQ_Params_init(&phy_control_queue_params);
    phy_control_queue_handle = MessageQ_create("phy_control_queue", &phy_control_queue_params);
    status = MessageQ_open("phy_control_queue", &phy_control_queue_id);

    // Create and open phy_send_queue
    MessageQ_Params_init(&phy_send_queue_params);
    phy_send_queue_handle = MessageQ_create("phy_send_queue", &phy_send_queue_params);
    status = MessageQ_open("phy_send_queue", &phy_send_queue_id);

	//phy_control_queue   = RTOS_QUEUE_CREATE( RN_PHY_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

	//MessageQ_Params_init(&messageQParams);

	//phy_send_queue_handle = MessageQ_create("phy_send_queue", &messageQParams);
	//phy_send_queue_id = MessageQ_open
    //phy_send_queue      = RTOS_QUEUE_CREATE( RN_PHY_QUEUE_LENGTH, sizeof( msg_buf_t * ) );


	//Error_init(&eb);
	/* Create 1 task with priority 15 */

	Task_Params_init(&phy_task_params);
	phy_task_params.stackSize = 512;
	phy_task_params.priority = 15;
	//task0 =
	Task_create((Task_FuncPtr)phy_task, &phy_task_params, NULL);

	Task_Params_init(&phy_send_task_params);
	phy_send_task_params.stackSize = 512;
	phy_send_task_params.priority = 15;
		//task0 =
	Task_create((Task_FuncPtr)phy_send_task, &phy_send_task_params, NULL);

	/*if (task0 == NULL)
	{
	System_abort("Task create failed");
	}*/
	//RTOS_TASK_CREATE( phy_task, ( signed char * ) "PHYTIM", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
	//RTOS_TASK_CREATE( phy_send_task, ( signed char * ) "PHYSND", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
    
    //RTOS_TASK_CREATE( rn_phy_comm_task, ( signed char * ) "PHYUDP", configMINIMAL_STACK_SIZE, NULL, RN_PHY_TASK_PRIORITY, NULL );
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

RTOS_QUEUE_ID_T rn_phy_send_queue_get(void)
{
    return phy_send_queue_id;
}

/*------------------------------------------------------------------
Functions: Activate slot
--------------------------------------------------------------------*/
phy_result_t rn_phy_activate_slot(uint8_t slot,phy_slot_conf_t slot_conf)
{
    if (slot < RN_PHY_MIN_SLOT)
    {
        printf("PHY_FAIL rn_phy_activate_slot(%d,%d)\n\r",slot,slot_conf);
        return PHY_FAIL;
    }

    if (slot > RN_PHY_MAX_SLOT)
    {
        printf("PHY_FAIL rn_phy_activate_slot(%d,%d)\n\r",slot,slot_conf); 
        return PHY_FAIL;
    }

    phy_slot_conf[slot] = slot_conf;

    printf("PHY_OK rn_phy_activate_slot(%d,%d)\n\r",slot,slot_conf); 
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

static void phy_task(UArg arg0, UArg arg1)
{
    //portTickType xNextWakeTime;
    uint8_t     slot_counter    =   0;  // Slot number is slot_counter + 1
    uint16_t    frame_counter   =   0;
    //uint16_t    search_counter  =   0;
    //int16_t    normal_counter  =   0;
    uint32_t    state_counter    =   0;
    int         random_time     =   0;
    //msg_buf_t	*msg_buf;

    ( void ) arg0;			/* Remove warning about unused parameters. */
    ( void ) arg1;			/* Remove warning about unused parameters. */
    
    //xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        //RTOS_QUEUE_RECEIVE(phy_control_queue_handle, &msg_buf, portMAX_DELAY );
        
        //vTaskDelayUntil( &xNextWakeTime, PHY_SLOT_FREQUENCY );
        Task_sleep(2);
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
            
            /*if(normal_counter >random_time)
            {
                phy_main_state = PHY_SLAVE_SEARCHING;
                search_counter = 0;
                normal_counter = 0;
            }*/
            
            if(0 == (slot_counter % 15))
            {
                slot_counter = 0;
                frame_counter++;
                send_frame_synch(frame_counter);
            }
            
            phy_slot_execute(slot_counter+1, frame_counter);
            break;
        case PHY_MASTER:
            if (0 == state_counter)
            {
                send_state_info(PLC_PHY_MASTER);
            }

            if(0 == (slot_counter % 15))
            {
                slot_counter = 0;
                frame_counter++;
                send_frame_synch(frame_counter);
            }
            phy_slot_execute(slot_counter+1, frame_counter);
            break;
        default: 
            break;
        }
        slot_counter++;
    }
}
/*
*/

static void phy_send_task(UArg arg0, UArg arg1)
{
    msg_buf_t *msg_buf = NULL;
    portBASE_TYPE   receive_result  = pdFALSE;

    ( void ) arg0;			/* Remove warning about unused parameters. */
    ( void ) arg1;			/* Remove warning about unused parameters. */
    for( ;; )
    {
        receive_result = RTOS_QUEUE_RECEIVE(phy_send_queue_handle, &msg_buf, portMAX_DELAY );

        if(receive_result ==  pdTRUE)
        {
            rn_log_msg_receive(RN_PHY, msg_buf);

            if(msg_buf->pd == PD_TCCR)
            {
                switch (msg_buf->mt)
                {
                case TCCR_DATA:     
                    //send_to_port(msg_buf);
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
                    //send_to_port(msg_buf);
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
            //RTOS_QUEUE_SEND(tccr_sender_queue, (MessageQ_Msg)msg_buf, 0U );
            rtos_send(tccr_sender_queue_id,msg_buf);
        }
        break;

    case PHY_SLOT_UC_SEND:

        msg_buf                         = msg_buf_alloc(TUCA_READY_TO_SEND_SIZE);
        msg_buf->pd                     = PD_TCI;
        msg_buf->mt                     = TUCA_READY_TO_SEND;
        msg_buf->data[TCCR_TIMESLOT_POS]= slot;
        msg_buf->data[TCCR_FN_POS]      = (uint8_t) ((frame >> 8) & 0xff);
        msg_buf->data[TCCR_FN_POS+1]    = (uint8_t) ((frame >> 0) & 0xff);

        if (NULL != uct_sender_queue_id)
        {
            rn_log_msg_send(RN_PHY, msg_buf);
            //RTOS_QUEUE_SEND(uct_sender_queue, (MessageQ_Msg) msg_buf, 0U );
            rtos_send(uct_sender_queue_id,msg_buf);
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
    //RTOS_QUEUE_SEND(plc_queue, &msg_buf, 0U );

    rtos_send(plc_queue_id, msg_buf);

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
    //RTOS_QUEUE_SEND(plc_queue, &msg_buf, 0U );
    rtos_send(plc_queue_id,msg_buf);
    return;
}
