/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : ap_sim.h
* Author            : Per Erik Sundvisson
* Description       : Definition of ap_sim module external interface
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

#ifndef AP_SIM_H
#define AP_SIM_H

void ap_sim_module_init(void);
void ap_sim_module_start(uint8_t start_phase);
void ap_sim_msg(msg_buf_t *msg_buf);
RTOS_QUEUE_ID_T ap_sim_queue_id(void);

// Simulation messages, for test of RN services

#define APP_APOLL_REQ       (0x10)
#define APP_APOLL_RSP       (0x11)
#define APP_BPOLL_REQ       (0x12)
#define APP_BPOLL_RSP       (0x13)
#define APP_STATUS_REQ      (0x14)
#define APP_STATUS_RSP      (0x15)

#endif /* AP_SIM_H */



