/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_log.c
* Author            : Per Erik Sundvisson
* Description       : Definition of RN log commands and functions
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/

/*-----------------------------------------------------------
Standard includes
-------------------------------------------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*-----------------------------------------------------------
Macros
-------------------------------------------------------------*/

//#define RN_LOG_QUEUE_LENGTH (20)

/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/

#include "rtos.h"

/*
-----------------------------------------------------------
Globals
-------------------------------------------------------------
*/

extern int32_t buf_counter;

/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rni.h"
#include "rn_log.h"
#include "rn_phy.h"
#include "rn_mac_cc.h"
#include "rn_manager.h"

/*-----------------------------------------------------------
Local declarations
-------------------------------------------------------------
*/

/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/

static string_for_mt_cb_t * app_string_for_mt = NULL;       // For registration of APP MT to text translation

static bool pd_filter[256];

#define MAX_CHARS 512    
static char text[MAX_CHARS + 1];        // Defined here to save stack. Only to be used by rn_log print thread
static char help_text[MAX_CHARS + 1];   // Defined here to save stack. Only to be used by help command thread
/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static char         *tccr_string_for_mt(uint8_t mt);
static log_buf_t    *log_buf_copy(msg_buf_t * msg_buf);
static log_buf_t    *log_buf_alloc(size_t size);

/*-----------------------------------------------------------
Command declarations
-------------------------------------------------------------*/
/*----------------------------------------------------------*/
static portBASE_TYPE rn_log_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_log_cmd =
{
	( const int8_t * const ) "rn-log", /* The command string to type. */
	( const int8_t * const ) "rn-log      <parameter(s)>      Logging of messages. \"rn-log ?\" lists parameters\r\n",
     //                       ---------   -----------------   ---------
	rn_log_command, /* The function to run. */
	1 /* Number of parameters expected. */
};
/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------*/

/******************************************************************************
* Function Name    : rn_log_module_init
* Function Purpose : Initiation of data in module              
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_log_module_init(void)
{
    uint32_t index;
    
    for (index=0;index<256;index++)
    {
        pd_filter[index] = true;   // switched off for all PD
    }

	/*rn_log_queue        = RTOS_QUEUE_CREATE( RN_LOG_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
   
	RTOS_TASK_CREATE( rn_log_task, ( signed char * ) "RN-LOG", configMINIMAL_STACK_SIZE, NULL, RN_LOG_TASK_PRIORITY, NULL );*/
    
    return;
}

/******************************************************************************
* Function Name    : rn_log_module_start
* Function Purpose : Start of module                
* Input parameters : start phase 
* Return value     : void
******************************************************************************/
void rn_log_module_start(uint8_t start_phase)
{
	(void)start_phase;

    if (1 == start_phase)
    {
        FreeRTOS_CLIRegisterCommand( &rn_log_cmd );
    }

	return;
}

/******************************************************************************
* Function Name    : cci_mt_translator_reg
* Function Purpose : Registration of callback for CCI air interface message type 
*                    to log text translation. First byte in CCI messages is 
*                    interpreted as MT
* Input parameters : Function of type string_for_mt_cb_t
* Return value     : none
******************************************************************************/
void cci_mt_translator_reg(string_for_mt_cb_t * translate_cb)
{
    app_string_for_mt = translate_cb;
}

/******************************************************************************
* Function Name    : rn_log_text
* Function Purpose : Logging of constant text 
* Input parameters : Pointer to constant text string, will not be copied 
* Return value     : none
******************************************************************************/
void rn_log_text(const char * text)
{
    log_buf_t   *log_buf = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    log_buf             = log_buf_alloc(0);
    log_buf->log_format = RN_LOG_TEXT;
    log_buf->tick       = ltick;   
    log_buf->text       = text;

   /* strcpy((char*)log_buf->data,text);
    strcat((char*)log_buf->data,"\n\r");*/

    // RTOS_QUEUE_SEND(rn_log_queue_id(), &log_buf, 0U );
    rtos_send(rn_log_queue_id(), log_buf);

}

/******************************************************************************
* Function Name    : rn_log_vtext
* Function Purpose : Logging any text 
* Input parameters : Pointer to (variable) text string, string will be copied 
* Return value     : none
******************************************************************************/
void rn_log_vtext(char * text)
{
    log_buf_t   *log_buf = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    log_buf             = log_buf_alloc(strlen(text)+1);
    log_buf->log_format = RN_LOG_VTEXT;
    log_buf->tick       = ltick;   
    log_buf->text       = text;

    strcpy((char*)log_buf->data,text);

    rtos_send(rn_log_queue_id(),log_buf);
}
/******************************************************************************
* Function Name    : console_write
* Function Purpose : Write any text to console
* Input parameters : Pointer to (variable) text string, string will be copied 
* Return value     : none
******************************************************************************/
void console_write(char * text)
{
    log_buf_t   *log_buf = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    log_buf             = log_buf_alloc(strlen(text)+1);
    log_buf->log_format = RN_LOG_CONSOLE_WRITE;
    log_buf->tick       = ltick;   
    log_buf->text       = text;

    strcpy((char*)log_buf->data,text);

    rtos_send(rn_log_queue_id(),log_buf);
}
/******************************************************************************
* Function Name    : rn_log_err_plus
* Function Purpose : Logging of constant error text
*                    File and line of caller will also be logged. Shall be 
*                    added by macro
* Input parameters : Pointer to constant text string, string will not be copied 
*                    Pointer to file name
*                    Line number
* Return value     : none
******************************************************************************/

void rn_log_err_plus    (const char * text, const char * file, int line)
{
    log_buf_t   *log_buf = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    log_buf = log_buf_alloc(0);
    log_buf->tick = ltick;
    log_buf->log_format = RN_LOG_ERR;

    log_buf->text = text;
    log_buf->file = file;
    log_buf->line = line;
  
    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_buf, 0U );

    rtos_send(rn_log_queue_id(), log_buf);

}

/******************************************************************************
* Function Name    : rn_log_err_msg_plus
* Function Purpose : Logging of constant error text and message data
*                    File and line of caller will also be logged, shall be 
*                    added by macro
* Input parameters : Pointer to constant text string, string will not be copied 
*                    Pointer to message
*                    Pointer to file name
*                    Line number
* Return value     : none
******************************************************************************/

void rn_log_err_msg_plus (const char * text, msg_buf_t *msg_buf,const char * file, int line)
{
    log_buf_t   *log_copy = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    if (msg_buf == NULL)
    {
        return;
    }

    log_copy = log_buf_copy(msg_buf);
    log_copy->log_format = RN_LOG_ERR_MSG;
    log_copy->text = (char*) text;
    log_copy->tick = ltick;
    log_copy->file = (char*) file;
    log_copy->line = line;

    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_copy, 0U );
    rtos_send(rn_log_queue_id(), log_copy);
}

/******************************************************************************
* Function Name    : rn_log_text_msg
* Function Purpose : Logging of constant error text and message data
* Input parameters : Pointer to constant text string, string will not be copied 
*                    Pointer to message
* Return value     : none
******************************************************************************/
void rn_log_text_msg        (const char * text, msg_buf_t *msg_buf)
{
    log_buf_t   *log_copy = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    if (msg_buf == NULL)
    {
        return;
    }

    /*if(pd_filter[msg_buf->pd])
    {
        return;
    }*/

    log_copy = log_buf_copy(msg_buf);
    log_copy->log_format = RN_LOG_MSG_TEXT;
    log_copy->text = (char*) text;
    log_copy->tick = ltick;
    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_copy, 0U );
    rtos_send(rn_log_queue_id(), log_copy);
}
/******************************************************************************
* Function Name    : rn_log_msg
* Function Purpose : Logging of message data
* Input parameters : Pointer to message
* Return value     : none
******************************************************************************/
void rn_log_msg(rn_log_id_t id, msg_buf_t *msg_buf)
{
    log_buf_t   *log_copy = NULL;
    uint32_t    ltick = RTOS_GET_TIME();
     
    if (RN_RAB != id)
    {
        return;
    }

    if (msg_buf == NULL)
    {
        return;
    }


    if(pd_filter[msg_buf->pd])
    {
        return;
    }

    log_copy                = log_buf_copy(msg_buf);
    log_copy->log_format    = RN_LOG_MSG_RAB;
    log_copy->log_id        = id;
    log_copy->tick          = ltick;
    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_copy, 0U );
    rtos_send(rn_log_queue_id(), log_copy);
    return;
}

/******************************************************************************
* Function Name    : rn_log_msg_send
* Function Purpose : Logging of sent message data
* Input parameters : Log id
*                    Pointer to message
* Return value     : none
******************************************************************************/
void rn_log_msg_send(rn_log_id_t id, msg_buf_t *msg_buf)
{
    log_buf_t   *log_copy = NULL;
    uint32_t    ltick = RTOS_GET_TIME();
     
    if (msg_buf == NULL)
    {
        return;
    }

    if(pd_filter[msg_buf->pd])
    {
        return;
    }

    if(id == RN_RAB)
    {
        return;
    }
    
    log_copy                = log_buf_copy(msg_buf);
    log_copy->log_format    = RN_LOG_MSG_ID_SEND;
    log_copy->log_id        = id;
    log_copy->tick          = ltick;
    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_copy, 0U );
    rtos_send(rn_log_queue_id(), log_copy);
    return;
}

/******************************************************************************
* Function Name    : rn_log_msg_receive
* Function Purpose : Logging of received message data
* Input parameters : Log id
*                    Pointer to message
* Return value     : none
******************************************************************************/
void rn_log_msg_receive(rn_log_id_t id, msg_buf_t *msg_buf)
{
    log_buf_t   *log_copy = NULL;
    uint32_t    ltick = RTOS_GET_TIME();

    if (msg_buf == NULL)
    {
        return;
    }

    if(id == RN_RAB)
    {
        return;
    }
        
    if(RN_PHY == id)
    {
        if(pd_filter[msg_buf->pd])
        {
            return;
        }
    }

    if(pd_filter[msg_buf->pd])
    {
        return;
    }

    log_copy                = log_buf_copy(msg_buf);
    log_copy->log_format    = RN_LOG_MSG_ID_RECEIVE;
    log_copy->log_id        = id;
    log_copy->tick          = ltick;
    //RTOS_QUEUE_SEND(rn_log_queue_id(), &log_copy, 0U );
    rtos_send(rn_log_queue_id(), log_copy);
    return;
}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/


/******************************************************************************
* Function Name    : rpc_string_for_mt
* Function Purpose : Translation of RPI MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * rpc_string_for_mt(uint8_t mt)
{
    switch(mt)
    {

    case RPI_MASTER_CONF:   return "RPI_MASTER_CONF";
    case RPI_SLAVE_CONF:    return "RPI_SLAVE_CONF";
    case RPI_SNIFFER_CONF:  return "RPI_SNIFFER_CONF";
    case RPI_IDLE_CONF:     return "RPI_IDLE_CONF";
    case RPI_STARTED:       return "RPI_STARTED";
    case RPI_STOPPED:       return "RPI_STOPPED";
    case RPI_MAC_LIST_CMD:  return "RPI_MAC_LIST_CMD";
    case RPI_MAC_STAT_CMD:  return "RPI_MAC_STAT_CMD";
    case RPI_STAT_RES:      return "RPI_STAT_RES";

    }
    return "RPI MT?";
}

/******************************************************************************
* Function Name    : rrc_string_for_mt
* Function Purpose : Translation of RRC MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/

static char * rrc_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    case RRC_REGISTER_REQ:      return "RRC_REGISTER_REQ";
    case RRC_REGISTER_CFM:      return "RRC_REGISTER_CFM";
    case RRC_REGISTER_REJ:      return "RRC_REGISTER_REJ";

    case RRC_DEREGISTER_REQ:    return "RRC_DEREGISTER_REQ";
    case RRC_DEREGISTER_CFM:    return "RRC_DEREGISTER_CFM";

    case RRC_REG_INFO:          return "RRC_REG_INFO";
    case RRC_UNIT_REPORT:       return "RRC_UNIT_REPORT";
    case RRC_TIMESLOT_REPORT:   return "RRC_TIMESLOT_REPORT";

    case RRC_APOLL_REQ:         return "RRC_APOLL_REQ";
    case RRC_APOLL_RSP:         return "RRC_APOLL_RSP";

    case RRC_BPOLL_REQ:         return "RRC_BPOLL_REQ";
    case RRC_BPOLL_RSP:         return "RRC_BPOLL_RSP";

    case RRC_STATUS_REQ:        return "RRC_STATUS_REQ";
    case RRC_STATUS_RSP:        return "RRC_STATUS_RSP";

    case RRC_CONSOLE_WRITE:     return "RRC_CONSOLE_WRITE";
    case RRC_LIST:              return "RRC_LIST";
    case RRC_STAT:              return "RRC_STAT";
    case RRC_RNI_CMD:           return "RRC_RNI_CMD";
    case RRC_MAC_LIST:          return "RRC_MAC_LIST";
    case RRC_MAC_STAT:          return "RRC_MAC_STAT";
    }
    return "RRC MT?";
}

/******************************************************************************
* Function Name    : plc_string_for_mt
* Function Purpose : Translation of PLC MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * plc_string_for_mt(uint8_t mt)
{
 
    switch(mt)
    {

    case PLC_PHY_IDLE:              return "PLI_PHY_IDLE";
    case PLC_PHY_MASTER_CONF:       return "PLI_PHY_MASTER_CONF";
    case PLC_PHY_SLAVE_CONF:        return "PLI_PHY_SLAVE_CONF";
    case PLC_PHY_SLAVE_SEARCHING:   return "PLI_PHY_SLAVE_SEARCHING";
    case PLC_PHY_SLAVE_NORMAL:      return "PLI_PHY_SLAVE_NORMAL";
    case PLC_PHY_MASTER:            return "PLI_PHY_MASTER";
    }
    return "PLI MT?";
}

/******************************************************************************
* Function Name    : tccr_string_for_mt
* Function Purpose : Translation of TCCR MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * tccr_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    case TCCR_DATA:             return "TCCR_DATA";
    case TCCR_READY_TO_SEND:    return "TCCR_RTS";
    case TCCR_EMPTY:            return "TCCR_EMPTY";
    }
    return "TCCR MT?";
}

/******************************************************************************
* Function Name    : rccr_string_for_mt
* Function Purpose : Translation of RCCR MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * rccr_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    
    case ACC_UDATA: return "ACC_UDATA";
    case BCC_UDATA: return "BCC_UDATA";
    case RCC_UDATA: return "RCC_UDATA";
    }

    return "RCCR MT?";
}

/******************************************************************************
* Function Name    : tci_string_for_mt
* Function Purpose : Translation of TCI MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * tci_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    
    case TCI_DATA:          return "TCI_DATA";
    case TCI_EMPTY:         return "TCI_EMPTY";
    case TCI_READY_TO_SEND: return "TCI_READY_TO_SEND";
    }

    return "TCI MT?";
}

/******************************************************************************
* Function Name    : rni_string_for_mt
* Function Purpose : Translation of RNI MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * rni_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    case RNI_START:         return "RNI_START";
        //    case RNI_STOP:          return "RNI_STOP";break;
    case RNI_STARTED:       return "RNI_STARTED";

    case RNI_CONNECT:       return "RNI_CONNECT";
    case RNI_SNIFF:         return "RNI_SNIFF";
    case RNI_UNDEFINE:      return "RNI_UNDEFINE";

    case RNI_UNDEFINED:     return "RNI_UNDEFINED";
    case RNI_CONNECTED:     return "RNI_CONNECTED";

    case RNI_DISCONNECTED:  return "RNI_DISCONNECTED";
    case RNI_CONNECTED_LIST:return "RNI_CONNECTED_LIST";
    case RNI_LIST:          return "RNI_LIST";
    case RNI_APOLL_ON:      return "RNI_APOLL_ON";
    case RNI_APOLL_OFF:     return "RNI_APOLL_OFF";
    case RNI_BPOLL_ON:      return "RNI_BPOLL_ON";
    case RNI_BPOLL_OFF:     return "RNI_BPOLL_OFF";
    case RNI_HOHO:          return "RNI_HOHO";
    case RNI_STAT:          return "RNI_STAT";
    }
    return "RNI MT?";
}

/******************************************************************************
* Function Name    : sync_string_for_mt
* Function Purpose : Translation of SYNC MT to message name
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char * sync_string_for_mt(uint8_t mt)
{
    switch(mt)
    {
    case SYNC_FRAME:        return "SYNC_FRAME";
    case SYNC_RN_TIMER:    return "SYNC_RN_TIMER";
    }
    return "SYNC MT?";
}

/******************************************************************************
* Function Name    : string_for_log_id
* Function Purpose : Translation of log id to text 
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char *string_for_log_id(rn_log_id_t log_id)
{
    switch(log_id)
    {
    case RN_APP: return "APP";
    case RN_RNM: return "RNM";
    case RN_CMD: return "CMD";
    case RN_RLC: return "RLC";
    case RN_MAC: return "MAC";
    case RN_PHY: return "PHY";
    }
    return "LOG_ID?";
}

/******************************************************************************
* Function Name    : string_for_pd
* Function Purpose : Translation of PD to short text 
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char *string_for_pd(uint8_t pd)
{
    switch(pd)
    {
    case PD_RNI:    return "RNI";
    case PD_CCI:    return "CCI";
    case PD_TCI:    return "TCI";
    case PD_RRC:    return "RRC";
    case PD_RCCR:   return "RCCR";
    //case PD_RCCA:   return "RCCA"; break;
    case PD_PLC:    return "PLI";
    case PD_RPI:    return "RPI";
    case PD_LSCCR:  return "LSCCR";
    case PD_LRCCR:  return "LRCCR";
    case PD_LACCR:  return "LACCR";

    case PD_LBCCR:  return "LBCCR";
    case PD_TUCA:   return "TUCA";
    case PD_TCCR:   return "TCCR";
    case PD_LBCCA:  return "LBCCA";
    case PD_LACCA:  return "LACCA";
    case PD_SYNC:   return "SYNC";
    } 
    return "PD?";
}

/******************************************************************************
* Function Name    : string_for_pd
* Function Purpose : Translation of PD to long text 
* Input parameters : mt
* Return value     : char * message name
******************************************************************************/
static char *string_for_pd_ext(uint8_t pd)
{
    switch(pd)
    {
    case PD_RNI:    return "RNI  - Radio Network Interface";
    case PD_CCI:    return "CCI  - RAB AP Control Channel Interface";
    case PD_TCI:    return "TCI  - AP Traffic Channel Interface";
    case PD_RRC:    return "RRC  - Radio Resource Control";
    case PD_RCCR:   return "RCCR - RAB RN Control Channel Interface";
    //case PD_RCCA:   return "RCCA"; break;
    case PD_PLC:    return "PLI  - Physical Layer Interface";
    case PD_RPI:    return "RPI  - Radio Protocol Interface";
    case PD_LSCCR:  return "LSCCR - Logical Synchronization Control Channel RN";
    case PD_LRCCR:  return "LRCCR - Logical Registration Control Channel RN";
    case PD_LACCR:  return "LACCR - Logical Addressed Control Channel RN";

    case PD_LBCCR:  return "LBCCR - Logical Broadcast Control Channel RN";
    case PD_TUCA:   return "TUCA  - Transport User Channel AP";
    case PD_TCCR:   return "TCCR  - Transport Control Channel RN";
    case PD_LBCCA:  return "LBCCA - Logical Broadcast Control Channel AP";
    case PD_LACCA:  return "LACCA - Logical Addressed Control Channel AP";
    case PD_SYNC:   return "SYNC  - Synchronization Interface, Internal";
    } 
    return "PD?";
}

/******************************************************************************
* Function Name    : string_for_mt
* Function Purpose : Translation of PD and MT to Message name 
* Input parameters : mt
*                    pd
* Return value     : char * message name
******************************************************************************/
static char *string_for_mt(uint8_t pd, uint8_t mt)
{
    switch (pd)
    {
    case PD_RNI:    return rni_string_for_mt(mt);
    case PD_CCI:    if (NULL != app_string_for_mt)
                    {
                        return app_string_for_mt(mt);
                    }
                    else
                    {
                        return "APP MT?";
                    }
    case PD_TCI:    return tci_string_for_mt(mt);
    
    case PD_RCCR:   return rccr_string_for_mt(mt);
    case PD_PLC:    return plc_string_for_mt(mt);
    case PD_RPI:    return rpc_string_for_mt(mt);
    
    case PD_RRC:    return rrc_string_for_mt(mt);
    
    case PD_LSCCR:  return "PD_LSCCR";
    case PD_LRCCR:  return "PD_LRCCR";
    case PD_LACCR:  return "PD_LACCR";
    case PD_LBCCA:  return "PD_LBCCA";
    case PD_LACCA:  return "PD_LACCA";
    case PD_LBCCR:  return "PD_LBCCR";
        
    case PD_TUCA:   return "PD_TUCA";
    case PD_TCCR:   return tccr_string_for_mt(mt);
    case PD_SYNC:   return sync_string_for_mt(mt);
    }

    return "PD? MT?";
}
//void console_printf(char * format,...)
//{
////#define MAX_CHARS 128
////    char text[MAX_CHARS + 1];
////
////    va_list vl;
////    va_start(vl,format);
////   /* sprintf(text,format,va_arg(vl,t);
////    console_write(text);*/
////    va_end(vl);
//}

/******************************************************************************
* Function Name    : rn_log_msg_print
* Function Purpose : Printout of log message 
* Input parameters : Pointer to log message                  
* Return value     : void
******************************************************************************/
void rn_log_msg_print(log_buf_t *log_buf)
{
    char byte_text[10];
    uint32_t index = 0;

    //printf("%5d.%03d ",log_buf->tick/1000,log_buf->tick%1000);

    if(RN_LOG_CONSOLE_WRITE == log_buf->log_format)
    {
        sprintf(text,"%s",log_buf->data);
        console_send(text);
        return; 
    }

    sprintf(text,"%5d.%03d ",log_buf->tick/1000,log_buf->tick%1000);
    log_send(text);

    switch(log_buf->log_format)
    {
    case RN_LOG_ERR:
        //printf("%s %s %d \r\n",log_buf->text, log_buf->file, log_buf->line);
        sprintf(text,"%s %s %d \r\n",log_buf->text, log_buf->file, log_buf->line); 
        log_send(text);
        return; 

    case RN_LOG_ERR_MSG:
        //printf("%s Line %d \r\n",log_buf->file,log_buf->line);
        sprintf(text,"%s Line %d \r\n",log_buf->file,log_buf->line);   
        log_send(text);

        //printf("%-14s",log_buf->text);
        sprintf(text,"%-14s",log_buf->text);
        log_send(text);
        break;

    case RN_LOG_TEXT:          
        //printf("%s \r\n",log_buf->text);
        sprintf(text,"%s \r\n",log_buf->text);
        log_send(text);
        return; 

    case  RN_LOG_VTEXT:
        //printf("%s \r\n",log_buf->data); 
        sprintf(text,"%s \r\n",log_buf->data);
        log_send(text);
        return; 
   
    case RN_LOG_MSG_TEXT:       
        //printf("%-14s",log_buf->text);
        sprintf(text,"%-14s",log_buf->text);
        log_send(text);
        break;

    case RN_LOG_MSG_ID_SEND:

        //printf(      "S %5s -> %5s",string_for_log_id(log_buf->log_id),string_for_pd(log_buf->pd));
        sprintf(text,"S %5s -> %5s",string_for_log_id(log_buf->log_id),string_for_pd(log_buf->pd));
        log_send(text);

        break;
        
    case RN_LOG_MSG_ID_RECEIVE:

        //printf(      "R %5s -> %5s",string_for_pd(log_buf->pd),string_for_log_id(log_buf->log_id));
        sprintf(text,"R %5s -> %5s",string_for_pd(log_buf->pd),string_for_log_id(log_buf->log_id));
        //            ----------
        log_send(text);
        break;

    case RN_LOG_MSG_RAB:

        if(log_buf->pd == PD_RCCR)
        {
            switch(log_buf->mt)
            {
            case ACC_UDATA:  
                //printf(      "  %4s%d -> %4s%d","R",log_buf->rn_sender,"R",log_buf->rn_receiver);
                sprintf(text,"  %4s%d -> %4s%d","R",log_buf->rn_sender,"R",log_buf->rn_receiver);
                //            ------------
                log_send(text);
                break;

            case BCC_UDATA:
                //printf("  %4s%d -> %5s","R",log_buf->rn_sender,"BC");
                sprintf(text,"  %4s%d -> %5s","R",log_buf->rn_sender,"BC");
                log_send(text);
                break;

            case RCC_UDATA:
                //printf("  %5s -> %5s","S","M");
                //      ----------
                sprintf(text,"  %5s -> %5s","S","M");
                log_send(text);
                break;
            }
        } 
        else if(log_buf->pd == PD_RCCA)
        {
            switch(log_buf->mt)
            {
            case ACC_UDATA:
                //printf("  %4s%d -> %4s%d","A",log_buf->rn_sender,"A",log_buf->rn_receiver);
                sprintf(text,"  %4s%d -> %4s%d","A",log_buf->rn_sender,"A",log_buf->rn_receiver);
                log_send(text);
                break;

            case BCC_UDATA:
                //printf("  %4s%d -> %5s","A",log_buf->rn_sender,"BC");
                sprintf(text,"  %4s%d -> %5s","A",log_buf->rn_sender,"BC");
                log_send(text);
                break;
            }
        }
        else
        {
            //printf(">>>>>> Error? %5s -> %5s-",string_for_log_id(log_buf->log_id),string_for_pd(log_buf->pd));
            sprintf(text,">>>>>> Error? %5s -> %5s-",string_for_log_id(log_buf->log_id),string_for_pd(log_buf->pd));
            log_send(text);
        }
        break;

   

    default: 
        //printf("Log format %d Undecoded \r\n",log_buf->log_format);
        sprintf(text,"Log format %d Undecoded \r\n",log_buf->log_format);
        log_send(text);
        return;
    }

    if(log_buf->pd == PD_RCCR)
    {
        //printf(" %-20s (%2d) ",string_for_mt(PD_RRC, log_buf->data[0]), log_buf->size);
        sprintf(text," %-20s (%2d) ",string_for_mt(PD_RRC, log_buf->data[0]), log_buf->size);
        log_send(text);
    }
    else  if(log_buf->pd == PD_CCI)
    {
        //printf(" %-20s (%2d) ",string_for_mt(PD_CCI, log_buf->data[0]), log_buf->size);
        sprintf(text," %-20s (%2d) ",string_for_mt(PD_CCI, log_buf->data[0]), log_buf->size);
        log_send(text);
    }
    else
    {
        //printf(" %-20s (%2d) ",string_for_mt(log_buf->pd, log_buf->mt), log_buf->size);
        sprintf(text," %-20s (%2d) ",string_for_mt(log_buf->pd, log_buf->mt), log_buf->size);
        log_send(text);
    }
    sprintf(text,"");
    while(index < log_buf->size)
    {
        if (index > 128)
        {
            return;
        }
        sprintf(byte_text,"%02X ",log_buf->data[index]);
        strcat(text,byte_text);
        //printf("%02X ",log_buf->data[index]);
        //console_send(text);
        index++;
    }

    //printf("\r\n");
    strcat(text,"\r\n");
    log_send(text);
}
/******************************************************************************
* Function Name    : log_buf_copy
* Function Purpose : Take copy of message for logging 
* Input parameters : Pointer to message                  
* Return value     : Pointer to log message
******************************************************************************/
static log_buf_t * log_buf_copy(msg_buf_t * msg_buf)
{
    log_buf_t * log_buf = log_buf_alloc(msg_buf->size);

    log_buf->mt            = msg_buf->mt;
    log_buf->pd            = msg_buf->pd;
    log_buf->rn_receiver   = msg_buf->rn_receiver;
    log_buf->rn_sender     = msg_buf->rn_sender;

    memcpy(log_buf->data,msg_buf->data,msg_buf->size);

    return log_buf;
}

/******************************************************************************
* Function Name    : log_buf_alloc
* Function Purpose : Alloc log message buffer 
* Input parameters : size             
* Return value     : Pointer to log message
******************************************************************************/
static log_buf_t * log_buf_alloc(size_t size)
{
    log_buf_t * log_buf = NULL;

    log_buf             = (log_buf_t *)RTOS_ALLOC(size + sizeof(log_buf_t));

    if(log_buf)
    {
        log_buf->data           = (uint8_t *) log_buf + sizeof(log_buf_t);
        log_buf->size           = size;
        log_buf->rn_receiver    = 0xFF;
        log_buf->rn_sender      = 0xFF;
        log_buf->pd             = 0xFF;
        log_buf->tick           = 0;
    }

    return log_buf;
}

/*
-----------------------------------------------------------
TEST COMMAND FUNCTIONS
-------------------------------------------------------------
*/

/******************************************************************************
* Function Name    : rn_log_mon_all
* Function Purpose : Set log control for all protocols
* Input parameters : control = false, no logging
*                    control = true, log all protocols (may cause overflow)
* Return value     : void
******************************************************************************/
static void rn_log_mon_all(bool control)
{
    uint32_t index;

    for (index=0;index<256;index++)
    {
        pd_filter[index] = !control; 
    }
}

/******************************************************************************
* Function Name    : rn_log_mon_rab
* Function Purpose : Activate/Deactivale monitoring of RAB messages
* Input parameters : control = false, no logging
*                    control = true, logging
* Return value     : void
******************************************************************************/
//static void rn_log_mon_rab(bool control)
//{
//     pd_filter[PD_RCCA] = !control;
//     pd_filter[PD_RCCR] = !control;
//}

/******************************************************************************
* Function Name    : rn_log_mon_switch_pd
* Function Purpose : Activate/Deactivale monitoring for specified PD
* Input parameters : pd Protocol Discriminator
*                    control = false, no logging
*                    control = true, logging
* Return value     : void
******************************************************************************/
static void rn_log_mon_switch_pd(uint8_t pd, bool control)
{
     pd_filter[pd] = !control; 
}
/*-----------------------------------------------------------*/
//static void rn_log_mon_lcc(bool control)
//{
//    pd_filter[PD_LACCR] = !control;
//    pd_filter[PD_LACCA] = !control;
//    pd_filter[PD_LBCCR] = !control;
//    pd_filter[PD_LBCCA] = !control;
//}
/*-----------------------------------------------------------*/
//static void rn_log_mon_rpc(bool control)
//{
//     pd_filter[PD_RPI] = !control;
//}
/*-----------------------------------------------------------*/
//static void rn_log_mon_rrc(bool control)
//{
//     pd_filter[PD_RRC] = !control;
//}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void rn_log_pd_list()
{
    uint32_t    index;
    bool        no_on = true;

    sprintf(help_text,"Checking Switched ON monitors \r\n"); console_write(help_text);

    for (index=0;index<256;index++)
    {
        if (!pd_filter[index])
        {
            //printf("%s \r\n",string_for_pd_ext((uint8_t)index));
            sprintf(help_text,"%s \r\n",string_for_pd_ext((uint8_t)index));
            console_write(help_text);
            no_on = false;
        }
    }

    if(no_on)
    {
        sprintf(help_text,"No switched ON monitors \r\n"); console_write(help_text);
    }
}

static void rn_log_help(void)
{
    sprintf(help_text,"Command Format: rn-log parameter \r\n\r\n");                                 console_write( help_text);
    sprintf(help_text,"Parameter Meaning\r\n");                                                     console_write( help_text);
    sprintf(help_text,"--------- -------\r\n");                                                     console_write( help_text);
    sprintf(help_text,"c-mon     Control Interfaces Monitor ON, interfaces RNI, PLI, RPI \r\n");    console_write(help_text);
    sprintf(help_text,"c-moff    Control Interfaces Monitor OFF \r\n");                             console_write(help_text);
    
    sprintf(help_text,"r-mon     RRC Interface Monitor ON \r\n");                                   console_write(help_text);
    sprintf(help_text,"r-moff    RRC Interface Monitor OFF \r\n");                                  console_write(help_text);
    
    sprintf(help_text,"a-mon     APP Interface Monitor ON \r\n");                                   console_write(help_text);
    sprintf(help_text,"a-moff    APP Interface Monitor OFF \r\n");                                  console_write(help_text);

    sprintf(help_text,"l-mon     Logical Interface Monitor ON \r\n");                               console_write(help_text);
    sprintf(help_text,"l-moff    Logical Interface Monitor OFF \r\n");                              console_write(help_text);
  
    sprintf(help_text,"moff      All Interface Monitors OFF \r\n");                                 console_write(help_text);
    sprintf(help_text,"l         List Interface Monitors which are ON \r\n");                       console_write(help_text);
}
/* ----------------------------------------------------------- */
static portBASE_TYPE rn_log_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

    int8_t                  *pcParameter;
    portBASE_TYPE           lParameterStringLength, xReturn;
    static portBASE_TYPE    lParameterNumber = 0;

    ( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

    if( lParameterNumber == 0 )
	{
		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;

        memset( pcWriteBuffer, 0x00, xWriteBufferLen );
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
									( 
										pcCommandString,		/* The command string itself. */
										lParameterNumber,		/* Return the next parameter. */
										&lParameterStringLength	/* Store the parameter string length. */
									);

		if( pcParameter != NULL )
		{
			/* Return the parameter string. */
			memset( pcWriteBuffer, 0x00, xWriteBufferLen );

			/*sprintf( ( char * ) pcWriteBuffer, "%d: ", lParameterNumber );
			strncat( ( char * ) pcWriteBuffer, ( const char * ) pcParameter, lParameterStringLength );
			strncat( ( char * ) pcWriteBuffer, "\r\n", strlen( "\r\n" ) );*/

            //printf("%s received \r\n", pcParameter);

            if (0 == strcmp((const char *)pcParameter,"c-mon"))
            {

                rn_log_mon_switch_pd(PD_PLC,true);
                rn_log_mon_switch_pd(PD_RPI,true);
                rn_log_mon_switch_pd(PD_RNI,true);

                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"c-moff"))
            {

                rn_log_mon_switch_pd(PD_PLC,false);
                rn_log_mon_switch_pd(PD_RPI,false);
                rn_log_mon_switch_pd(PD_RNI,false);

                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"moff"))
            {
                rn_log_mon_all(false);
                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"a-mon"))
            {
                rn_log_mon_switch_pd(PD_RCCA,true);
                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"a-moff"))
            {
                rn_log_mon_switch_pd(PD_RCCA,false);
                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"r-mon"))
            {
                rn_log_mon_switch_pd(PD_RCCR,true);
                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"r-moff"))
            {
                rn_log_mon_switch_pd(PD_RCCR,false);
                rn_log_pd_list();
            }
            else if (0 == strcmp((const char *)pcParameter,"t-mon"))
            {
                 rn_log_mon_switch_pd(PD_TCCR,true);
                
                rn_log_pd_list();
            }
          
            else if (0 == strcmp((const char *)pcParameter,"b"))
            {
                //printf("buf_counter = %d \r\n",buf_counter);
                rn_msg_print();
            }
            else if (0 == strcmp((const char *)pcParameter,"l"))
            {
                rn_log_pd_list();
            } 
            else if (0 == strcmp((const char *)pcParameter,"l-mon"))
            {
                rn_log_mon_switch_pd(PD_LACCR,true);
                rn_log_mon_switch_pd(PD_LACCA,true);
                rn_log_mon_switch_pd(PD_LBCCR,true);
                rn_log_mon_switch_pd(PD_LBCCA,true);

            }
            else if (0 == strcmp((const char *)pcParameter,"l-moff"))
            {
                rn_log_mon_switch_pd(PD_LACCR,false);
                rn_log_mon_switch_pd(PD_LACCA,false);
                rn_log_mon_switch_pd(PD_LBCCR,false);
                rn_log_mon_switch_pd(PD_LBCCA,false);
            }
            else if (0 == strcmp((const char *)pcParameter,"?"))
            {
                rn_log_help();
            }
            else
            {
                rn_log_help();
            }

            
			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
            /* No more data to return. */
			//xReturn = pdFALSE;

			lParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[ 0 ] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			lParameterNumber = 0;
		}
	}
  
	return xReturn;
}
