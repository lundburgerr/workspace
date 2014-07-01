/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_manager.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_manager module external interface
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

#ifndef RN_MANAGER_H
#define RN_MANAGER_H

#include "rtos.h"

//
// RN main configuration states
//

typedef enum                
{
	RN_UNDEFINED,
	RN_MASTER,
	RN_SLAVE,
    RN_SNIFFER
} rn_main_conf_t;

//typedef struct
//{
//    MessageQ_MsgHeader  header;             /* Required by IPC*/
//    uint8_t             pd;                 // Protocol Discriminator, defines interpretation of message
//    uint8_t             mt;                 // Message Type
//    uint8_t             rn_receiver;        // RN receiver address, when applicable
//    uint8_t             rn_sender;          // RN sender address, when applicable
//    uint32_t		    size;               // Size of data field, requested by caller
//    uint32_t		    alloc_size;         // Size allocated for buffer
//    uint8_t			    *data;              // Pointer to where message data begins
//    char                *file;
//    int                 line;
//} msg_buf_t;

// Parking lot

#define RN_BASE_TIME_20ms                   (1)                     // The RN base time is 20 ms

#define RN_BASE_TIME_40ms                   ( 2*RN_BASE_TIME_20ms)
#define RN_BASE_TIME_60ms                   ( 3*RN_BASE_TIME_20ms)
#define RN_BASE_TIME_240ms                  (12*RN_BASE_TIME_20ms)
#define RN_BASE_TIME_500ms                  (25*RN_BASE_TIME_20ms)
#define RN_BASE_TIME_620ms                  (31*RN_BASE_TIME_20ms)

#define RN_BASE_TIME_1s                     (50*RN_BASE_TIME_20ms)

#define RN_BASE_TIME_2s                     ( 2*RN_BASE_TIME_1s)
#define RN_BASE_TIME_3s                     ( 3*RN_BASE_TIME_1s)
#define RN_BASE_TIME_4s                     ( 4*RN_BASE_TIME_1s)
#define RN_BASE_TIME_5s                     ( 5*RN_BASE_TIME_1s)

#define RCC_UDATA       (0x03)

// Internal RNI message types
// For regular operation
#define RNI_START       (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x00)
#define RNI_CONNECT     (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x01)
#define RNI_UNDEFINE    (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x02)
#define RNI_UNDEFINED   (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x03)

// For testing
#define RNI_SNIFF       (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x10)
#define RNI_LIST        (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x11)
#define RNI_APOLL_ON    (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x12)
#define RNI_APOLL_OFF   (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x13)
#define RNI_SYNC_ON     (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x14)
#define RNI_SYNC_OFF    (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x15)
#define RNI_SYNC        (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x16)
#define RNI_BPOLL_ON    (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x17)
#define RNI_BPOLL_OFF   (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x18)
#define RNI_HOHO        (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x19)
#define RNI_STAT        (RNI_FIRST_INTERNAL_MESSAGE_TYPE + 0x20)

/******************************************************************************
* Function Name    : rn_module_init
* Function Purpose : Init RN modules
* Input parameters : void
* Return value     : void
* Initiation of RN SW modules. Module data and configuration shall be set to 
* default values
******************************************************************************/

void rn_module_init(void);
/******************************************************************************
* Function Name    : rn_module_start
* Function Purpose : Start RN modules
* Input parameters : start phase
* Return value     : void
* Start of RN SW modules 
******************************************************************************/
void rn_module_start(uint8_t start_phase);
/******************************************************************************
* Function Name    : rn_manager_module_init
* Function Purpose : Init rn_manager module
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_manager_module_init(void);
/******************************************************************************
* Function Name    : rn_manager_module_start
* Function Purpose : start rn_manager module
* Input parameters : start phase
* Return value     : void
******************************************************************************/
void rn_manager_module_start(uint8_t start_phase);

/******************************************************************************
* Function Name    : receive_rrc_apoll_req
* Function Purpose : This function receives and responds to RRC_APOLL_REQ
* Input parameters : msg_buf, RRC_APOLL_REQ message
                     own_eqmid, own eqmid for the response
* Return value     : void
******************************************************************************/
void receive_rrc_apoll_req(msg_buf_t *msg_buf, uint32_t own_eqmid);

/******************************************************************************
* Function Name    : receive_rrc_bpoll_req
* Function Purpose : This function receives and responds to RRC_BPOLL_REQ
* Input parameters : msg_buf, RRC_BPOLL_REQ message
                     own_eqmid, own eqmid for the response
* Return value     : void
******************************************************************************/
void receive_rrc_bpoll_req(msg_buf_t *msg_buf, uint32_t own_eqmid);

/******************************************************************************
* Function Name    : receive_rrc_status_req
* Function Purpose : This function receives and responds to RRC_STATUS_REQ
* Input parameters : msg_buf, RRC_STATUS_REQ message
                     own_eqmid, own eqmid for the response
* Return value     : void
******************************************************************************/
void receive_rrc_status_req(msg_buf_t *msg_buf, uint32_t own_eqmid);

/******************************************************************************
* Function Name    : rn_manager_task_fkn
* Function Purpose : Function implementing manager task
* Input parameters : in_queue, Control queue for the task
* Return value     : void
******************************************************************************/
void rn_manager_task_fkn(MessageQ_Handle in_queue);

/******************************************************************************
* Function Name    : broadcast_rrc_status_req
* Function Purpose : Broadcast RRC_STATUS_REQ over RN
* Input parameters : time, time stamp to be inserted in message
* Return value     : void
******************************************************************************/
void broadcast_rrc_status_req(uint32_t time);

// RRC message types

#define RRC_REGISTER_REQ        (0x10)
#define RRC_REGISTER_CFM        (0x11)
#define RRC_REGISTER_REJ        (0x12)

#define RRC_DEREGISTER_REQ      (0x13)
#define RRC_DEREGISTER_CFM      (0x14)

#define RRC_REG_INFO            (0x20)

#define RRC_UNIT_REPORT         (0x30)
#define RRC_TIMESLOT_REPORT     (0x31)

#define RRC_APOLL_REQ           (0x32)  // Used for testing
#define RRC_APOLL_RSP           (0x33)  // Used for testing

#define RRC_BPOLL_REQ           (0x34)  // Used for testing
#define RRC_BPOLL_RSP           (0x35)  // Used for testing

#define RRC_STATUS_REQ          (0x36)  // Used for testing
#define RRC_STATUS_RSP          (0x37)  // Used for testing

#define RRC_CONSOLE_WRITE       (0x38)  // Used for testing
#define RRC_LIST                (0x39)  // Used for testing
#define RRC_STAT                (0x40)  // Used for testing
#define RRC_RNI_CMD             (0x41)  // Used for testing
#define RRC_MAC_LIST            (0x42)  // Used for testing
#define RRC_MAC_STAT            (0x43)  // Used for testing
/*-----------------------------------------------------------------
Registration state is defined by the master and mirrored to the slaves
-------------------------------------------------------------------*/
// The rn_registration_state_t is shared between units and must not be changed easily

typedef enum
{
    RN_NONEX                        = 0, 
    RN_REGISTERED_CONNECTED         = 1,
    RN_REGISTERED_ABSENT            = 2
} rn_registration_state_t;

typedef struct
{
    uint32_t                    eqmid;
    rn_registration_state_t     rn_registration_state;
    rn_registration_state_t     reported_state;
} rn_registration_data_t;

typedef struct
{
    uint32_t        response_time;
    
} rn_performance_data_t;    // As measured by and reported to master

typedef struct
{
    rn_registration_data_t      rn_registration_data;
    uint32_t                    state_timer;
    rn_performance_data_t       rn_performance_data;
} rn_management_data_t;


// RN internal interface definitions

// Control Interfaces

#define PD_RRC      (0x11)  // Radio Resource Control, for monitoring
#define PD_PLC      (0x12)  // Physical Layer Control
#define PD_RPI      (0x13)  // Radio Protocol Interface

// Radio Bearer Interfaces (RAB)
#define PD_RCCA     PD_CCI  // RAB Control Channel AP
#define PD_RCCR     (0x20)  // RAB Control Channel RN

// Logical Channel Interfaces, RN internal (RLC-MAC)

#define PD_LSCCR    (0x21)  // Logical Synchronization Control Channel RN
#define PD_LRCCR    (0x22)  // Logical Registration Control Channel RN
#define PD_LACCR    (0x23)  // Logical Addressed Control Channel RN
#define PD_LBCCR    (0x24)  // Logical Broadcast Control Channel RN

#define PD_LBCCA    (0x25)  // Logical Broadcast Control Channel AP
#define PD_LACCA    (0x26)  // Logical Addressed Control Channel AP

// Transport Channel Interfaces (MAC-PHY)
#define PD_TCCR     (0x31)  // Transport Control Channel RN  
#define PD_TUCA     (0x32)  // Transport User Channel AP

// Synchonization interface
#define PD_SYNC     (0x40)  // Introduced for separation of synchronization messages during logging/debugging


void remote_write(uint8_t a,char *text);

/******************************************************************************
* Function Name    : rn_receive_rrc
* Function Purpose : Receive RRC messages, common part                
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

void rn_receive_rrc(msg_buf_t *msg_buf);
/******************************************************************************
* Function Name    : rn_receive_rpi
* Function Purpose : Receive RPI messages, common part                
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/
void rn_receive_rpi(msg_buf_t *msg_buf);
/*-------------------------------------------------------------------*/
#endif /* RN_MANAGER_H */



