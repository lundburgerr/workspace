/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_mac_cc.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_mac_cc module external interface
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

#ifndef RN_MAC_CC_H
#define RN_MAC_CC_H

#include "rtos.h"

void rn_mac_cc_module_init(void);
void rn_mac_cc_module_start(uint8_t phase);

void rn_mac_cc_master_conf      (uint8_t rnid);
void rn_mac_cc_slave_conf       (uint8_t rnid);
void rn_mac_cc_sniffer_conf     (uint8_t rnid);
void rn_mac_cc_idle_conf        (void);

void    rn_mac_cc_address_conf (uint8_t RN_address);
uint8_t rn_mac_cc_address(void);

void rn_mac_cc_register_manager_queue   (RTOS_QUEUE_ID_T rnm_queue_conf);
rn_result_t rn_mac_lcc_send(uint8_t pd, msg_buf_t * msg);

// RPI (Radio Protocol Control) Message Types

#define RPI_MASTER_CONF     (0x01)
#define RPI_SLAVE_CONF      (0x02)
#define RPI_SNIFFER_CONF    (0x03)
#define RPI_IDLE_CONF       (0x04)
#define RPI_ADDRESS_CONF    (0x05)

#define RPI_FRAME_SYNC      (0x10)
#define RPI_STARTED         (0x11)
#define RPI_STOPPED         (0x12)

#define RPI_MAC_LIST_CMD    (0x13)
#define RPI_MAC_STAT_CMD    (0x14)
#define RPI_STAT_RES        (0x15)

// RPC Message Size to alloc

#define RPC_FRAME_SYNC_SIZE         (2)

// RPC Information positions
#define  RPC_FRAME_SYNC_FN_POS      (0)

#define LCC_UDATA           (0x10)

typedef bool msg_buf_receiver_t (msg_buf_t * msg_p); // Callback Prototype for message passing without queue

void  rn_mac_register_lcc_receiver(msg_buf_receiver_t * cc_receiver_cb);
void  rn_mac_register_ltc_receiver(msg_buf_receiver_t * tc_receiver_cb);

void  rn_mac_cc_task_fkn(RTOS_QUEUE_HANDLE_T in_queue);

#endif /* RN_MAC_CC_H */




