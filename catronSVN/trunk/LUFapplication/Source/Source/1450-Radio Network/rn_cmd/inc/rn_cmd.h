/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_cmd.h
* Author            : Per Erik Sundvisson
* Description       : Definition of rn_cmd module external interface
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

#ifndef RN_CMD_H
#define RN_CMD_H

void rn_cmd_module_init(void);
void rn_cmd_module_start(uint8_t start_phase);
void rn_cmd_task_fkn(void);
//const char *rn_result_text(rn_result_t result);
#endif /* RN_MANAGER_H */



