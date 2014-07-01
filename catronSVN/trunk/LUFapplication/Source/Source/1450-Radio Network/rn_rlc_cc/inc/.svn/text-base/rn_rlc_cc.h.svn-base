/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_rlc_cc.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_rlc_cc module external interface
********************************************************************************
* Revision History
* 2013-03-20        : First Issue
*******************************************************************************/
#ifndef RN_RLC_CC_H
#define RN_RLC_CC_H

#include "rtos.h"
/******************************************************************************
* Function Name    : rn_rlc_cc_module_init
* Function Purpose : Init module
* Input parameters : void
* Return value     : void
******************************************************************************/
void        rn_rlc_cc_module_init(void);
/******************************************************************************
* Function Name    : rn_rlc_cc_module_start
* Function Purpose : Start module
* Input parameters : start phase
* Return value     : void
******************************************************************************/
void        rn_rlc_cc_module_start(uint8_t phase);
/******************************************************************************
* Function Name    : rn_rccr_broadcast
* Function Purpose : Broadcast RN messaage
* Input parameters : Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rccr_broadcast   (msg_buf_t *msg);

/******************************************************************************
* Function Name    : rn_rccr_send
* Function Purpose : Send RN messaage to specified RN address
* Input parameters : RN address, Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rccr_send        (uint8_t rn_adr, msg_buf_t *msg);
/******************************************************************************
* Function Name    : rn_rcc_send
* Function Purpose : Send RN registration messaage
* Input parameters : Pointer to message
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rn_rcc_send         (msg_buf_t *msg);

/******************************************************************************
* Function Name    : rccr_register_message_receiver
* Function Purpose : Register receiver of RN messaages
* Input parameters : Queue ID
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rccr_register_message_receiver(RTOS_QUEUE_ID_T queue_handle);
/******************************************************************************
* Function Name    : rn_rlc_address_conf
* Function Purpose : Configure RN address
* Input parameters : RN address
* Return value     : void
******************************************************************************/
void        rn_rlc_address_conf (uint8_t RN_address);
/******************************************************************************
* Function Name    : rn_rlc_router_conf
* Function Purpose : Configure RLC for message routing
* Input parameters : bool true for ON, false for OFF
* Return value     : void
******************************************************************************/

void        rn_rlc_router_conf(bool on_if_true);
/******************************************************************************
* Function Name    : rn_rlc_sniffer_conf
* Function Purpose : Configure RLC for message sniffing 
* Input parameters : bool true for sniffing, false for OFF
* Return value     : void
******************************************************************************/
void        rn_rlc_sniffer_conf(bool on_if_true);
#endif /* RN_RLC_CC_H */



