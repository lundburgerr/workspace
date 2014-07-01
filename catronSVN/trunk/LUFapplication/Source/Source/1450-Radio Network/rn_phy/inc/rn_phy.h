/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_phy.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_phy module external interface
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
* 2013-06-10        : Function headings and other comments added
*                     Changed to IPC types
*******************************************************************************/

#ifndef RN_PHY_H
#define RN_PHY_H

#include "rni.h"

/******************************************************************************
* PLI (Physical Layer Interface) Message Types for manager communication
******************************************************************************/
#define PLC_PHY_IDLE                (0x01)  // No data, size to alloc = 0
#define PLC_PHY_MASTER_CONF         (0x02)  // No data, size to alloc = 0
#define PLC_PHY_SLAVE_CONF          (0x03)  // No data, size to alloc = 0

#define PLC_PHY_SLAVE_SEARCHING     (0x04)  // No data, size to alloc = 0
#define PLC_PHY_SLAVE_NORMAL        (0x05)  // No data, size to alloc = 0

#define PLC_PHY_MASTER              (0x07)  // No data, size to alloc = 0


/******************************************************************************
* TCCR (Transport Control Channel RN) Message Types
******************************************************************************/

#define TCCR_READY_TO_SEND           (0x01)
#define TCCR_DATA                    (0x02)
#define TCCR_EMPTY                   (0x03)

// TCCR Message Size to alloc

#define TCCR_HEADER_SIZE             (3)  

#define TCCR_READY_TO_SEND_SIZE     (TCCR_HEADER_SIZE)
#define TCCR_EMPTY_SIZE             (TCCR_HEADER_SIZE)

#define MAC_DATA_SIZE               (32)    // Including CRC 
#define TCCR_DATA_SIZE              (TCCR_HEADER_SIZE + 3*MAC_DATA_SIZE)

// TCCR Information positions

#define TCCR_TIMESLOT_POS            (0)
#define TCCR_FN_POS                  (1)   // Two bytes, MSB first
#define TCCR_DATA_POS                (3)   // Data to/From air interface, TCCR_AIR_DATA_SIZE bytes

/******************************************************************************
* TUCA (Transport User Channel AP) Message Types
******************************************************************************/
#define TUCA_READY_TO_SEND          (TCI_READY_TO_SEND)
#define TUCA_DATA                   (TCI_DATA)
#define TUCA_EMPTY                  (TCI_EMPTY)

// TUCA Message Size to alloc

#define TUCA_HEADER_SIZE            (TCI_HEADER_SIZE)

#define TUCA_READY_TO_SEND_SIZE     (TUCA_HEADER_SIZE)
#define TUCA_EMPTY_SIZE             (TUCA_HEADER_SIZE)

#define TUCA_AIR_DATA_SIZE          (TCI_AIR_DATA_SIZE)        // This excludes Tag byte
#define TUCA_DATA_SIZE              (TUCA_HEADER_SIZE + TUCA_AIR_DATA_SIZE)

// TUCA Information positions

#define TUCA_TIMESLOT_POS            (TCI_CHANNEL_POS)
#define TUCA_FN_POS                  (TCI_FN_POS)      // Two bytes, MSB first
#define TUCA_DATA_POS                (TCI_DATA_POS)    // Data to/From air interface, TUCA_AIR_DATA_SIZE bytes

/******************************************************************************
* SYNC Synchronization Interface message Types
******************************************************************************/
#define SYNC_FRAME                   (0x01)  // To be sent each frame to registered manager (PLC receiver)
#define SYNC_RN_TIMER                (0x02)  // (parking lot)

// SYNC message size and data position 

#define SYNC_FRAME_SIZE             (2)
#define SYNC_FRAME_FRAME_POS        (0)

/******************************************************************************
* PHY Result types 
******************************************************************************/
typedef enum
{
    PHY_OK = 0,
    PHY_FAIL
} phy_result_t;


/******************************************************************************
* Slot configurations
******************************************************************************/
typedef enum                
{
    PHY_SLOT_INACTIVE,      // Slot Not configured      
	PHY_SLOT_DCC_SEND,      // Slot activated for DCC send
    PHY_SLOT_DCC_RECEIVE,   // Slot activated for DCC receive
    PHY_SLOT_UCC_SEND,      // Slot activated for UCC send
    PHY_SLOT_UCC_RECEIVE,   // Slot activated for UCC receive
    PHY_SLOT_UC_SEND,       // Slot activated for UC send
    PHY_SLOT_UC_RECEIVE     // Slot activated for UC receive
} phy_slot_conf_t; 

/******************************************************************************
* Function Name    : rn_phy_module_init
* Function Purpose : Module data initiatons 
* Input parameters : void 
* Return value     : void
******************************************************************************/
void 	rn_phy_module_init();
/******************************************************************************
* Function Name    : rn_phy_module_start
* Function Purpose : Module start 
* Input parameters : start_phase
*                    Start phase 1, executes in start task. All modules have 
*                    executed module init.
*                    Start phase 2, executes in start task. All modules have 
*                    executed module start phase 1.
* Return value     : void
******************************************************************************/
void 	rn_phy_module_start(uint8_t start_phase);

/******************************************************************************
* Function Name    : rn_phy_idle_conf
* Function Purpose : Configure PHY as IDLE 
* Input parameters : void 
* Return value     : void
******************************************************************************/
void    rn_phy_idle_conf(void);

/******************************************************************************
* Function Name    : rn_phy_master_conf
* Function Purpose : Configure PHY as RN-master.
*                    The RN-master shall generate synchronization
* Input parameters : rnid, Radio Network ID 
* Return value     : void
******************************************************************************/
void    rn_phy_master_conf(uint8_t rnid);    

/******************************************************************************
* Function Name    : rn_phy_slave_conf
* Function Purpose : Configure PHY as RN-slave. 
*                    The RN-slave shall Synchronize to RN-master
* Input parameters : rnid, Radio Network ID 
* Return value     : void
******************************************************************************/
void    rn_phy_slave_conf(uint8_t rnid);        // 

/******************************************************************************
* Function Name    : rn_phy_send_queue_get
* Function Purpose : Get PHY send queue identity
* Input parameters : void 
* Return value     : MessageQ_QueueId, IPC queue identity
* The ID can be used for sending messages to PHY air interface
******************************************************************************/
MessageQ_QueueId rn_phy_send_queue_get(void);
/******************************************************************************
* Function Name    : rn_phy_register_plc_receiver
* Function Purpose : Register PLI (Physical Layer Interface) message receiver
* Input parameters : MessageQ_QueueId, IPC queue identity
* Return value     : See phy_result_t
* This will be used for sending messages to RNM (client) and it is 
* intended for status reports etc. from PHY (server)
******************************************************************************/
phy_result_t rn_phy_register_plc_receiver(MessageQ_QueueId queue_id);

/******************************************************************************
* Function Name    : rn_phy_register_tccr_message_receiver
* Function Purpose : Register TCCR (Transport Control Channel RN) message receiver 
* Input parameters : MessageQ_QueueId, IPC queue identity
* Return value     : See phy_result_t
* This will be used for sending transport messages to MAC CC
******************************************************************************/
phy_result_t rn_phy_register_tccr_message_receiver(MessageQ_QueueId queue_p);

/******************************************************************************
* Function Name    : rn_phy_register_tccr_message_receiver
* Function Purpose : Register TCCR (Transport Control Channel RN) message sender  
* Input parameters : MessageQ_QueueId, IPC queue identity
* Return value     : See phy_result_t
* This will be used for sending send sync messages to MAC CC
******************************************************************************/
phy_result_t rn_phy_register_tccr_message_sender(MessageQ_QueueId queue);

/******************************************************************************
* Function Name    : rn_phy_register_uct_message_receiver
* Function Purpose : Register UCT (User Channel Transport) message receiver  
* Input parameters : MessageQ_QueueId, IPC queue identity
* Return value     : See phy_result_t
* This will be used for sending transport messages to MAC UC
******************************************************************************/
phy_result_t rn_phy_register_uct_message_receiver(MessageQ_QueueId queue_p);

/******************************************************************************
* Function Name    : rn_phy_register_uct_message_sender
* Function Purpose : Register UCT (User Channel Transport) message sender  
* Input parameters : MessageQ_QueueId, IPC queue identity
* Return value     : See phy_result_t
* This will be used for sending SYNC messages to MAC UC
******************************************************************************/
phy_result_t rn_phy_register_uct_message_sender(MessageQ_QueueId queue_p);

/******************************************************************************
* Function Name    : rn_phy_activate_slot
* Function Purpose : Activate slot for specified configuration   
* Input parameters : timeslot number
*                    slot_conf, see phy_slot_conf_t
* Return value     : See phy_result_t
******************************************************************************/
phy_result_t rn_phy_activate_slot(uint8_t timeslot,phy_slot_conf_t slot_conf);

/******************************************************************************
* Function Name    : rn_phy_deactivate_slot
* Function Purpose : DeActivate slot   
* Input parameters : timeslot number
* Return value     : See phy_result_t
******************************************************************************/
phy_result_t rn_phy_deactivate_slot(uint8_t timeslot);

/*----------------------------------------------------------------------------------
Parking lot
------------------------------------------------------------------------------------*/
// Prototypes for callback handling of transport messages to MAC, not implemented yet
// Could be used to avoid a context switch if load becomes an issue (not probable)

typedef bool        msg_buf_receiver_t (msg_buf_t * msg_p);
void                phy_register_tccr_cb(msg_buf_receiver_t * tccr_receiver_cb);
void                phy_register_tct_cb(msg_buf_receiver_t * tct_receiver_cb);

#endif /* RN_PHY_H */
