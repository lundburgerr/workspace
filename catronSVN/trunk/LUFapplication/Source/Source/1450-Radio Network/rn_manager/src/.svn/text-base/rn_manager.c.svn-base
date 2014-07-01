/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_manager.c
* Author            : Per Erik Sundvisson
* Description       : Definition of common RN management data and functions
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

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
#define RNM_QUEUE_LENGTH					( 10 )
/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/

#include "rtos.h"

/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rni.h"

#include "rn_cmd.h"
#include "rn_manager.h"
#include "rn_master.h"
#include "rn_slave.h"
#include "rn_listener.h"
#include "rn_rlc_cc.h"
#include "rn_mac_cc.h"
#include "rn_phy.h"
#include "rn_log.h"
//#include "rn_msg.h"

/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/

static rn_main_conf_t rn_main_conf      = RN_UNDEFINED; // RN-manager main configuration state
static MessageQ_Handle  rnm_queue	= NULL;         // Queue handler, for message reception
static RTOS_QUEUE_ID_T rni_receiver     = NULL;         // Queue id, for message sending

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/
static rn_result_t eqmid_allowed(uint32_t eqmid);
static rn_result_t rnid_allowed(uint8_t rnid);
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/

/*
-----------------------------------------------------------
*/
void rn_module_init(void)
{
    rn_cmd_module_init();
    rn_mac_cc_module_init();
    rn_manager_module_init();
    rn_master_module_init();
    rn_msg_init();
    rn_phy_module_init();  
    rn_rlc_cc_module_init();
    rn_slave_module_init();
    rn_log_module_init();
}
/*
-----------------------------------------------------------
*/

void rn_module_start(uint8_t start_phase)
{
    rn_cmd_module_start(start_phase);
    rn_mac_cc_module_start(start_phase); 
    rn_manager_module_start(start_phase); 
    rn_master_module_start(start_phase); 
    rn_phy_module_start(start_phase); 
    rn_rlc_cc_module_start(start_phase); 
    rn_slave_module_start(start_phase);
    rn_log_module_start(start_phase);
}

/*-----------------------------------------------------------
Function: rn_master_module_init
-------------------------------------------------------------
*/
void rn_manager_module_init(void)
{
    return;
}
/*-----------------------------------------------------------
Function: rn_master_module_start
-------------------------------------------------------------*/
void rn_manager_module_start(uint8_t start_phase)
{
    (void) start_phase;
    return;
}
/*-----------------------------------------------------------
Function: rn_master_module_start
-------------------------------------------------------------*/
rn_result_t rni_tc_id_range_get(uint8_t *first_tc_id, uint8_t *last_tc_id)
{
    *first_tc_id = TCI_MIN_CHANNEL_NUMBER;
    *last_tc_id  = TCI_MAX_CHANNEL_NUMBER;

    return RN_OK;
}
/* ----------------------------------------------------------- */
const char *rn_result_text(rn_result_t result)
{
    switch(result)
    {
    case RN_OK:                                 return "RN_OK";
    case RN_ORDERED:                            return "RN_ORDERED";
    case RN_FAILURE:                            return "RN_FAILURE";
    case RN_VALUE_OUT_OF_RANGE:                 return "RN_VALUE_OUT_OF_RANGE";
    case RN_ILLEGAL_QUEUE_REFERENCE:            return "RN_ILLEGAL_QUEUE_REFERENCE";
    case RN_SW_START_MAYBE_NOT_EXECUTED:        return "RN_SW_START_MAYBE_NOT_EXECUTED";
    case RN_NETWORK_ALREADY_STARTED:            return "RN_NETWORK_ALREADY_STARTED";
    case RN_ADDRESS_OUT_OF_RANGE:               return "RN_ADDRESS_OUT_OF_RANGE";
    case RN_NULL_POINTER_RECEIVED:              return "RN_NULL_POINTER_RECEIVED";
    case RN_MAX_DATA_SIZE_EXCEEDED:             return "RN_MAX_DATA_SIZE_EXCEEDED";
    case RN_MESSAGE_RECEIVER_NOT_REGISTERED:    return "RN_MESSAGE_RECEIVER_NOT_REGISTERED";
    case RN_PHY_REPORTED_FAILURE:               return "RN_PHY_REPORTED_FAILURE";
    case RN_RNID_OUT_OF_RANGE:                  return "RN_RNID_OUT_OF_RANGE";
    case RN_EQMID_OUT_OF_RANGE:                 return "RN_EQMID_OUT_OF_RANGE";
        
    default: return ">>>>> RESULT NOT DECODED";
    }
}

/*------------------------------------------------------------------
Function: Register RNI message receiver 
--------------------------------------------------------------------*/
rn_result_t rni_register_message_receiver	(RTOS_QUEUE_ID_T queue_id)
{
    if (queue_id != NULL)
    {
        rni_receiver = queue_id;    // Not thread safe, only one may call anyway
        return RN_OK;
    }
    rn_log_err("RN_ILLEGAL_QUEUE_REFERENCE");
    return RN_ILLEGAL_QUEUE_REFERENCE;

}
/*------------------------------------------------------------------
Function: Start Radio Network
--------------------------------------------------------------------*/
rn_result_t rni_start(uint8_t rnid, uint32_t eqmid)
{
    msg_buf_t *msg_buf = NULL;

    if(RN_OK != rnid_allowed(rnid))
    {
        return rnid_allowed(rnid);
    }

    if(RN_OK != eqmid_allowed(eqmid))
    {
        return eqmid_allowed(eqmid);
    }

    if(NULL == rni_receiver)
    {
        rn_log_err("RN_MESSAGE_RECEIVER_NOT_REGISTERED");
        return RN_MESSAGE_RECEIVER_NOT_REGISTERED;
    }
    
    if(NULL != rnm_queue)
    {
        msg_buf = msg_buf_alloc(5);

        msg_buf->pd             = PD_RNI;
        msg_buf->mt             = RNI_START;
       
        msg_put_uint8 (msg_buf,0,rnid);
        msg_put_uint32(msg_buf,1,eqmid);
 
        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);
        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}
/*------------------------------------------------------------------
Function: Stop Radio Network
--------------------------------------------------------------------*/
rn_result_t rni_stop(void)
{
    return rni_undefine();
}
/*------------------------------------------------------------------
Function: Connect to Radio Network
--------------------------------------------------------------------*/

rn_result_t rni_connect(uint8_t  rnid, uint32_t eqmid)
{
   
    msg_buf_t *msg_buf = NULL;
    (void) rnid;
    (void) eqmid; 

    if(RN_OK != rnid_allowed(rnid))
    {
        return rnid_allowed(rnid);
    }

    if(RN_OK != eqmid_allowed(eqmid))
    {
        return eqmid_allowed(eqmid);
    }

    if(NULL == rni_receiver)
    {
        rn_log_err("RN_MESSAGE_RECEIVER_NOT_REGISTERED");
        return RN_MESSAGE_RECEIVER_NOT_REGISTERED;
    }

    if(NULL != rnm_queue)
    {
        msg_buf                 = msg_buf_alloc(5);
        msg_buf->pd             = PD_RNI;
        msg_buf->mt             = RNI_CONNECT;

        msg_put_uint8 (msg_buf,0,rnid);
        msg_put_uint32(msg_buf,1,eqmid);
               
        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);

        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}

/*------------------------------------------------------------------
Function: Undefine unit
--------------------------------------------------------------------*/
rn_result_t rni_undefine(void)
{
    msg_buf_t *msg_buf = NULL;

    if(NULL != rnm_queue)
    {
        msg_buf = msg_buf_alloc(0);
        msg_buf->pd     = PD_RNI;
        msg_buf->mt     = RNI_UNDEFINE;

        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);
        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}
/*------------------------------------------------------------------
Function: DisConnect from Radio Network
--------------------------------------------------------------------*/
rn_result_t rni_disconnect(void)
{
    return rni_undefine();
}
/*------------------------------------------------------------------
Function: Listen to Radio Network
--------------------------------------------------------------------*/

rn_result_t rni_sniff(uint8_t  rnid)
{
   
    msg_buf_t *msg_buf = NULL;
   
    if(RN_OK != rnid_allowed(rnid))
    {
        return rnid_allowed(rnid);
    }

    if(NULL == rni_receiver)
    {
        rn_log_err("RN_MESSAGE_RECEIVER_NOT_REGISTERED");
        return RN_MESSAGE_RECEIVER_NOT_REGISTERED;
    }

    if(NULL != rnm_queue)
    {
        msg_buf                 = msg_buf_alloc(1);
        msg_buf->pd             = PD_RNI;
        msg_buf->mt             = RNI_SNIFF;

        msg_put_uint8 (msg_buf,0,rnid);
               
        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);
        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}
rn_result_t rni_sync_on(void)
{
    msg_buf_t *msg_buf = NULL;
   
         
    if(NULL == rni_receiver)
    {
        rn_log_err("RN_MESSAGE_RECEIVER_NOT_REGISTERED");
        return RN_MESSAGE_RECEIVER_NOT_REGISTERED;
    }

    if(NULL != rnm_queue)
    {
        msg_buf                 = msg_buf_alloc(1);
        msg_buf->pd             = PD_RNI;
        msg_buf->mt             = RNI_SYNC_ON;

 
               
        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);
        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}
rn_result_t rni_sync_off(void)
{
    msg_buf_t *msg_buf = NULL;
   
         
    if(NULL == rni_receiver)
    {
        rn_log_err("RN_MESSAGE_RECEIVER_NOT_REGISTERED");
        return RN_MESSAGE_RECEIVER_NOT_REGISTERED;
    }

    if(NULL != rnm_queue)
    {
        msg_buf                 = msg_buf_alloc(1);
        msg_buf->pd             = PD_SYNC;
        msg_buf->mt             = RNI_SYNC_OFF;

 
               
        rn_log_msg_send(RN_RNM, msg_buf);
        //RTOS_QUEUE_SEND( rnm_queue, &msg_buf, 0U );
        rtos_send(rnm_queue_id(),msg_buf);
        return RN_ORDERED;
    }
    else
    {
        rn_log_err("RN_SW_START_MAYBE_NOT_EXECUTED");
        return RN_SW_START_MAYBE_NOT_EXECUTED;
    }
}
/*
-----------------------------------------------------------
For description see header file
-----------------------------------------------------------
*/
void receive_rrc_apoll_req(msg_buf_t *msg_buf, uint32_t own_eqmid)
{
    // Respond to sender with RRC_APOLL_RSP
    msg_buf_t *status_response = msg_buf_copy(msg_buf);  // New buffer must be allocated since the original will be freed after this function
    uint32_t index = 0; 

    msg_put_uint8_i(status_response,&index,RRC_APOLL_RSP);

    msg_return_uint32_i(status_response,&index);            // Skip timestamp
    msg_put_uint32_i   (status_response,&index,own_eqmid);
    msg_set_data_size  (status_response,index);

    status_response->mt            = RRC_APOLL_RSP;        // For logging only
    status_response->pd            = PD_RRC;               // For logging only
    status_response->rn_receiver   = msg_buf->rn_sender;   // For logging only
    
    //rn_log_msg_send(RN_RNM,status_response);

    rn_rccr_send(status_response->rn_receiver, status_response);
}
/*
-----------------------------------------------------------
For description see header file
-----------------------------------------------------------
*/
void receive_rrc_bpoll_req(msg_buf_t *msg_buf, uint32_t own_eqmid)
{
    // Respond to sender with RRC_BPOLL_RSP
    msg_buf_t *status_response = msg_buf_copy(msg_buf);  // The original will be freed after this function
    uint32_t index = 0; 

    msg_put_uint8_i(status_response,&index,RRC_BPOLL_RSP);

    msg_return_uint32_i(status_response,&index);            // Skip timestamp
    msg_put_uint32_i   (status_response,&index,own_eqmid);
    msg_set_data_size  (status_response,index);

  
    rn_rccr_send(msg_return_rn_sender(msg_buf), status_response);
}
/*
-----------------------------------------------------------
For description see header file
-----------------------------------------------------------
*/
void receive_rrc_status_req(msg_buf_t *msg_buf, uint32_t own_eqmid)
{
    // Respond to sender with RRC_STATUS_RSP
    msg_buf_t *status_response = msg_buf_copy(msg_buf);  // The original will be freed after this function
    uint32_t index = 0; 

    msg_put_uint8_i(status_response,&index,RRC_STATUS_RSP);

    msg_return_uint32_i(status_response,&index);            // Skip timestamp
    msg_put_uint32_i   (status_response,&index,own_eqmid);
    msg_set_data_size  (status_response,index);
  
    rn_rccr_send(msg_return_rn_sender(msg_buf), status_response);
}
/******************************************************************************
* Function Name    : broadcast_rrc_status_req
* Function Purpose : Inquiry to all units to respond                  
* Input parameters : time, timestamp to put in message
* Return value     : void
******************************************************************************/
void broadcast_rrc_status_req(uint32_t time)
{
    msg_buf_t   *status_req;
    uint32_t    data_index;

    status_req = msg_buf_alloc(20); // Max number of application bytes to send
    data_index = 0;

    msg_put_uint8_i     (status_req,&data_index,RRC_STATUS_REQ);
    msg_put_uint32_i    (status_req,&data_index,time);
    msg_set_data_size   (status_req,data_index);

    rn_rccr_broadcast(status_req);
}


/******************************************************************************
* Function Name    : rn_manager_task_fkn
* Function Purpose : RN manager task function                
* Input parameters : control_queue
* Return value     : void
******************************************************************************/

void rn_manager_task_fkn(MessageQ_Handle control_queue)
{
    msg_buf_t   *msg_buf = NULL;
    uint8_t     rnid = RN_INVALID_RNID;
    uint32_t    eqmid = RN_INVALID_EQMID;

    rnm_queue = control_queue;

    rccr_register_message_receiver(rnm_queue_id());           // Registers for reception of Radio Bearer CC Messages, RN
    //rn_mac_cc_register_manager_queue(rnm_queue_id());
    rn_phy_register_plc_receiver(rnm_queue_id());

    //rn_mac_cc_idle_conf();
    //rn_log_text("RN_UNDEFINED");
	
    for( ;; )
	{
        switch (rn_main_conf)
        {
        case RN_UNDEFINED:
         
        	RTOS_QUEUE_RECEIVE(rnm_queue, &msg_buf, portMAX_DELAY );
            rn_log_msg_receive(RN_RNM, msg_buf);
            
            if (msg_buf->pd == PD_RNI)
            {
                switch (msg_buf->mt)
                {
                case RNI_START:
                    rnid =  msg_return_uint8 (msg_buf,0);
                    eqmid = msg_return_uint32(msg_buf,1);
                    rn_main_conf = RN_MASTER;
                    break;

                case RNI_CONNECT:
                    rnid =  msg_return_uint8 (msg_buf,0);
                    eqmid = msg_return_uint32(msg_buf,1);
                    rn_main_conf = RN_SLAVE;
                    break;
            
                case RNI_SNIFF:
                    rnid =  msg_return_uint8 (msg_buf,0);
                    eqmid = msg_return_uint32(msg_buf,1);
                    rn_main_conf = RN_SNIFFER;
                    break;
                case RNI_LIST:
                    console_write("Main state: RN_UNDEFINED");
                    break;

                default:
                    //rn_log_text("Main state:              RN_UNDEFINED");
                   /* rn_log_text_msg("Message not implemented: ",msg_buf);*/
                    break;
                }
            }
            else
            {
                //rn_log_err_msg("Ignored Protocol Discriminator in RN_UNDEFINED state\n\r",msg_buf);
            }
            msg_buf_free(msg_buf);
            break;

        case RN_MASTER:
            rn_log_text("RN_MASTER");
            rn_master(rnm_queue, rni_receiver, rnid, eqmid);
            rn_main_conf = RN_UNDEFINED;
            rn_log_text("RN_UNDEFINED");
            break;

        case RN_SLAVE: 
            rn_log_text("RN_SLAVE");
            rn_slave(rnm_queue, rni_receiver, rnid, eqmid);
            rn_main_conf = RN_UNDEFINED;
            rn_log_text("RN_UNDEFINED");
            break;

        case RN_SNIFFER: 
            rn_log_text("RN_SNIFFER");
            rn_sniffer(rnm_queue, rni_receiver, rnid);
            rn_main_conf = RN_UNDEFINED;
            rn_log_text("RN_UNDEFINED");
            break;

        default: //FIXME, impossible to reach this point?
            break;
        }
    }

}
/******************************************************************************
* Function Name    : rni_console_write
* Function Purpose : Write text on remote console
* Input parameters : RN address
*                    text string, max 18 char, longer texts will be terminated 
*                    with NULL
* Return value     : void
******************************************************************************/
void rni_console_write(uint8_t addr,char *text)
{
    msg_buf_t *text_msg;
    size_t len;

    if(RN_OWN_ADDRESS == addr)
    {
        console_write(text);
        return;
    }

    len = strlen(text);

    if(len + 2 > RN_MAX_MESSAGE_DATA)
    {
        len = RN_MAX_MESSAGE_DATA - 2;
    }

    text_msg = msg_buf_alloc(len+2);
    
    msg_put_uint8(text_msg,0,RRC_CONSOLE_WRITE);

    strcpy((char*)&text_msg->data[1],text);
    
    msg_put_uint8(text_msg,len+2,0);

    rn_rccr_send(addr, text_msg);   
}

/******************************************************************************
* Function Name    : receive_rrc_console_write
* Function Purpose : Write received text on console
* Input parameters : Message buffer with text string in pos 1
* Return value     : void
******************************************************************************/
void receive_rrc_console_write(msg_buf_t *msg_buf)
{
    char text[64];

    sprintf(text,"RN-addr %d: %s \r\n",msg_return_rn_sender(msg_buf),&msg_buf->data[1]);

    console_write(text);
}
/******************************************************************************
* Function Name    : rn_receive_rrc
* Function Purpose : Receive RRC messages, common part                
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

void rn_receive_rrc(msg_buf_t *msg_buf)
{
    uint8_t mt;
    uint32_t response_time;
    uint32_t time_stamp;
    msg_buf_t *rpi_cmd;

    mt = msg_buf->data[0]; 
    

    switch (mt)
    {
    case RRC_CONSOLE_WRITE:
        receive_rrc_console_write(msg_buf);
        break; 

    case RRC_MAC_LIST:
        rpi_cmd = msg_buf_alloc(1);
        msg_put_uint8(rpi_cmd,0,msg_return_rn_sender(msg_buf));
        msg_put_pd(PD_RPI,rpi_cmd);
        msg_put_mt(RPI_MAC_LIST_CMD,rpi_cmd);
        rtos_send(rn_mac_cc_input_queue_id(),rpi_cmd);
        break;
 
    case RRC_MAC_STAT:
        rpi_cmd = msg_buf_alloc(1);
        msg_put_uint8(rpi_cmd,0,msg_return_rn_sender(msg_buf));
        msg_put_pd(PD_RPI,rpi_cmd);
        msg_put_mt(RPI_MAC_STAT_CMD,rpi_cmd);
        rtos_send(rn_mac_cc_input_queue_id(),rpi_cmd);
        break;

    default:
       
        rn_log_err_msg("rn_receive_rrc: Not implemented",msg_buf);
        break;
    }
}
/******************************************************************************
* Function Name    : receive_rpi_stat_res
* Function Purpose : Receive statistics and send to remote or local console
* Input parameters : Message buffer
* Return value     : void
******************************************************************************/
void receive_rpi_stat_res(msg_buf_t *msg_buf)
{
    char text[64];

    sprintf(text,"%s %d %d",
        msg_return_uint32(msg_buf,1),
        msg_return_uint8(msg_buf,5),
        msg_return_uint32(msg_buf,6));

    if(RN_OWN_ADDRESS == msg_return_uint8(msg_buf,0))
    {
        strcat(text,"\r\n");
        console_write(text);
    }
    else
    {
        rni_console_write(msg_return_uint8(msg_buf,0),text);
    }
}

/******************************************************************************
* Function Name    : rn_receive_rpi
* Function Purpose : Receive RPI messages, common part                
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

void rn_receive_rpi(msg_buf_t *msg_buf)
{
    switch (msg_return_mt(msg_buf))
    {
    case RPI_STAT_RES:
        receive_rpi_stat_res(msg_buf);
        break; 
        
    default:
       
        rn_log_err_msg("rn_receive_rrc: Not implemented",msg_buf);
        break;
    }
}
/* 
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/

/******************************************************************************
* Function Name    : eqmid_allowed
* Function Purpose : Check if provided REQMID is allowed
* Input parameters : eqmid, Equipment Identity 
* Return value     : See rn_result_t
******************************************************************************/
static rn_result_t eqmid_allowed(uint32_t eqmid)
{

    if((RN_EQMID_MIN <= eqmid) && (eqmid <= RN_EQMID_MAX))
    {
        return RN_OK;
    }
    return RN_EQMID_OUT_OF_RANGE;
}

/******************************************************************************
* Function Name    : rnid_allowed
* Function Purpose : Check if provided RNID is allowed
* Input parameters : rnid, Radio network Identity 
* Return value     : See rn_result_t
******************************************************************************/
static rn_result_t rnid_allowed(uint8_t rnid)
{
    if((11 <= rnid) && (rnid <= 18))
    {
        return RN_OK;
    }

    if((21 <= rnid) && (rnid <= 28))
    {
        return RN_OK;
    }

    if((31 <= rnid) && (rnid <= 38))
    {
        return RN_OK;
    }

    return RN_RNID_OUT_OF_RANGE;
}
