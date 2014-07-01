/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_listener.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_listerner module external interface
********************************************************************************
* Revision History
* 2013-03-20        : First Issue
*******************************************************************************/

#ifndef RN_LISTENER_H
#define RN_LISTENER_H

#include "rtos.h"

/******************************************************************************
* Function Name    : rn_listener_module_init
* Function Purpose : This function executes the RN-listener module initiations.
******************************************************************************/
void rn_listener_module_init(void);

/******************************************************************************
* Function Name    : rn_listener_module_start
* Function Purpose : This function executes the RN-listener module start.
* Input parameters : start phase (FFS)
******************************************************************************/
void rn_listener_module_start(uint8_t start_phase);

/******************************************************************************
* Function Name    : rn_listener
* Function Purpose : This function executes the RN-listener task.
* Input parameters : in_q, queue for messsage inputs
*                    app_q, queue fdefined by the application for event notifications
                     rnid, Radio Network Identity for listening
* Return value     : void
******************************************************************************/
void rn_sniffer(RTOS_QUEUE_HANDLE_T in_q, RTOS_QUEUE_ID_T out_q, uint8_t rnid);

#endif /* RN_LISTENER_H */



