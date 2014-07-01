/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rni.h
* Author            : Per Erik Sundvisson
* Description       : Declaration of application interface for the Radio Network
*                     See 1450-SDD-003 Radio Network Software API for background
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/
#ifndef RNI_H
#define RNI_H

#include "rtos.h"           // RTOS dependencies
#include "rn_manager.h"     // RN Manager internal interface definitions

// Protocol Discriminators for message based part of interface
// Makes it possible to identify protocol if messages are received in a common queue
// Can be retreived through API

#define PD_RNI      (0x01)   // Radio Network Interface
#define PD_CCI      (0x02)   // Control Channel Interface
#define PD_TCI      (0x03)   // Traffic Channel Interface

// RN address definitions

#define RN_MAX_NO_OF_SLAVES         (8)

#define RN_MIN_ADDRESS              (0)
#define RN_MAX_ADDRESS              (RN_MAX_NO_OF_SLAVES)

#define RN_MASTER_ADDRESS           (0)

#define RN_MIN_SLAVE_ADDRESS        (RN_MASTER_ADDRESS+1)
#define RN_MAX_SLAVE_ADDRESS        (RN_MAX_ADDRESS)

#define RN_ADDRESS_INVALID          (0xFF)
#define RN_INVALID_ADDRESS          (RN_ADDRESS_INVALID)

// Equipment ID (EQMID) shall be unique among all units in the system
// The following value is forbidden, it is used to indicate invalid EQMID.

#define RN_INVALID_EQMID            (0)
#define RN_EQMID_INVALID            RN_INVALID_EQMID

// In current release EQMID is limited to the following range

#define RN_EQMID_MAX                (65535)
#define RN_EQMID_MIN                (1)

// And for RNID:

#define RN_INVALID_RNID             (0xFF)
#define RN_RNID_INVALID             RN_INVALID_RNID

/******************************************************************************
*   Result codes returned by different RN interface functions
******************************************************************************/
typedef enum 
{
	RN_OK   		                    = 0,
	RN_ORDERED                          = 1,	
	RN_FAILURE                          = 2,
	RN_VALUE_OUT_OF_RANGE               = 3,
	RN_ILLEGAL_QUEUE_REFERENCE          = 4,
    RN_SW_START_MAYBE_NOT_EXECUTED      = 5,
	RN_NETWORK_ALREADY_STARTED          = 6,
    RN_ADDRESS_OUT_OF_RANGE             = 7,
    RN_NULL_POINTER_RECEIVED            = 8,
    RN_MAX_DATA_SIZE_EXCEEDED           = 9,
    RN_MESSAGE_RECEIVER_NOT_REGISTERED  = 10,
    RN_PHY_REPORTED_FAILURE             = 11,
    RN_PD_NOT_SUPPORTED                 = 12,
    RN_RNID_OUT_OF_RANGE                = 13,
    RN_EQMID_OUT_OF_RANGE               = 14,
    RN_NUMBER_OF_CODES 
} rn_result_t;

/*----------------------------------------------------------------------------
	RADIO NETWORK INTERFACE (RNI)								
------------------------------------------------------------------------------*/

/******************************************************************************
Message Types delivered over PD_RNI
******************************************************************************/

#define RNI_CONNECTED                   0x01
#define RNI_DISCONNECTED                0x02
#define RNI_CONNECTED_LIST              0x03
#define RNI_STARTED                     0x04
#define RNI_QUAL_POOR                   0x05
#define RNI_QUAL_OK                     0x06

// Parameter positions
#define RNI_CONNECTED_EQMID_POS         (0)     // Four bytes, MSB first
#define RNI_CONNECTED_ADDR_POS          (4)     // One byte

#define RNI_CONNECTED_LIST_NADDR_POS    (0)     // One byte, addresses follow
// Alloc sizes
#define RNI_CONNECTED_SIZE              (6)     
// Internal RNI message types start here:
#define RNI_FIRST_INTERNAL_MESSAGE_TYPE 0x10 

/******************************************************************************
* Function Name    : rni_register_message_receiver
* Function Purpose : Register RNI message receiver
* Input parameters : queue ID
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rni_register_message_receiver	(MessageQ_QueueId queue);

/******************************************************************************
* Function Name    : rni_start
* Function Purpose : Start Radio Network
* Input parameters : rnid, Radio network Identity to start
*                    eqmid, Own Equipment ID
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rni_start(uint8_t rnid, uint32_t eqmid);

/******************************************************************************
* Function Name    : rni_connect
* Function Purpose : Connect to Radio Network
* Input parameters : rnid, Radio network Identity to connect to
*                    eqmid, Own unique Equipment ID
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rni_connect(uint8_t  rnid, uint32_t eqmid);

/******************************************************************************
* Function Name    : rni_sniff
* Function Purpose : Sniff Radio Network
* Input parameters : rnid, Radio network Identity
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rni_sniff(uint8_t  rnid);

/******************************************************************************
* Function Name    : rni_undefine
* Function Purpose : Undefine unit
* Input parameters : void
* Return value     : See rn_result_t
* FFS: It is assumed that this function will replace rni_disconnect and rni_stop after 
* discussion, it is so far equivalent to rni_disconnect and rni-stop
******************************************************************************/
rn_result_t rni_undefine(void);

/******************************************************************************
* Function Name    : rni_tc_id_range_get
* Function Purpose : Get Traffic Channel Range
* Output parameters: first_tc_id    First TC ID in range
                     last_tc_id     Last  TC ID in range
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t rni_tc_id_range_get(uint8_t *first_tc_id, uint8_t *last_tc_id);

/*------------------------------------------------------------------
	CONTROL CHANNEL INTERFACE (CCI)								
--------------------------------------------------------------------*/
/******************************************************************************
*   Message Types delivered over PD_CCI
******************************************************************************/
#define ACC_UDATA   (0x01)      // Addressed Unacknowledged data
#define BCC_UDATA   (0x02)      // Broadcast Unacknowledged data

/******************************************************************************
*   Maximum allowed number of data bytes in CCI messages
******************************************************************************/
#define RN_MAX_MESSAGE_DATA         (20)

/******************************************************************************
* Function Name    : cci_register_message_receiver
* Function Purpose : Register CCI message receiver
* Input parameter  : queue ID
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_register_message_receiver(MessageQ_QueueId queue);

/******************************************************************************
* Function Name    : cci_send
* Function Purpose : Send message data to Application in specified RN address 
* Input parameter  : rn_receiver_address
                     msg, message containing data
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_send(uint8_t rn_receiver_address, msg_buf_t *msg);

/******************************************************************************
* Function Name    : cci_broadcast
* Function Purpose : Broadcast message data to Applications in all connected units  
* Input parameter  : msg, message containing data
* Return value     : See rn_result_t
******************************************************************************/
rn_result_t cci_broadcast(msg_buf_t *msg);

/*----------------------------------------------------------------------------
	TRAFFIC CHANNEL INTERFACE								
------------------------------------------------------------------------------*/

/******************************************************************************
*   Compilation constants for PD_TCI
******************************************************************************/

#define TCI_MIN_CHANNEL_NUMBER      (4)
#define TCI_MAX_CHANNEL_NUMBER      (15)

/******************************************************************************
* Function Name   : tci_activate_send_channel
* Function Purpose: Activate specified channel for sending
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_activate_send_channel(uint8_t tc);

/******************************************************************************
* Function Name   : tci_activate_receive_channel
* Function Purpose: Activate specified channel for reception
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_activate_receive_channel(uint8_t channel);

/******************************************************************************
* Function Name   : tci_deactivate_channel
* Function Purpose: Dectivate specified channel
* Input parameter : Traffic Channel ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t tci_deactivate_channel(uint8_t channel);
/******************************************************************************
* Function Name   : tci_voice
* Function Purpose: Provide pointers to audio I/O
* I/O parameters  : tci_channel_t, array of pointers
* Return value    : See rn_result_t
* This function shall be called each 4 ms and will deliver pointers to 
* audio buffers
*******************************************************************************/
typedef struct
{
    short *channel[12];
}
tci_channel_t;

void tci_voice(tci_channel_t *tci_channel);

/*----------------------------------------------------------------------------
RN message handling
RN messages shall be allocated, freed, and accessed through the following 
set of functions
------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name   : msg_buf_alloc
* Function Purpose: Allocate message to be used over RN interface
* Input parameters: Requested size of data field
*                   Overhead will be added for internal RN use, requested
*                   data size should cover max size required by caller
* Return value     : Pointer to message, NULL if failure
******************************************************************************/
//msg_buf_t   *msg_buf_alloc(size_t data_size);      // USE THIS FUNCTION CALL, IT WILL BE EXPANDED DURING DEBUGGING
msg_buf_t   *msg_buf_alloc_dbg(size_t data_size, char *file, int line);

/******************************************************************************
* Function Name   : msg_buf_free
* Function Purpose: Free a message received over RN interface
* Input parameters: Pointer to message
* Return value    : FFS
* FIXME: error handling pending
******************************************************************************/
void        msg_buf_free(msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_return_data_pos
* Function Purpose: Gives direct access to data field of message
                    Can be used to set/get data bytes directly
* Input parameters: Pointer to message
* Return value    : Pointer to data
******************************************************************************/
uint8_t     *msg_return_data_pos    (msg_buf_t * msg_buf);  

/******************************************************************************
* Function Name   : msg_return_data_size
* Function Purpose: Returns size, in bytes, of data field in message         
* Input parameters: Pointer to message
* Return value    : Data size
******************************************************************************/
uint32_t    msg_return_data_size    (msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_set_data_size
* Function Purpose: Sets size, in bytes, of data field in message         
* Input parameters: Pointer to message, Requested size
*                   Requested size must not exceed allocated size
* Return value    : true if accepted, false otherwise
* FIXME: error handling pending
******************************************************************************/
bool        msg_set_data_size       (msg_buf_t * msg_buf, uint32_t  size); 
/******************************************************************************
* Function Name   : msg_return_pd
* Function Purpose: Return Protocol Discriminator         
* Input parameters: Pointer to message
* Return value    : PD 
******************************************************************************/
uint8_t    msg_return_pd            (msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_put_pd
* Function Purpose: Put Protocol Discriminator         
* Input parameters: PD
*                   Pointer to message
* Return value    : bool 
******************************************************************************/
bool    msg_put_pd            (uint8_t pd, msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_return_mt
* Function Purpose: Return Message Type         
* Input parameters: Pointer to message
* Return value    : MT
******************************************************************************/
uint8_t    msg_return_mt            (msg_buf_t * msg_buf);
/******************************************************************************
* Function Name   : msg_put_mt
* Function Purpose: Put Protocol Discriminator         
* Input parameters: MT
*                   Pointer to message
* Return value    : bool 
******************************************************************************/
bool    msg_put_mt            (uint8_t mt, msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_return_rn_sender
* Function Purpose: Return Sender of message         
* Input parameters: Pointer to message
* Return value    : RN address of sender
******************************************************************************/
uint8_t     msg_return_rn_sender     (msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_return_rn_receiver
* Function Purpose: Return receiverf of message         
* Input parameters: Pointer to message
* Return value    : RN address of receiver
* FFS: Maybe only for RN internal use
******************************************************************************/
uint8_t     msg_return_rn_receiver   (msg_buf_t * msg_buf);

/******************************************************************************
* Function Name   : msg_buf_copy
* Function Purpose: Create copy of message in new allocated buffer     
* Input parameters: Pointer to original message
* Return value    : Pointer to message copy
******************************************************************************/
//msg_buf_t * msg_buf_copy        (msg_buf_t * msg_buf); // USE THIS FUNCTION CALL, IT WILL BE EXPANDED DURING DEBUGGING
msg_buf_t * msg_buf_copy_dbg    (msg_buf_t * msg_buf, char *file, int line);

/******************************************************************************
* Function Name   : msg_return_uintX
* Function Purpose: return value of type uintX_t  from data field, starting on 
*                   byte index  
* Input parameters: Pointer to message, index to position of value
* Return value    : uintX value
******************************************************************************/
uint8_t     msg_return_uint8        (msg_buf_t * msg_buf, uint32_t index);
uint16_t    msg_return_uint16       (msg_buf_t * msg_buf, uint32_t index);
uint32_t    msg_return_uint32       (msg_buf_t * msg_buf, uint32_t index);

/******************************************************************************
* Function Name   : msg_return_uintX_i
* Function Purpose: return value of type uintX_t from data field, starting on 
*                   byte index  
* Input parameters: Pointer to message, pointer to index to position
* Output parameter: Index to next position after returned value
* Return value    : uintX value
******************************************************************************/
uint8_t     msg_return_uint8_i      (msg_buf_t * msg_buf, uint32_t *index);             
uint16_t    msg_return_uint16_i     (msg_buf_t * msg_buf, uint32_t *index);
uint32_t    msg_return_uint32_i     (msg_buf_t * msg_buf, uint32_t *index);

/******************************************************************************
* Function Name   : msg_put_uintX
* Function Purpose: Puts value of type uintX_t in data field, starting on byte index  
* Input parameters: Pointer to message, index to position, value of type uintX_t
* Return value    : Index to next position after put value
******************************************************************************/
uint32_t    msg_put_uint8           (msg_buf_t * msg_buf, uint32_t index, uint8_t value);
uint32_t    msg_put_uint16          (msg_buf_t * msg_buf, uint32_t index, uint16_t value);
uint32_t    msg_put_uint32          (msg_buf_t * msg_buf, uint32_t index, uint32_t value);

/******************************************************************************
* Function Name   : msg_put_uintX_i
* Function Purpose: Puts value of type uintX_t in data field, starting on byte index  
* Input parameters: Pointer to message, index to position, value of type uintX_t
* Output parameter: Index to next position after put value
* Return value    : Index to next position after put value
******************************************************************************/
uint32_t    msg_put_uint8_i         (msg_buf_t * msg_buf, uint32_t *index, uint8_t value);
uint32_t    msg_put_uint16_i        (msg_buf_t * msg_buf, uint32_t *index, uint16_t value);
uint32_t    msg_put_uint32_i        (msg_buf_t * msg_buf, uint32_t *index, uint32_t value);

/******************************************************************************
* Function Name   : msg_buf_header_remove
* Function Purpose: Removes data from the header  
* Input parameters: Pointer to message, number of bytes to remove
* Output parameter: none
* Return value    : bool, true if successful
******************************************************************************/

bool        msg_buf_header_remove   (msg_buf_t * msg_buf, uint32_t n_bytes);
/******************************************************************************
* Function Name   : msg_buf_header_add
* Function Purpose: Adds data positions to the header  
* Input parameters: Pointer to message, number of bytes to add
* Output parameter: none
* Return value    : bool, true if successful
******************************************************************************/

bool        msg_buf_header_add      (msg_buf_t * msg_buf, uint32_t n_bytes);  

/*----------------------------------------------------------------------------
Test support
------------------------------------------------------------------------------*/
/******************************************************************************
* Function Name   : rn_result_text
* Function Purpose: Provides text for rn_result_t values, for printouts  
* Input parameters: result
* Output parameter: none
* Return value    : Pointer to text
******************************************************************************/
const char *rn_result_text(rn_result_t result);

typedef char * string_for_mt_cb_t (uint8_t mt); // Callback Prototype for mt to string translation

/******************************************************************************
* Function Name    : cci_mt_translator_reg
* Function Purpose : Registration of callback for CCI air interface message type 
*                    to log text translation. First byte in CCI messages is 
*                    interpreted as MT
* Input parameters : Function of type string_for_mt_cb_t
* Return value     : none
******************************************************************************/
typedef char * string_for_mt_cb_t (uint8_t mt); // Callback Prototype for mt to string translation

void cci_mt_translator_reg(string_for_mt_cb_t * translate_cb);

/******************************************************************************
* Function Name    : rni_sync_on
* Function Purpose : Request sync message ON
* Input parameters : void
* Return value     : rn_result_t
******************************************************************************/
rn_result_t rni_sync_on(void);

/******************************************************************************
* Function Name    : rni_sync_off
* Function Purpose : Request sync message OFF
* Input parameters : void
* Return value     : rn_result_t
******************************************************************************/
rn_result_t rni_sync_off(void);

/******************************************************************************
* Function Name    : rni_console_write
* Function Purpose : Write text on remote console
* Input parameters : RN address
*                    text string, max 18 char, longer texts will be terminated by NULL
* Return value     : void
******************************************************************************/
void rni_console_write(uint8_t addr,char *text);

/*----------------------------------------------------------------------------
To be moved to RN internal file
------------------------------------------------------------------------------*/

/******************************************************************************
*   Messages delivered over PD_TCI
******************************************************************************/

// Message types

#define TCI_READY_TO_SEND (0x01)      // Request to send data
#define TCI_DATA          (0x02)      // Rerceived Data or send data
#define TCI_EMPTY         (0x03)      // Empty means no Data to send

// Parameter positions

#define TCI_CHANNEL_POS     (0)     // TCI Channel number One byte
#define TCI_FN_POS          (1)     // Frame Number, two bytes, MSB first
#define TCI_DATA_POS        (3)     // Data to/From air interface, TCI_AIR_DATA_SIZE bytes

#define TCI_HEADER_SIZE     (3)

#define TCI_AIR_DATA_SIZE   (95)    // Max Number of data bytes to be tranferred over air
#define TCI_DATA_SIZE       (TCI_HEADER_SIZE + TCI_AIR_DATA_SIZE)

/******************************************************************************
* Function Name    : tci_send_queue_get
* Function Purpose : return ID for queue provided by RN for sending of TC data
* Input parameter  : None
* Return value     : Queue ID
*******************************************************************************/
MessageQ_QueueId tci_send_queue_get(void);

/******************************************************************************
* Function Name   : tci_register_sender
* Function Purpose: A sender queue provided by AP shall be registered with 
*                   RN before activation of any TC send channel. 
*                   It is used by RN to request data to send for activated 
*                   send channels.
* Input parameter : Sender queue ID
* Return value    : See rn_result_t
*******************************************************************************/
rn_result_t	tci_register_sender(MessageQ_QueueId ap_tc_sender_queue);


/******************************************************************************
* Function Name   : tci_register_receive_queue
* Function Purpose: Register TC receive queue
*                   This queue is provided by AP and shall be registered 
*                   with RN before activation of any TC receive channel. 
*                   RN will put received data on this queue for activated 
*                   receive channels.
* Input parameter : Receiver queue ID
* Return value    : See rn_result_t
--------------------------------------------------------------------*/
rn_result_t tci_register_receive_queue(MessageQ_QueueId ap_tc_receive_queue);
#endif /* RNI_H */



