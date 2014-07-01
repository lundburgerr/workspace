/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_slave.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_slave module external interface
********************************************************************************
* Revision History
* 2013-03-20        : First Issue
*******************************************************************************/
#ifndef RN_SLAVE_H
#define RN_SLAVE_H

#include "rtos.h"

/******************************************************************************
* Function Name    : rn_slave_module_init
* Function Purpose : Module Init              
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_slave_module_init(void);

/******************************************************************************
* Function Name    : rn_slave_module_start
* Function Purpose : Module Start              
* Input parameters : start phase
* Return value     : void
******************************************************************************/
void rn_slave_module_start(uint8_t phase);

/******************************************************************************
* Function Name    : rn_slave
* Function Purpose : This function executes the RN-slave function. Two queues 
*                    are given as input reference. rn_slave receives messages
*                    through the rnm_input_queue and it delivers event messages 
*                    to the application client through the rni_output_queue.
*                    rn_slave supports the PD_RNI, PD_PLC, PD_RPC and PD_RRC
* Input parameters : rnm_input_queue
*                    rni_output_queue
*                    rnid
*                    eqmid
* Return value     : void
******************************************************************************/
void rn_slave(RTOS_QUEUE_HANDLE_T rnm_input_queue, RTOS_QUEUE_ID_T rni_output_queue, uint8_t rnid, uint32_t eqmid);

#endif /* RN_SLAVE_H */
