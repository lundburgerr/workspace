/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_log.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of rn_log module external inferface
********************************************************************************
* Revision History
* 2013-03-20        : First Issue
*******************************************************************************/

#ifndef RN_LOG_H
#define RN_LOG_H

// Own address, used for console write routing in some cases

#define RN_ADDRESS_OWN              (0xFE)
#define RN_OWN_ADDRESS              (RN_ADDRESS_OWN)

typedef enum
{
    RN_APP,
    RN_CMD,
    RN_RNM,
    RN_RAB,         // Only for logging of RAB messages
    RN_RLC,
    RN_MAC,
    RN_PHY
} rn_log_id_t;

/******************************************************************************
* Function Name    : rn_log_module_init
* Function Purpose : Init mopdule
* Input parameters : none
* Return value     : none
******************************************************************************/
void rn_log_module_init(void);

/******************************************************************************
* Function Name    : rn_log_module_start
* Function Purpose : Init mopdule
* Input parameters : Start Phase
* Return value     : none
******************************************************************************/
void rn_log_module_start(uint8_t start_phase);

/******************************************************************************
* Function Name    : rn_log_err_msg(const char * text,   msg_buf_t *msg_buf)
* Function Purpose : Logg error condition for specified message
*                    Call is a macro which will expand into
*                    rn_log_err_msg_plus    (const char * tag,   msg_buf_t *msg_buf, const char * file, int line);
*                    File path and line no will be added and printed
* Input parameters : Constant text, message pointer
* Return value     : none
******************************************************************************/
//void    rn_log_err_msg        (const char * tag,   msg_buf_t *msg_buf);
#define rn_log_err_msg(a,b)     rn_log_err_msg_plus(a,b,__FILE__,__LINE__)
void    rn_log_err_msg_plus     (const char * tag,   msg_buf_t *msg_buf, const char * file, int line);

/******************************************************************************
* Function Name    : rn_log_err(const char * text)
* Function Purpose : Logg error condition
*                    Call is a macro which will expand into
*                    rn_log_err_plus    (const char * text, const char * file, int line);
*                    File path and line no will be added and printed
* Input parameters : Constant text, message pointer
*                    Only pointer to text will be copied, it can not be variable
* Return value     : none
******************************************************************************/
//void rn_log_err    (const char * tag);
#define rn_log_err(a)   rn_log_err_plus(a,__FILE__,__LINE__)
void rn_log_err_plus    (const char * tag,  const char * file, int line);

/******************************************************************************
* Function Name   : rn_log_vtext
* Function Purpose: Logg variable text
* Input parameters: Text which will be copied, it can be modified with for example
*                   sprintf
* Return value    : none
******************************************************************************/
void rn_log_vtext   (char * text);

/******************************************************************************
* Function Name   : rn_log_text
* Function Purpose: Logg text
* Input parameters: Constant text. Only pointer to text will be copied, 
*                   it can not be variable
* Return value    : none
******************************************************************************/
void rn_log_text            (const char * text);

/******************************************************************************
* Function Name   : rn_log_text_msg
* Function Purpose: Logg text and message
* Input parameters: Constant text. Only pointer to text will be copied, 
*                   it can not be variable.
*                   Message will be copied.
* Return value    : none
******************************************************************************/
void rn_log_text_msg        (const char * text, msg_buf_t *msg_buf);
/******************************************************************************
* Function Name   : rn_log_msg
* Function Purpose: Logg message
* Input parameters: Log id, message pointer
* Return value    : none
*
* FFS: Formats not settled yet
******************************************************************************/

typedef enum
{
    RN_LOG_ERR_MSG = 1,
    RN_LOG_MSG_TEXT,
    RN_LOG_MSG_RAB,
    RN_LOG_MSG_ID_SEND,
    RN_LOG_MSG_ID_RECEIVE,
    RN_LOG_ERR,
    RN_LOG_TEXT,
    RN_LOG_VTEXT,
    RN_LOG_CONSOLE_WRITE
} rn_log_format_t;

typedef struct
{
    MessageQ_MsgHeader  header;         /* Required by IPC*/
    rn_log_format_t log_format;
    rn_log_id_t     log_id;
    uint8_t         pd;                 // Protocol Discriminator, defines interpretation of message
    uint8_t         mt;                 // Message Type
    uint8_t         rn_receiver;        // RN receiver address, when applicable
    uint8_t         rn_sender;          // RN sender address, when applicable

    const char*     text;               // Text from application, when applicable
    const char*     file;               // File from compiler, when applicable
    int             line;               // Line no from compiler, when applicable
    uint32_t        tick;               // Tick, for debug purpose
    uint32_t		size;               // Size of data
    uint8_t			*data;              // Pointer to where message data begins, when applicable
} log_buf_t;


void rn_log_msg             (rn_log_id_t id,    msg_buf_t *msg_buf);
void rn_log_msg_send        (rn_log_id_t id,    msg_buf_t *msg_buf);
void rn_log_msg_receive     (rn_log_id_t id,    msg_buf_t *msg_buf);
void log_msg_print          (log_buf_t *log_buf);
void rn_log_msg_print       (log_buf_t *log_buf);
RTOS_QUEUE_ID_T rn_log_queue_id(void);
#endif /* RN_LOG_H */



