/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_rlc_cc.c
* Author            : Per Erik Sundvisson
* Description       : Implementation RN RLC CC functions
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

/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/
#include "rtos.h"
/*
-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rni.h"
#include "rn_rlc_cc.h"
#include "rn_mac_cc.h"
#include "rn_log.h"
#include "rn_phy.h"
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/

static RTOS_QUEUE_ID_T cci_receiver = NULL;         // Shall be defined by application
static RTOS_QUEUE_ID_T rccr_receiver = NULL;        // Shall be defined by application

static uint8_t  own_rn_address  = RN_ADDRESS_INVALID;
static bool     router_on       = false;
static bool     sniffer_on      = false;

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static rn_result_t rn_rlc_acc_sanity_check(msg_buf_t *msg);
static rn_result_t rn_rlc_bcc_sanity_check(msg_buf_t *msg);

static bool lcc_receiver(msg_buf_t * msg_p);
static void ltc_receiver(msg_buf_t * msg_p);
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/

/*-----------------------------------------------------------
Function: rn_rlcmac_module_init
-------------------------------------------------------------
*/
void rn_rlc_cc_module_init(void)
{
    rn_mac_register_lcc_receiver(lcc_receiver);
    rn_mac_register_ltc_receiver(ltc_receiver);
	return;
}

/*-----------------------------------------------------------
Function: rn_rlcmac_module_start
-------------------------------------------------------------
*/
void rn_rlc_cc_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}
/******************************************************************************
* Function Name    : rn_rlc_router_conf
* Function Purpose : Configure RLC for message routing
* Input parameters : bool true for ON, false for OFF
* Return value     : void
******************************************************************************/
void rn_rlc_router_conf(bool on_if_true)
{
    router_on = on_if_true;
}
/******************************************************************************
* Function Name    : rn_rlc_sniffer_conf
* Function Purpose : Configure RLC for message sniffing 
* Input parameters : bool true for sniffing, false for OFF
* Return value     : void
******************************************************************************/
void rn_rlc_sniffer_conf(bool on_if_true)
{
    sniffer_on = on_if_true;
}
/******************************************************************************
* Function Name    : rn_rlc_address_conf
* Function Purpose : Configure RN address 
* Input parameters : RN address
* Return value     : void
******************************************************************************/

void rn_rlc_address_conf (uint8_t addr)
{
    taskENTER_CRITICAL();
    own_rn_address = addr; 
    rn_mac_cc_address_conf(own_rn_address);
    taskEXIT_CRITICAL();
}
/******************************************************************************
* Function Name    : rn_rlc_own_address
* Function Purpose : Get own RN address 
* Input parameters : void
* Return value     : RN address
******************************************************************************/
uint8_t rn_rlc_own_address(void)
{
    uint8_t returnvalue;

    taskENTER_CRITICAL();
    returnvalue = own_rn_address;
    taskEXIT_CRITICAL();
    
    return returnvalue; 
}

/******************************************************************************
* Function Name    : cci_register_message_receiver
* Function Purpose : Register CCI message receiver
* Input parameter  : queue
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_register_message_receiver(RTOS_QUEUE_ID_T queue_handle)
{
   
    if (queue_handle != NULL)
    {
        taskENTER_CRITICAL();
        cci_receiver = queue_handle;
        taskEXIT_CRITICAL();
        return RN_OK;
    }

    return RN_ILLEGAL_QUEUE_REFERENCE;
}
/******************************************************************************
* Function Name   : tci_register_sender
* Function Purpose: A sender queue provided by AP shall be registered with 
*                   RN before activation of any TC send channel. 
*                   It is used by RN to request data to send for activated 
*                   send channels.
* Input parameter : Sender queue handle
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_register_sender(RTOS_QUEUE_ID_T queue_id)
{
    rn_phy_register_uct_message_sender(queue_id);
    return RN_OK; // FIXME: 	Should be evaluated
}
/******************************************************************************
* Function Name   : tci_register_receive_queue
* Function Purpose: Register TC receive queue
*                   This queue is provided by AP and shall be registered 
*                   with RN before activation of any TC receive channel. 
*                   RN will put received data on this queue for activated 
*                   receive channels.
* Input parameter : Receiver queue handle
* Return value    : See rn_result_t
--------------------------------------------------------------------*/
rn_result_t tci_register_receive_queue(RTOS_QUEUE_ID_T queue_id)
{
    rn_phy_register_uct_message_receiver(queue_id);
    return RN_OK; // FIXME: 	Should be evaluated
}

bool rni_tci_channel_within_range(uint8_t channel)
{
    if(channel < TCI_MIN_CHANNEL_NUMBER)
    {
        return false;
    }

    if(channel > TCI_MAX_CHANNEL_NUMBER)
    {
        return false;
    }

    return true;
}
/******************************************************************************
* Function Name   : tci_activate_receive_channel
* Function Purpose: Activate specified channel for reception
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_activate_receive_channel(uint8_t channel)
{

    if (rni_tci_channel_within_range(channel))
    {
       
        if(PHY_OK ==  rn_phy_activate_slot(channel,PHY_SLOT_UC_RECEIVE))
        {
            return RN_OK;
        }
        else
        {
             return RN_PHY_REPORTED_FAILURE;
        }
    }
    else
    {
        return RN_VALUE_OUT_OF_RANGE;
    }  
}
/******************************************************************************
* Function Name   : tci_activate_send_channel
* Function Purpose: Activate specified channel for sending
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_activate_send_channel(uint8_t channel)
{
    if (rni_tci_channel_within_range(channel))
    {
        rn_phy_activate_slot(channel,PHY_SLOT_UC_SEND);
        return RN_OK; // FIXME: 	Result should be evaluated
    }
    else
    {
        return RN_VALUE_OUT_OF_RANGE;
    }  
}
/******************************************************************************
* Function Name   : tci_deactivate_channel
* Function Purpose: Dectivate specified channel
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_deactivate_channel(uint8_t channel)
{
    if (rni_tci_channel_within_range(channel))
    {
        rn_phy_activate_slot(channel,PHY_SLOT_INACTIVE);
        return RN_OK; // FIXME: 	Result should be evaluated
    }
    else
    {
        return RN_VALUE_OUT_OF_RANGE;
    }  
}
/******************************************************************************
* Function Name    : tci_send_queue_get
* Function Purpose : return ID for queue provided by RN for sending of TC data
* Input parameter  : None
* Return value     : Queue ID
*******************************************************************************/
MessageQ_QueueId tci_send_queue_get(void)
{
    return rn_phy_send_queue_get();
}

/******************************************************************************
* Function Name   : rccr_register_message_receiver
* Function Purpose: Register RCCR receive queue
* Input parameter : Receiver queue ID
* Return value    : See rn_result_t
--------------------------------------------------------------------*/
rn_result_t rccr_register_message_receiver(MessageQ_QueueId queue_id)
{
    if (queue_id != NULL)
    {
        taskENTER_CRITICAL();
        rccr_receiver = queue_id;    // Not thread safe, only one may call anyway
        taskEXIT_CRITICAL();
        return RN_OK;
    }
    return RN_ILLEGAL_QUEUE_REFERENCE;
}
/******************************************************************************
* Function Name    : cci_send
* Function Purpose : Send message data to Application in specified RN address 
* Input parameter  : rn_receiver_address
                     msg, message containing data
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_send(uint8_t rn_receiver_address, msg_buf_t *msg)   
{ 
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }

    msg->rn_receiver    = rn_receiver_address;
    msg->rn_sender      = rn_rlc_own_address();

    if (RN_OK != rn_rlc_acc_sanity_check(msg))
    {
        return rn_rlc_acc_sanity_check(msg);
    }

    msg->pd         = PD_RCCA;
    msg->mt         = ACC_UDATA;

    rn_log_msg(RN_RAB,msg);
    //rn_log_msg_receive(RN_RLC, msg);

    return rn_mac_lcc_send(PD_LACCA, msg);
}

/******************************************************************************
* Function Name    : cci_broadcast
* Function Purpose : Broadcast message data to Applications in all connected units  
* Input parameter  : msg, message containing data
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_broadcast(msg_buf_t *msg)
{
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }

    msg->rn_sender      = rn_rlc_own_address();

    if (RN_OK != rn_rlc_bcc_sanity_check(msg))
    {
        return rn_rlc_bcc_sanity_check(msg);
    }

    msg->pd         = PD_RCCA;
    msg->mt         = BCC_UDATA;
     
    rn_log_msg(RN_RAB, msg);

    //rn_log_msg_receive(RN_RLC, msg);
    return rn_mac_lcc_send(PD_LBCCA, msg);
}
/******************************************************************************
* Function Name    : rn_rccr_send
* Function Purpose : Send RN messaage to specified RN address
* Input parameters : RN address, Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rccr_send(uint8_t rn_receiver_address, msg_buf_t *msg)   
{
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }

    msg->rn_sender      = rn_rlc_own_address();
    msg->rn_receiver    = rn_receiver_address;

    if (RN_OK != rn_rlc_acc_sanity_check(msg))
    {
        return rn_rlc_acc_sanity_check(msg);
    }
    
    msg->pd         = PD_RCCR;
    msg->mt         = ACC_UDATA;
        
    rn_log_msg(RN_RAB, msg);

//    rn_log_msg_receive(RN_RLC, msg);

    return rn_mac_lcc_send(PD_LACCR, msg);
}
/******************************************************************************
* Function Name    : rn_rcc_send
* Function Purpose : Send RN registration messaage
* Input parameters : Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rcc_send(msg_buf_t *msg)
{
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }

    if (RN_OK != rn_rlc_bcc_sanity_check(msg))
    {
        return rn_rlc_bcc_sanity_check(msg);
    }
    
    msg->pd         = PD_RCCR;
    msg->mt         = RCC_UDATA;   
    
    rn_log_msg(RN_RAB, msg);
//  rn_log_msg_receive(RN_RLC, msg);

    //rn_log_msg_send(RN_RLC, msg);
    return rn_mac_lcc_send(PD_LRCCR, msg);
}
/******************************************************************************
* Function Name    : rn_rccr_broadcast
* Function Purpose : Broadcast RN messaage
* Input parameters : Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rccr_broadcast(msg_buf_t *msg)
{
    if (RN_OK != rn_rlc_bcc_sanity_check(msg))
    {
        return rn_rlc_bcc_sanity_check(msg);
    }
    
    msg->pd         = PD_RCCR;
    msg->mt         = BCC_UDATA;   
    
    msg->rn_sender  = rn_rlc_own_address();
     
    rn_log_msg(RN_RAB, msg);

    //rn_log_msg_receive(RN_RLC, msg);

    //rn_log_msg_send(RN_RLC, msg);
    return rn_mac_lcc_send(PD_LBCCR, msg);
}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/
static void ltc_receiver(msg_buf_t * msg)
{

}
static bool lcc_receiver(msg_buf_t * msg)
{
    msg_buf_t   *broadcast_copy;
    
    if(msg_return_rn_sender(msg) == rn_rlc_own_address()) // Do not accept messages to own address
    {
        msg_buf_free(msg);
        return false;
    }

    rn_log_msg_receive(RN_RLC, msg);

    if(router_on)       // Route broadcast packets and packets addressed to other units
    {
        switch(msg->pd)
        {
        case PD_LACCR:
            if(msg_return_rn_receiver(msg) != rn_rlc_own_address())
            {
                // FIXME: Shall these messages be logged?
                rn_mac_lcc_send(msg->pd, msg);
                return false;
            }
            break;
        case PD_LACCA:
            if(msg_return_rn_receiver(msg) != rn_rlc_own_address())
            {
                // FIXME: Shall these messages be logged?
                rn_mac_lcc_send(msg->pd, msg);
                return false;
            }
            break;
        case PD_LBCCR:
        case PD_LBCCA:
            broadcast_copy = msg_buf_copy(msg);                   // Original shall be dispatched in this unit
            rn_mac_lcc_send(broadcast_copy->pd, broadcast_copy);  // Copy will be broadcast
            break;
        }
    }
    else                
    {
        if(!sniffer_on) // No routing. If not sniffer, drop addressed packets addressed to other units
        {
            switch(msg->pd)
            {
            case PD_LACCR:
            case PD_LACCA:
                if(msg_return_rn_receiver(msg) != rn_rlc_own_address())
                {
                    msg_buf_free(msg);  // Drop message...
                    return false;             // ...and return
                }
                break;
            }
        }
    }

    switch(msg->pd)
    {
    case PD_LACCR:
        msg->pd = PD_RCCR;
        msg->mt = ACC_UDATA;
        break;
    
    case PD_LBCCR:
        msg->pd = PD_RCCR;
        msg->mt = BCC_UDATA;
        break;
    
    case PD_LRCCR:
        msg->pd = PD_RCCR;
        msg->mt = RCC_UDATA;
        break;

    case PD_LACCA:
        msg->pd = PD_RCCA;
        msg->mt = ACC_UDATA;
        break;

    case PD_LBCCA:
        msg->pd = PD_RCCA;
        msg->mt = BCC_UDATA;
        break;
       
    default: 
        rn_log_err_msg("lcc_receiver, PD not implemented",msg);
        msg_buf_free(msg);
        return false;
    }

    rn_log_msg(RN_RAB, msg);
  
    switch(msg->pd)
    {
    case PD_RCCR:
        if(NULL == rccr_receiver)
        {
            rn_log_err_msg("lcc_receiver, No queue registered for RRC",msg);
            msg_buf_free(msg);
            return true; // Message received but not dispatched
        }
        else
        {
          
            //rn_log_msg_send(RN_RLC,msg);
            //RTOS_QUEUE_SEND(rccr_receiver, &msg, 0U );
        	rtos_send(rnm_queue_id(),msg);
        }
        break;

    case PD_RCCA:
        if(NULL == cci_receiver)
        {
            rn_log_err_msg("lcc_receiver, No queue registered for CCI",msg);
            msg_buf_free(msg);
            return true; // Message received but not dispatched
        }
        else
        {
            //rn_log_msg_send(RN_RLC,msg);
            //RTOS_QUEUE_SEND(cci_receiver, &msg, 0U );
            rtos_send(cci_receiver,msg);
        }
    }
    return true; // Message received in this unit
}
/*-----------------------------------------------------------
Function: rn_rlc_acc_sanity_check
-------------------------------------------------------------*/
static rn_result_t rn_rlc_acc_sanity_check(msg_buf_t *msg)
{
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }

    if(msg->rn_receiver > RN_MAX_ADDRESS)
    {
        return RN_ADDRESS_OUT_OF_RANGE;
    }

    if(msg->size > RN_MAX_MESSAGE_DATA)
    {
        return RN_MAX_DATA_SIZE_EXCEEDED;
    }

   if(msg->rn_sender > RN_MAX_ADDRESS)
   {
       return RN_ADDRESS_OUT_OF_RANGE;
   }

    return RN_OK;
}

/*-----------------------------------------------------------
Function: rn_rlc_bcc_sanity_check
-------------------------------------------------------------*/
static rn_result_t rn_rlc_bcc_sanity_check(msg_buf_t *msg)
{ 
    if(NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }
    
    if(msg->size > RN_MAX_MESSAGE_DATA)
    {
        return RN_MAX_DATA_SIZE_EXCEEDED;
    }

   return RN_OK;
}

