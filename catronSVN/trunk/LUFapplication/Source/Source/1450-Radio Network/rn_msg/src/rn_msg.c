/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_msg.c
* Author            : Per Erik Sundvisson
* Description       : Definition of RN message handling
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/
/*-----------------------------------------------------------
Standard includes
-------------------------------------------------------------
*/

#include <stdint.h>
#include <stdlib.h>

/*-----------------------------------------------------------
Macros
-------------------------------------------------------------*/

/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/
#include "rtos.h"
/*
-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/
#include "rni.h"
#include "rn_log.h"
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
extern int32_t buf_counter = 0;

#define RN_MSG_LEAK_BUFFER_SIZE 50
static msg_buf_t *leak_buffer[RN_MSG_LEAK_BUFFER_SIZE+1];
/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/
void rn_msg_init(void)
{
    int i;

    for (i=0;i<=RN_MSG_LEAK_BUFFER_SIZE;i++)
    {
        leak_buffer[i] = NULL;
    }
}
void rn_msg_print(void)
{
    int i;

    for (i=0;i<=RN_MSG_LEAK_BUFFER_SIZE;i++)
    {
        if ( leak_buffer[i] != NULL)
        {
            printf("Line %d %s \n\r",leak_buffer[i]->line, leak_buffer[i]->file);
            rn_log_text_msg("In leak buffer",leak_buffer[i]);
        }
    }
}

/******************************************************************************
* Function Name   : msg_buf_alloc_dbg
* Function Purpose: Allocate message to be used over RN interface, debug version
* Input parameters: Requested size of data field
*                   Overhead will be added for internal RN use, requested
*                   data size should cover max size required by caller
* Return value     : Pointer to message, NULL if failure
******************************************************************************/
msg_buf_t * msg_buf_alloc_dbg(size_t size, char * file, int line)
{

#define MSG_BUF_FOOTROOM (8)
#define MSG_BUF_HEADROOM (8)
    int i;
    uint32_t        alloc_size;
    msg_buf_t *     msg_buf = NULL;

    alloc_size = size + sizeof(msg_buf_t) + MSG_BUF_HEADROOM + MSG_BUF_FOOTROOM;

    msg_buf             = (msg_buf_t *)RTOS_ALLOC(alloc_size);

    if(msg_buf)
    {
        msg_buf->data           = (uint8_t *) msg_buf + (sizeof(msg_buf_t) + MSG_BUF_HEADROOM);
        msg_buf->size           = size;
        msg_buf->alloc_size     = alloc_size;
        msg_buf->rn_receiver    = 0xFF;
        msg_buf->rn_sender      = 0xFF;
        msg_buf->pd             = 0xFF;
        msg_buf->file           = file;
        msg_buf->line           = line;
        buf_counter++;
    }
    else
    {
        // Failed to allocate memory
        // FIXME: Restart should be executed
    }

    for (i=0;i<=RN_MSG_LEAK_BUFFER_SIZE;i++)
    {
        if (leak_buffer[i] == NULL)         // FIXME: Should be protected, but this is for testing only
        {
            leak_buffer[i] = msg_buf;
            break;
        }
    }

    return msg_buf;
}
/******************************************************************************
* Function Name     : msg_buf_free
* Function Purpose  : Free message buffer
* Input parameters  : Pointer to buffer
* Return value      : void
******************************************************************************/
void msg_buf_free(msg_buf_t * msg_buf)
{
    int i;

    for (i=0;i<=RN_MSG_LEAK_BUFFER_SIZE;i++)
    {
        if (leak_buffer[i] == msg_buf)
        {
            leak_buffer[i] = NULL;
            break;
        }
    }
    RTOS_FREE((void *) msg_buf);
    buf_counter--;
}
/*
-------------------------------------------------------------
*/
bool msg_buf_header_remove(msg_buf_t * msg_buf, uint32_t n_bytes)
{ 
    // FIXME: Range check to be done
    msg_buf->data += n_bytes;
    msg_buf->size -= n_bytes;

    return true;
}
/*
-------------------------------------------------------------
*/
bool msg_buf_header_add(msg_buf_t * msg_buf, uint32_t n_bytes)
{ 
    // FIXME: Range check to be done
    msg_buf->data -= n_bytes;
    msg_buf->size += n_bytes;

    return true;
}
/*
-------------------------------------------------------------
*/
uint8_t msg_return_uint8(msg_buf_t * msg_buf, uint32_t index)
{
    uint8_t value = 0;

    // FIXME: Range check to be done
    value =             msg_buf->data[index];

    return value;
}
/*
-------------------------------------------------------------
*/

uint32_t msg_return_data_size(msg_buf_t * msg_buf)
{
    return msg_buf->size;
}
/*
-------------------------------------------------------------
*/
uint8_t    msg_return_pd            (msg_buf_t * msg_buf)
{
    // FIXME: Error handling pending
    return msg_buf->pd;
}
/*
-------------------------------------------------------------
*/

bool    msg_put_pd            (uint8_t pd, msg_buf_t * msg_buf)
{
    msg_buf->pd = pd;
    return true;
}

uint8_t    msg_return_mt            (msg_buf_t * msg_buf)
{
    // FIXME: Error handling pending
    return msg_buf->mt;
}

bool    msg_put_mt            (uint8_t mt, msg_buf_t * msg_buf)
{
    msg_buf->mt = mt;
    return true;
}
bool msg_set_data_size(msg_buf_t * msg_buf, uint32_t  size)
{
    // FIXME: Range check to be done

    msg_buf->size = size;

    return true;
}
/*
-------------------------------------------------------------
*/
uint8_t msg_return_rn_sender(msg_buf_t * msg_buf)
{
    return msg_buf->rn_sender;
}
/*
-------------------------------------------------------------
*/
uint8_t msg_return_rn_receiver(msg_buf_t * msg_buf)
{
    return msg_buf->rn_receiver;
}
/*
-------------------------------------------------------------
*/
msg_buf_t *msg_buf_copy_dbg(msg_buf_t *original, char * file, int line)
{
    msg_buf_t * copy = msg_buf_alloc_dbg(msg_return_data_size(original), file, line);

    copy->mt            = original->mt;
    copy->pd            = original->pd;
    copy->rn_receiver   = original->rn_receiver;
    copy->rn_sender     = original->rn_sender;

    memcpy(copy->data, original->data, original->size);

    return copy;
}
/*
-------------------------------------------------------------
*/
uint8_t *msg_return_data_pos(msg_buf_t * msg_buf)
{
    return msg_buf->data;
}
/*
-------------------------------------------------------------
*/
uint8_t msg_return_uint8_i(msg_buf_t * msg_buf, uint32_t *index)
{
    uint8_t     value = 0;
    uint32_t    lindex = *index;
     // FIXME: Range check to be done
    value =             msg_buf->data[lindex];
    
    lindex++;
    *index = lindex;
    return value;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint8(msg_buf_t * msg_buf, uint32_t index, uint8_t value)
{
     // FIXME: Range check to be done
    msg_buf->data[index]        = value;
   
    return index+1;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint8_i(msg_buf_t * msg_buf, uint32_t *index, uint8_t value)
{
    uint32_t    lindex = *index;
     // FIXME: Range check to be done

    msg_buf->data[lindex]        = value;

    lindex++;
    *index =lindex;

    return *index;
}
/*
-------------------------------------------------------------
*/
uint16_t msg_return_uint16(msg_buf_t * msg_buf, uint32_t index)
{
    uint16_t value = 0;
     // FIXME: Range check to be done
    value =             (uint16_t) (msg_buf->data[index]    << 8) & 0xFF00;
    value =  value +    (uint16_t) (msg_buf->data[index+1]  << 0) & 0xFFFF;
   
    return value;
}
/*
-------------------------------------------------------------
*/
uint16_t msg_return_uint16_i(msg_buf_t * msg_buf, uint32_t *index)
{
    uint16_t     value = 0;
    uint32_t    lindex = *index;
     // FIXME: Range check to be done
    value =             (uint16_t) (msg_buf->data[lindex]    << 8) & 0xFF00;
    value =  value +    (uint16_t) (msg_buf->data[lindex]  << 0) & 0xFFFF;
    
    lindex +=2;
    *index = lindex;

    return value;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint16(msg_buf_t * msg_buf, uint32_t index, uint16_t value)
{
     // FIXME: Range check to be done
    msg_buf->data[index]        = (uint8_t) (value >> 8) &0xFF;
    msg_buf->data[index+1]      = (uint8_t) (value >> 0) &0xFF;
     
    return index+2;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint16_i(msg_buf_t * msg_buf, uint32_t *index, uint16_t value)
{
    uint32_t    lindex = *index;
     // FIXME: Range check to be done
    msg_buf->data[lindex]        = (uint8_t) (value >> 8) &0xFF;
    msg_buf->data[lindex+1]      = (uint8_t) (value >> 0) &0xFF;
    
    lindex +=2;
    *index = lindex;

    return *index;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_return_uint32(msg_buf_t * msg_buf, uint32_t index)
{
    uint32_t value = 0;
     // FIXME: Range check to be done
    value =             (uint32_t) ( msg_buf->data[index]   << 24) & 0xFF000000;
    value =  value +    (uint32_t) ( msg_buf->data[index+1] << 16) & 0xFFFF0000;
    value =  value +    (uint32_t) ( msg_buf->data[index+2] <<  8) & 0xFFFFFF00;
    value =  value +    (uint32_t) ( msg_buf->data[index+3]);

    return value;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_return_uint32_i(msg_buf_t * msg_buf, uint32_t *index)
{
    uint32_t value = 0;
    uint32_t    lindex = *index;
     // FIXME: Range check to be done

    value =             (uint32_t) ( msg_buf->data[lindex]   << 24) & 0xFF000000;
    value =  value +    (uint32_t) ( msg_buf->data[lindex+1] << 16) & 0xFFFF0000;
    value =  value +    (uint32_t) ( msg_buf->data[lindex+2] <<  8) & 0xFFFFFF00;
    value =  value +    (uint32_t) ( msg_buf->data[lindex+3] <<  0) & 0xFFFFFFFF;
    
    lindex +=4;
    *index = lindex;

    return value;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint32(msg_buf_t * msg_buf, uint32_t index, uint32_t value)
{
     // FIXME: Range check to be done
    msg_buf->data[index]        = (uint8_t) (value >> 24)&0xFF;
    msg_buf->data[index+1]      = (uint8_t) (value >> 16)&0xFF;
    msg_buf->data[index+2]      = (uint8_t) (value >> 8) &0xFF;
    msg_buf->data[index+3]      = (uint8_t) (value >> 0) &0xFF;
        
    return index + 4;
}
/*
-------------------------------------------------------------
*/
uint32_t msg_put_uint32_i(msg_buf_t * msg_buf, uint32_t *index, uint32_t value)
{
    uint32_t    lindex = *index;
    lindex = msg_put_uint32(msg_buf, lindex, value);
 
    *index = lindex;    
    return lindex;
}
/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/

