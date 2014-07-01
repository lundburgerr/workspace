/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_master.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_master module external interface
********************************************************************************
* Revision History
* 2013-03-20        : First Issue
*******************************************************************************/

#ifndef RN_MASTER_H
#define RN_MASTER_H
#include "rtos.h"

void rn_master_module_init(void);
void rn_master_module_start(uint8_t phase);

/******************************************************************************
* Function Name    : rn_master
* Function Purpose : The rn_master function shall be called by the rn_manager when 
*                    the unit shall be configered as RN-master. The function 
*                    implements the RN-master function and executes in the RN-manager
*                    thread.
*                    rn_master supports the PD_RNI, PD_PLC, PD_RPC and PD_RCCR
* Input parameters : rnm_input_queue
*                    rni_output_queue
*                    rnid
*                    eqmid
* Return value     : void
******************************************************************************/
void rn_master(RTOS_QUEUE_HANDLE_T rnm_input_queue, RTOS_QUEUE_ID_T rni_output_queue, uint8_t rnid, uint32_t eqmid);

#endif /* RN_MASTER_H */



