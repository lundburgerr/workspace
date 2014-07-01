/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_mac_cc.c
* Author            : Per Erik Sundvisson
* Description       : Definition of RN MAC CC data and functions
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
#include <stdio.h>

/*
-----------------------------------------------------------
Macros
-------------------------------------------------------------
*/

#define RN_MAC_QUEUE_LENGTH     (10)

#define RN_MAC_CC_MIN_SLOT  (1)
#define RN_MAC_CC_MAX_SLOT  (3)

// Logical Channel Encoding

#define MAC_LSCCR_TAG       (0x00) // Bits from left: 00, Synchronization Channel
#define MAC_LACCR_TAG       (0x40) // Bits from left: 010, Addressed RN Control Channel
#define MAC_LACCA_TAG       (0x60) // Bits from left: 011, Addressed AP Control Channel 
#define MAC_UC_TAG          (0x80) // Bits : 1000 0000, AP User Channel
#define MAC_LBCCR_TAG       (0xC0) // Bits : 1100 0000, RN Broadcast Channel 
#define MAC_LBCCA_TAG       (0xE0) // Bits : 1110 0000, AP Broadcast Channel 
#define MAC_LRCCR_TAG       (0xC1) // Bits : 1100 0001, RN Registration Channel 
#define MAC_NULL_TAG        (0xDF) // Bits : 1101 1111, NULL Channel
#define MAC_FILL_TAG        (0xBB) // Bits : 1011 1011 (TBD)

// Logical Channel Decoding

#define MAC_2_TAG_FILTER        (0xC0) // Bits : 1100 0000, filters out Synch channel tag
#define MAC_3_TAG_FILTER        (0xE0) // Bits : 1110 0000, filters out Addressed Control Channel tags
#define MAC_RNID_FILTER         (0x3F) // Bits : 0011 1111, filters out rnid from SC tag
#define MAC_RN_RECEIVER_FILTER  (0x1F) // Bits : 0001 1111, filters out receiver address
#define MAC_RN_SENDER_FILTER    (0x1F) // Bits : 0001 1111, filters out sender address 
#define MAC_DATA_SIZE_FILTER    (0x1F) // Bits : 0001 1111, filters out message length

// (other tags are constant bytes as encoded above)

// Control Channel Transport slots

#define DCC_1_SLOT          (1)
#define UCC_2_SLOT          (2)
#define UCC_3_SLOT          (3)

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
#include "rn_mac_cc.h"
#include "rn_phy.h"
#include "rn_log.h"

/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
/***********************************************************
*   Timeslot data
************************************************************/
typedef enum
{
    MAC_CC_SLOT_IDLE = 1,           // Idle slot 
    MAC_CC_SLOT_DCC_SEND,           // DCC send slot 
    MAC_CC_SLOT_DCC_RECEIVE,        // DCC receive slot
    MAC_CC_SLOT_UCC_SEND,           // UCC send slot
    MAC_CC_SLOT_UCC_RECEIVE         // UCC receive slot
} mac_slot_conf_t;

typedef struct
{
    mac_slot_conf_t mac_slot_conf;

    uint32_t        tccr_stat_received_empty;
    uint32_t        tccr_stat_received_data;
    
    uint32_t        tccr_stat_received_rts;
    uint32_t        tccr_stat_sent_data; 
    uint32_t        tccr_stat_sent_empty;

    uint32_t        tccr_stat_more_than_sync;
    
    msg_buf_t       *interleave[3];
    msg_buf_t       *de_interleave[3];

} mac_slot_data_t;

//static char tccr_stat_received_empty_text[]    = "TR EMPTY";
//static char tccr_stat_received_data_text[]     = "TR DATA";
//
//static char tccr_stat_received_rts_text[]      = "TS RTS";
//static char tccr_stat_sent_data_text[]         = "TS DATA";
//static char tccr_stat_sent_empty_text[]        = "TS EMPTY";
//static char tccr_stat_more_than_sync_text[]    = "TS NS DATA";

static mac_slot_data_t mac_slot_data[RN_MAC_CC_MAX_SLOT+1];

static msg_buf_receiver_t * mac_lcc_receive = NULL;

static uint32_t lsccr_send_cnt = 0;
static uint32_t lbccr_send_cnt = 0;
static uint32_t laccr_send_cnt = 0;
static uint32_t lrccr_send_cnt = 0;

static uint32_t lbcca_send_cnt = 0;
static uint32_t lacca_send_cnt = 0;


static uint32_t lsccr_receive_cnt = 0;

static uint32_t lbccr_receive_cnt = 0;
static uint32_t laccr_receive_cnt = 0;
static uint32_t lrccr_receive_cnt = 0;

static uint32_t lbcca_receive_cnt = 0;
static uint32_t lacca_receive_cnt = 0;


// MAC control queue provides control and timing for the MAC thread
static MessageQ_Handle mac_cc_input_queue           = NULL;

static uint8_t mac_rn_address = RN_ADDRESS_INVALID; 
static uint8_t mac_rnid = RN_RNID_INVALID;
/*-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static void rpi_console_write(uint8_t rn_address,const char * tag_text, uint8_t data8, uint32_t data32);
static void mac_list_cmd(uint8_t out_address, uint8_t slot);
static void mac_slot_stat_cmd(uint8_t out_address, uint8_t slot);
static void mac_stat_cmd(uint8_t out_address);

static void send_mac_cc_conf(uint8_t mt,uint8_t rnid);
static void mac_cc_master_conf(uint8_t rnid);                   // Configure MAC CC as master
static void mac_cc_slave_conf(uint8_t rnid);                    // Configure MAC CC as slave
static void mac_cc_sniffer_conf(uint8_t rnid);                  // Configure MAC CC as sniffer
static void mac_cc_idle_conf(uint8_t rnid);                     // Configure MAC CC as idle

static void mac_fill_data(msg_buf_t * transport, uint32_t *input_index, msg_buf_t * msg);

static void mac_cc_receive(msg_buf_t *rts);                     // Execute MAC receive
static bool mac_cc_fill_transport(MessageQ_Handle send_queue,uint32_t *index, msg_buf_t *msg_transport_buf);

static void mac_phy_send(msg_buf_t * msg_transport_buf);

static void mac_dcc_send(uint8_t slot, uint16_t frame);         // Execute MAC DCC send
static void mac_ucc_send(uint8_t slot, uint16_t frame);         // Execute MAC UCC send

/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/
//g(x) = x16 + x12 + x5 + 1
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
typedef uint16_t crc;
crc  crcTable[256];
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))

void crcInit(void)
{
    crc  remainder;
    int  dividend;
    uint8_t bit;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for ( bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */			
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */

crc crcFast(uint8_t const message[], int nBytes)
{
    uint8_t data;
    crc remainder = 0;
    int byte;

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder);

}   /* crcFast() */


/******************************************************************************
* Function Name    : rn_mac_cc_module_init
* Function Purpose : Module Init              
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_mac_cc_module_init(void)
{
    uint8_t slot;

    for (slot = RN_MAC_CC_MIN_SLOT; slot <= RN_MAC_CC_MAX_SLOT;slot++)
    {
        mac_slot_data[slot].mac_slot_conf       = MAC_CC_SLOT_IDLE;
        mac_slot_data[slot].de_interleave[0]    = NULL;
        mac_slot_data[slot].de_interleave[1]    = NULL;
        mac_slot_data[slot].de_interleave[2]    = NULL;
        
        mac_slot_data[slot].tccr_stat_received_empty    = 0;
        mac_slot_data[slot].tccr_stat_received_data     = 0;
         
        mac_slot_data[slot].tccr_stat_received_rts      = 0;
        mac_slot_data[slot].tccr_stat_sent_data         = 0;
        mac_slot_data[slot].tccr_stat_sent_empty        = 0;
        mac_slot_data[slot].tccr_stat_more_than_sync    = 0;
    }

    mac_slot_data[DCC_1_SLOT].interleave[0]     = msg_buf_alloc(MAC_DATA_SIZE);
    mac_slot_data[DCC_1_SLOT].interleave[1]     = msg_buf_alloc(MAC_DATA_SIZE);
    mac_slot_data[DCC_1_SLOT].interleave[2]     = msg_buf_alloc(MAC_DATA_SIZE);
    
    mac_slot_data[UCC_2_SLOT].interleave[0]     = msg_buf_alloc(MAC_DATA_SIZE);
    mac_slot_data[UCC_2_SLOT].interleave[1]     = mac_slot_data[UCC_2_SLOT].interleave[0]; 
    mac_slot_data[UCC_2_SLOT].interleave[2]     = mac_slot_data[UCC_2_SLOT].interleave[0];
     
    mac_slot_data[UCC_3_SLOT].interleave[0]     = msg_buf_alloc(MAC_DATA_SIZE);
    mac_slot_data[UCC_3_SLOT].interleave[1]     = mac_slot_data[UCC_3_SLOT].interleave[0];
    mac_slot_data[UCC_3_SLOT].interleave[2]     = mac_slot_data[UCC_3_SLOT].interleave[0];



    crcInit();
    /*mac_cc_input_queue      = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );

    rn_phy_register_tccr_message_receiver(mac_cc_input_queue);
    rn_phy_register_tccr_message_sender(mac_cc_input_queue);*/

   /* mac_common_send_queue   = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    mac_rcc_send_queue      = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );*/
   
    /*
    mac_acc_rrc_send_queue  = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    mac_acc_app_send_queue  = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    
    mac_bcc_rrc_send_queue  = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    mac_bcc_app_send_queue  = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );*/

	/*RTOS_TASK_CREATE( mac_cc_task, ( signed char * ) "MAC", MAC_TASK_STACK, NULL, RN_MAC_TASK_PRIORITY, NULL );*/
    
    //mac_queue[MAC_REG_QUEUE].mac_main_queue        = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    //mac_queue[MAC_REG_QUEUE].mac_overflow_queue    = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
     
    //mac_queue[MAC_SHARED_QUEUE].mac_main_queue        = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    //mac_queue[MAC_SHARED_QUEUE].mac_overflow_queue    = RTOS_QUEUE_CREATE( RN_MAC_QUEUE_LENGTH, sizeof( msg_buf_t * ) );
    
    return;
}

/******************************************************************************
* Function Name    : rn_mac_cc_module_start
* Function Purpose : Module Start              
* Input parameters : start phase
* Return value     : void
******************************************************************************/
void rn_mac_cc_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}
/******************************************************************************
* Function Name    : rn_mac_register_lcc_receiver
* Function Purpose : Registration of Logical Control Channel Receiver CallBack               
* Input parameters : msg_buf_receiver_t * pointer to callback function
* Return value     : void
******************************************************************************/
void  rn_mac_register_lcc_receiver(msg_buf_receiver_t * lcc_receiver_cb)
{
    mac_lcc_receive = lcc_receiver_cb;
}

/******************************************************************************
* Function Name    : rn_mac_register_ltc_receiver
* Function Purpose : Registration of Logical Traffic Channel Receiver CallBack               
* Input parameters : msg_buf_receiver_t * pointer to callback function
* Return value     : void
******************************************************************************/
void  rn_mac_register_ltc_receiver(msg_buf_receiver_t * lcc_receiver_cb)
{
    // Not implemented yet
}

/******************************************************************************
* Function Name    : rn_mac_lcc_send
* Function Purpose : Send message to logical channel, queue               
* Input parameters : Protocol Discriminator, specifies logical channel
*                    Pointer to message
* Return value     : rn_result_t
******************************************************************************/
rn_result_t rn_mac_lcc_send(uint8_t pd, msg_buf_t * msg)
{
    if (NULL == msg)
    {
        return RN_NULL_POINTER_RECEIVED;
    }
    
    msg->pd = pd;
    msg->mt = LCC_UDATA;

    rn_log_msg_send(RN_RLC, msg);
  
    switch(msg->pd)
    {
    case PD_LSCCR:                          // Logical Synchronization Control Channel 
        rn_log_err_msg("Logical Synchronization Control Channel RN not allowed here",msg);
        return RN_PD_NOT_SUPPORTED;
        break;

    case PD_LRCCR:                          // Logical Registration Control Channel RN
        rtos_send(rn_mac_rcc_send_queue_id(),msg);
        return RN_OK;   
        break;

    case PD_LACCR:                          // Logical Addressed Control Channel RN
    case PD_LBCCR:                          // Logical Broadcast Control Channel RN
    case PD_LBCCA:                          // Logical Broadcast Control Channel AP
    case PD_LACCA:                          // Logical Addressed Control Channel AP
        rtos_send(rn_mac_common_queue_id(),msg);
        return RN_OK;   
        break;

    default:
        rn_log_err_msg("PD not supported",msg);
        msg_buf_free(msg);
        return RN_PD_NOT_SUPPORTED;
        break;
    }

    //if(msg->pd == PD_LRCCR)
    //{
    //    //RTOS_QUEUE_SEND(mac_rcc_send_queue_id(), &msg, 0U );
    //    rtos_send(rn_mac_rcc_send_queue_id(),msg);
    //}
    //else
    //{
    //    //if(msg->pd == 3)
    //    //{
    //    //    rn_log_text("rn_mac_lcc_send PD?");
    //    //}
    //    //RTOS_QUEUE_SEND(rn_mac_common_queue_id(), &msg, 0U );
    //    rtos_send(rn_mac_common_queue_id(),msg);

    //}

    //return RN_OK;   // FIXME
}

/******************************************************************************
* Function Name    : rn_mac_cc_master_conf
* Function Purpose : Configure MAC CC as master              
* Input parameters : rnid
* Return value     : void
******************************************************************************/
void rn_mac_cc_master_conf(uint8_t rnid)
{
    
    send_mac_cc_conf(RPI_MASTER_CONF,rnid);

    //mac_rnid = rnid;
    //mac_cc_master_conf(rnid);    // Later this call may be moved into the thread
}

/******************************************************************************
* Function Name    : rn_mac_cc_slave_conf
* Function Purpose : Configure MAC CC as slave              
* Input parameters : rnid
* Return value     : void
******************************************************************************/
void rn_mac_cc_slave_conf(uint8_t rnid)
{
    send_mac_cc_conf(RPI_SLAVE_CONF,rnid);
    //mac_cc_slave_conf(rnid);    // Later this call may be moved into the thread
}
/******************************************************************************
* Function Name    : rn_mac_cc_sniffer_conf
* Function Purpose : Configure MAC CC as sniffer             
* Input parameters : rnid
* Return value     : void
******************************************************************************/
void rn_mac_cc_sniffer_conf(uint8_t rnid)
{
   send_mac_cc_conf(RPI_SNIFFER_CONF,rnid);
   //mac_cc_sniffer_conf(rnid);    // Later this call may be moved into the thread
}
/******************************************************************************
* Function Name    : rn_mac_cc_idle_conf
* Function Purpose : Configure MAC CC as idle             
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_mac_cc_idle_conf(void)
{
     send_mac_cc_conf(RPI_IDLE_CONF,RN_INVALID_RNID);

    //mac_cc_idle_conf();    // Later this call may be moved into the thread
}

/******************************************************************************
* Function Name    : rn_mac_cc_address_conf
* Function Purpose : Configure MAC CC address             
* Input parameters : address
* Return value     : void
******************************************************************************/
void rn_mac_cc_address_conf(uint8_t address)
{
    mac_rn_address = address; // Later this call may be moved into the thread
}

/******************************************************************************
* Function Name    : rn_mac_cc_task_fkn
* Function Purpose : Implements the MAC task             
* Input parameters : input_queue_handle, control queue
* Return value     : void, does not return
******************************************************************************/
void rn_mac_cc_task_fkn(MessageQ_Handle input_queue_handle)
{
    msg_buf_t   *msg_buf = NULL;
  
    uint8_t     timeslot    = 0;
    uint16_t    frame_no;
     
   
    
    mac_cc_input_queue = input_queue_handle;

    rn_phy_register_tccr_message_receiver(rn_mac_cc_input_queue_id());
    rn_phy_register_tccr_message_sender(rn_mac_cc_input_queue_id());

    for( ;; )
    {
        MESSAGEQ_GET(mac_cc_input_queue, &msg_buf, portMAX_DELAY );
     
        if (msg_buf->pd == PD_RPI)
        {
            rn_log_msg_receive(RN_MAC, msg_buf);
            switch (msg_buf->mt)
            {
            case RPI_MASTER_CONF:                 
               
                mac_cc_master_conf(msg_return_uint8(msg_buf,0));

                rn_log_text("MAC_CC: MASTER"); 
                msg_buf->mt = RPI_STARTED;
                msg_set_data_size(msg_buf,0);

                rn_log_msg_send(RN_MAC, msg_buf);
                rtos_send(rnm_queue_id(),msg_buf);
                break;   

            case RPI_SLAVE_CONF:                  
                mac_cc_slave_conf(msg_return_uint8(msg_buf,0));
                
                rn_log_text("MAC_CC: SLAVE"); 
                msg_buf->mt = RPI_STARTED;
                msg_set_data_size(msg_buf,0);
                rn_log_msg_send(RN_MAC, msg_buf);
                rtos_send(rnm_queue_id(),msg_buf);
                break; 

                
            case RPI_SNIFFER_CONF:                  
                mac_cc_sniffer_conf(msg_return_uint8(msg_buf,0));
                
                rn_log_text("MAC_CC: SNIFFER"); 
                msg_buf->mt = RPI_STARTED;
                msg_set_data_size(msg_buf,0);
                rn_log_msg_send(RN_MAC, msg_buf);
                rtos_send(rnm_queue_id(),msg_buf);
                break; 

                 
            case RPI_IDLE_CONF:                  
                mac_cc_idle_conf(msg_return_uint8(msg_buf,0));
                
                rn_log_text("MAC_CC: IDLE"); 
                msg_buf->mt = RPI_STOPPED;

                msg_set_data_size(msg_buf,0);
                rn_log_msg_send(RN_MAC, msg_buf);
                rtos_send(rnm_queue_id(),msg_buf);
                break; 

            case RPI_MAC_LIST_CMD:
                //rni_console_write(msg_return_uint8(msg_buf,0),"MAC conf");
                mac_list_cmd(msg_return_uint8(msg_buf,0),1);
                mac_list_cmd(msg_return_uint8(msg_buf,0),2);
                mac_list_cmd(msg_return_uint8(msg_buf,0),3);
                break;

            case RPI_MAC_STAT_CMD:
                mac_slot_stat_cmd(msg_return_uint8(msg_buf,0),1);
                mac_slot_stat_cmd(msg_return_uint8(msg_buf,0),2);
                mac_slot_stat_cmd(msg_return_uint8(msg_buf,0),3);

                mac_stat_cmd(msg_return_uint8(msg_buf,0));
                break;

            default: 
                rn_log_text_msg("MAC_CC: RPI message not implemented",msg_buf);
                //printf("%s %d", __FILE__,__LINE__);
                msg_buf_free(msg_buf); 
                break;
            }
        }
        else if (msg_buf->pd == PD_TCCR)
        {
            timeslot = msg_return_uint8 (msg_buf, TCCR_TIMESLOT_POS);
            frame_no = msg_return_uint16(msg_buf, TCCR_FN_POS);

            if (timeslot < RN_MAC_CC_MIN_SLOT)
            {

            }

            if (timeslot > RN_MAC_CC_MAX_SLOT)
            {

            }
               
            switch (msg_buf->mt)
            {
           

            case TCCR_READY_TO_SEND:
                mac_slot_data[timeslot].tccr_stat_received_rts++;
                switch(mac_slot_data[timeslot].mac_slot_conf)
                {
                case MAC_CC_SLOT_DCC_SEND:
                    mac_dcc_send(timeslot, frame_no);
                    msg_buf_free(msg_buf); 
                    break;

                case MAC_CC_SLOT_UCC_SEND:
                    mac_ucc_send(timeslot,frame_no);
                    msg_buf_free(msg_buf); 
                    break;
                }
                break;

            case TCCR_DATA:
                mac_slot_data[timeslot].tccr_stat_received_data++;
                switch(mac_slot_data[timeslot].mac_slot_conf)
                {
                case MAC_CC_SLOT_DCC_RECEIVE:
                    // FIXME: Deinterleave
                    // FIXME: Decode
                    // FIXME: Calculate CRC
                    // FIXME: Skip if CRC error
                    mac_cc_receive(msg_buf);
                    break;
                case MAC_CC_SLOT_UCC_RECEIVE:
                    // FIXME: Decode
                    // FIXME: Calculate CRC
                    // FIXME: Skip if CRC error
                    mac_cc_receive(msg_buf);
                    break;
                }
                msg_buf_free(msg_buf); 
                break; 
                
            case TCCR_EMPTY:
                mac_slot_data[timeslot].tccr_stat_received_empty++;
                msg_buf_free(msg_buf); 
                break;

            default: 
                rn_log_text_msg("MAC_CC: Message not implemented",msg_buf);
                msg_buf_free(msg_buf); 
                break;
            }
        }
        else
        {
            rn_log_text_msg("MAC_CC: Protocol not implemented\n\r",msg_buf);
            msg_buf_free(msg_buf);
        }
    }
}
/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/
static void rpi_console_write(uint8_t rn_address,const char * tag_text, uint8_t data8, uint32_t data32)
{ 
    msg_buf_t   *stat_msg = NULL;
    uint32_t    stat_index;

    stat_msg = msg_buf_alloc(2+sizeof(char*) + sizeof(uint32_t));
    stat_index = 0;
   
    msg_put_uint8_i(stat_msg,&stat_index,rn_address); 
    msg_put_uint32_i(stat_msg,&stat_index,(uint32_t) tag_text);
    msg_put_uint8_i(stat_msg,&stat_index,data8); // Reporting timeslot 1
    msg_put_uint32_i(stat_msg,&stat_index,data32);

    stat_msg->mt = RPI_STAT_RES;
    stat_msg->pd = PD_RPI;

    rtos_send(rnm_queue_id(),stat_msg);
}

/******************************************************************************
* Function Name    : mac_list_cmd
* Function Purpose : Send MAC info to console               
* Input parameters : out_address specifies console address
* Return value     : void
******************************************************************************/
static void mac_list_cmd(uint8_t out_address, uint8_t slot)
{
    

     switch(mac_slot_data[slot].mac_slot_conf)
     {
     case MAC_CC_SLOT_IDLE :            rpi_console_write(out_address,"IDLE ",slot,0); break;
     case MAC_CC_SLOT_DCC_SEND :        rpi_console_write(out_address,"S DCC",slot,0); break;
     case MAC_CC_SLOT_DCC_RECEIVE :     rpi_console_write(out_address,"R DCC",slot,0); break;
     case MAC_CC_SLOT_UCC_RECEIVE :     rpi_console_write(out_address,"R UCC",slot,0); break;
     case MAC_CC_SLOT_UCC_SEND:         rpi_console_write(out_address,"S UCC",slot,0); break;
     }
}
/******************************************************************************
* Function Name    : mac_slot_stat_cmd
* Function Purpose : Send MAC statistics for speified slot to console               
* Input parameters : out_address specifies console address
* Return value     : void
******************************************************************************/
static void mac_slot_stat_cmd(uint8_t out_address, uint8_t slot)
{
    // MAC_CC_SLOT_IDLE = 1,           // Idle slot 
    //MAC_CC_SLOT_DCC_SEND,           // DCC send slot 
    //MAC_CC_SLOT_DCC_RECEIVE,        // DCC receive slot
    //MAC_CC_SLOT_UCC_SEND,           // UCC send slot
    //MAC_CC_SLOT_UCC_RECEIVE         // UCC receive slot

    switch(mac_slot_data[slot].mac_slot_conf)
    {
    case MAC_CC_SLOT_DCC_SEND:
    case MAC_CC_SLOT_UCC_SEND:
        //rpi_console_write(out_address,"S RTS  ",slot,mac_slot_data[slot].tccr_stat_received_rts);
        rpi_console_write(out_address,"S TCCR ",slot,mac_slot_data[slot].tccr_stat_sent_data);
        //rpi_console_write(out_address,"S EMPTY",slot,mac_slot_data[slot].tccr_stat_sent_empty);
        //rpi_console_write(out_address,"S MTS  ",slot,mac_slot_data[slot].tccr_stat_more_than_sync);
        break;

    case MAC_CC_SLOT_DCC_RECEIVE:
    case MAC_CC_SLOT_UCC_RECEIVE:
        rpi_console_write(out_address,"R TCCR ",slot,mac_slot_data[slot].tccr_stat_received_data);
        //rpi_console_write(out_address,"R EMPTY",slot,mac_slot_data[slot].tccr_stat_received_empty);
        break;
    }
    
}
/******************************************************************************
* Function Name    : mac_stat_cmd
* Function Purpose : Send MAC logical channel statistics to console               
* Input parameters : out_address specifies console address
* Return value     : void
******************************************************************************/
static void mac_stat_cmd(uint8_t out_address)
{
        rpi_console_write(out_address,"S LSCCR",0,lsccr_send_cnt);
        rpi_console_write(out_address,"R LSCCR",0,lsccr_receive_cnt);

        rpi_console_write(out_address,"S LRCCR",0,lrccr_send_cnt);
        rpi_console_write(out_address,"R LRCCR",0,lrccr_receive_cnt);

        rpi_console_write(out_address,"S LACCR",0,laccr_send_cnt);
        rpi_console_write(out_address,"R LACCR",0,laccr_receive_cnt);
        
        rpi_console_write(out_address,"S LBCCR",0,lbccr_send_cnt);
        rpi_console_write(out_address,"R LBCCR",0,lbccr_receive_cnt);

        rpi_console_write(out_address,"S LACCA",0,lacca_send_cnt);
        rpi_console_write(out_address,"R LACCA",0,lacca_receive_cnt);

        rpi_console_write(out_address,"S LBCCA",0,lbcca_send_cnt);
        rpi_console_write(out_address,"R LBCCA",0,lbcca_receive_cnt);  
}

/******************************************************************************
* Function Name    : send_mac_cc_conf
* Function Purpose : Send internal configuration message to MAC task               
* Input parameters : Protocol Discriminator, specifies logical channel
*                    Pointer to 
* Return value     : void
******************************************************************************/
static void send_mac_cc_conf(uint8_t mt,uint8_t rnid)
{
    msg_buf_t *mac_conf;

    mac_conf = msg_buf_alloc(1);

    msg_put_pd(PD_RPI,mac_conf);
    msg_put_mt(mt,mac_conf);
    msg_put_uint8(mac_conf,0,rnid);

    rtos_send(rn_mac_cc_input_queue_id(),mac_conf);
    
}

/******************************************************************************
* Function Name    : mac_cc_master_conf
* Function Purpose : Configure MAC for operation as RN-master             
* Input parameters : Radio Network ID
* Return value     : void
******************************************************************************/
static void mac_cc_master_conf(uint8_t rnid)
{
    char log_text[40] ="";

    mac_rnid = rnid;

    rn_phy_master_conf(rnid);

    mac_slot_data[DCC_1_SLOT].mac_slot_conf = MAC_CC_SLOT_DCC_SEND;
    rn_phy_activate_slot(DCC_1_SLOT,PHY_SLOT_DCC_SEND); 

    mac_slot_data[UCC_2_SLOT].mac_slot_conf = MAC_CC_SLOT_UCC_RECEIVE;
    rn_phy_activate_slot(UCC_2_SLOT,PHY_SLOT_UCC_RECEIVE);

    mac_slot_data[UCC_3_SLOT].mac_slot_conf = MAC_CC_SLOT_UCC_RECEIVE;
    rn_phy_activate_slot(UCC_3_SLOT,PHY_SLOT_UCC_RECEIVE);
   
    sprintf(log_text,"mac_cc_master_conf, RNID = %d",rnid);
    rn_log_vtext(log_text);
}

/******************************************************************************
* Function Name    : mac_cc_slave_conf
* Function Purpose : Configure MAC for operation as RN-slave             
* Input parameters : Radio Network ID
* Return value     : void
******************************************************************************/
static void mac_cc_slave_conf(uint8_t rnid)
{
    char log_text[40] ="";
    rn_phy_slave_conf(rnid);

    mac_slot_data[DCC_1_SLOT].mac_slot_conf     = MAC_CC_SLOT_DCC_RECEIVE;
    rn_phy_activate_slot(DCC_1_SLOT,PHY_SLOT_DCC_RECEIVE);   

    mac_slot_data[UCC_2_SLOT].mac_slot_conf     = MAC_CC_SLOT_UCC_SEND;
    rn_phy_activate_slot(UCC_2_SLOT,PHY_SLOT_UCC_SEND);

    mac_slot_data[UCC_3_SLOT].mac_slot_conf     = MAC_CC_SLOT_UCC_SEND;
    rn_phy_activate_slot(UCC_3_SLOT,PHY_SLOT_UCC_SEND);
    
    sprintf(log_text,"mac_cc_slave_conf RNID = %d",rnid);
    rn_log_vtext(log_text);

}
/******************************************************************************
* Function Name    : mac_cc_sniffer_conf
* Function Purpose : Configure MAC for operation as RN-sniffer             
* Input parameters : Radio Network ID
* Return value     : void
******************************************************************************/
static void mac_cc_sniffer_conf(uint8_t rnid)
{ 
    char log_text[40] ="";
      
    mac_rnid = rnid;

    rn_phy_slave_conf(rnid);

    mac_slot_data[DCC_1_SLOT].mac_slot_conf = MAC_CC_SLOT_DCC_RECEIVE;
    rn_phy_activate_slot(DCC_1_SLOT,PHY_SLOT_DCC_RECEIVE);   

    mac_slot_data[UCC_2_SLOT].mac_slot_conf = MAC_CC_SLOT_UCC_RECEIVE;
    rn_phy_activate_slot(UCC_2_SLOT,PHY_SLOT_UCC_RECEIVE);

    mac_slot_data[UCC_3_SLOT].mac_slot_conf = MAC_CC_SLOT_UCC_RECEIVE;
    rn_phy_activate_slot(UCC_3_SLOT,PHY_SLOT_UCC_RECEIVE);

    sprintf(log_text,"mac_cc_sniffer_conf, RNID = %d",rnid);
    rn_log_vtext(log_text);

}

/******************************************************************************
* Function Name    : mac_cc_idle_conf
* Function Purpose : Configure MAC as IDLE             
* Input parameters : Radio Network ID, shall be set as invalid
* Return value     : void
******************************************************************************/
static void mac_cc_idle_conf(uint8_t rnid)
{

    mac_rnid = rnid;

    mac_slot_data[DCC_1_SLOT].mac_slot_conf = MAC_CC_SLOT_IDLE;
    rn_phy_deactivate_slot(DCC_1_SLOT); 

    mac_slot_data[UCC_2_SLOT].mac_slot_conf = MAC_CC_SLOT_IDLE;
    rn_phy_deactivate_slot(UCC_2_SLOT); 

    mac_slot_data[UCC_3_SLOT].mac_slot_conf = MAC_CC_SLOT_IDLE;
    rn_phy_deactivate_slot(UCC_3_SLOT); 

    rn_phy_idle_conf();
    rn_log_text("mac_cc_idle_conf(void)");
}
 
//void rn_mac_receive(msg_buf_t *message_received)
//{
//    if (NULL != mac_lcc_receive)
//    {
//        mac_lcc_receive(message_received);
//    }
//    else
//    {
//        rn_log_err_msg("rn_mac_receive, no callback registered", message_received);
//        msg_buf_free(message_received);
//    }
//}

/******************************************************************************
* Function Name    : mac_fill_data
* Function Purpose : Fill data from transport buffer into message buffer            
* Input parameters : Pointer to transport buffer
*                    Index to current position
*                    Pointer to message buffer
* Return value     : void
******************************************************************************/
static void mac_fill_data(msg_buf_t * transport, uint32_t *input_index, msg_buf_t * msg)
{
    memcpy(msg->data, &transport->data[*input_index],msg->size);
    *input_index += msg->size;
}

/******************************************************************************
* Function Name    : mac_cc_receive
* Function Purpose : MAC DCC/UCC receive            
* Input parameters : Pointer to received transport buffer
* Return value     : void
******************************************************************************/
static void mac_cc_receive(msg_buf_t *transport_received)
 {
     msg_buf_t  *message_received = NULL;
     uint32_t   read_index;
     uint8_t    mac_tag;
     uint8_t    rnid;               // Any use for this?
     
     uint8_t    rn_receiver = RN_INVALID_ADDRESS;
     uint8_t    rn_sender   = RN_INVALID_ADDRESS;
     uint8_t    data_size;

     uint16_t   frame_number;       // Any use for this?

     read_index = TCCR_DATA_POS;

     mac_tag = msg_return_uint8_i(transport_received, &read_index);

     while ((mac_tag != MAC_NULL_TAG) 
         && 
         (read_index < msg_return_data_size(transport_received)))
     {
         if (MAC_LSCCR_TAG == (MAC_2_TAG_FILTER & mac_tag))
         {
             rnid = MAC_RNID_FILTER & mac_tag;
             mac_tag = MAC_LSCCR_TAG;
         } 
         else if (MAC_LACCR_TAG == (MAC_3_TAG_FILTER & mac_tag))
         {
             rn_receiver = MAC_RN_RECEIVER_FILTER & mac_tag;
             mac_tag = MAC_LACCR_TAG;
         }
         else if (MAC_LACCA_TAG == (MAC_3_TAG_FILTER & mac_tag))
         {
             rn_receiver = MAC_RN_RECEIVER_FILTER & mac_tag;
             mac_tag = MAC_LACCA_TAG;
         }

         switch(mac_tag)
         {
         case  MAC_LSCCR_TAG:
             frame_number = msg_return_uint16_i(transport_received, &read_index);
             lsccr_receive_cnt++;
             // FIXME: What to do?
             break;
             
         case MAC_LACCR_TAG:
             rn_sender = MAC_RN_SENDER_FILTER & msg_return_uint8_i(transport_received, &read_index);
             data_size = MAC_DATA_SIZE_FILTER & msg_return_uint8_i(transport_received, &read_index);
             
             message_received = msg_buf_alloc(data_size);
             mac_fill_data(transport_received, &read_index, message_received);

             message_received->pd           = PD_LACCR;
             message_received->mt           = ACC_UDATA;
             message_received->rn_receiver  = rn_receiver;
             message_received->rn_sender    = rn_sender;
             
             if(mac_lcc_receive(message_received))
             {
                 laccr_receive_cnt++;
             }

             break;

         case MAC_LACCA_TAG:
             rn_sender = MAC_RN_SENDER_FILTER & msg_return_uint8_i(transport_received, &read_index);
             data_size = MAC_DATA_SIZE_FILTER & msg_return_uint8_i(transport_received, &read_index);

             message_received = msg_buf_alloc(data_size);
             mac_fill_data(transport_received, &read_index, message_received);

             message_received->pd           = PD_LACCA;
             message_received->mt           = ACC_UDATA;
             message_received->rn_receiver  = rn_receiver;
             message_received->rn_sender    = rn_sender;

             //mac_lcc_receive(message_received); // Callback

             if(mac_lcc_receive(message_received))
             {
                 lacca_receive_cnt++;
             }
             break;

         case MAC_LRCCR_TAG:
             
             data_size = MAC_DATA_SIZE_FILTER & msg_return_uint8_i(transport_received, &read_index);
             message_received = msg_buf_alloc(data_size);
             mac_fill_data(transport_received, &read_index, message_received);

             message_received->pd           = PD_LRCCR;
             message_received->mt           = RCC_UDATA;
             message_received->rn_receiver  = rn_receiver;      // Dummy setting
             message_received->rn_sender    = rn_sender;        // Dummy setting

             //mac_lcc_receive(message_received); // Callback

             if(mac_lcc_receive(message_received))
             {
                 lrccr_receive_cnt++;
             }
             break;

         case MAC_LBCCR_TAG:
             rn_sender = MAC_RN_SENDER_FILTER & msg_return_uint8_i(transport_received, &read_index);
             data_size = MAC_DATA_SIZE_FILTER & msg_return_uint8_i(transport_received, &read_index);
             
             message_received = msg_buf_alloc(data_size);
             mac_fill_data(transport_received, &read_index, message_received);

             message_received->pd           = PD_LBCCR;
             message_received->mt           = BCC_UDATA;
             message_received->rn_receiver  = rn_receiver;
             message_received->rn_sender    = rn_sender;
             
             //mac_lcc_receive(message_received); // Callback

             if(mac_lcc_receive(message_received))
             {
                 lbccr_receive_cnt++;
             }
             break;

         case MAC_LBCCA_TAG:
             rn_sender = MAC_RN_SENDER_FILTER & msg_return_uint8_i(transport_received, &read_index);
             data_size = MAC_DATA_SIZE_FILTER & msg_return_uint8_i(transport_received, &read_index);
             
             message_received = msg_buf_alloc(data_size);
             mac_fill_data(transport_received, &read_index, message_received);

             message_received->pd           = PD_LBCCA;
             message_received->mt           = BCC_UDATA;
             message_received->rn_receiver  = rn_receiver;
             message_received->rn_sender    = rn_sender;
             
             // mac_lcc_receive(message_received); // Callback
             if(mac_lcc_receive(message_received))
             {
                 lbcca_receive_cnt++;
             }
             break;
        
     
         default:
             rn_log_err_msg("mac_cc_receive, tag decode error", transport_received);
             break;

         }
         mac_tag = msg_return_uint8_i(transport_received, &read_index);
     }
 }
 /******************************************************************************
* Function Name    : mac_cc_fill_transport
* Function Purpose : Fill CC transport buffer with data to send            
* Input parameters : Handle for logical send queue
*                    Pointer to Index to next position in transport buffer
*                    Pointer to transport buffer
* Output parameter   Index to next position in transport buffer
* Return value     : bool = true if anything filled
******************************************************************************/
static bool mac_cc_fill_transport(MessageQ_Handle send_queue,uint32_t *index, msg_buf_t *msg_transport_buf)
{
    uint32_t        remaining_input_data;
    uint32_t        available_transport_bytes = 0;
    //msg_buf_t       *msg_input_buf1     = NULL;
    msg_buf_t       *msg_input_buf      = NULL;
    uint32_t        in_data_index       = *index;   // For checking if anything was filled
    uint32_t        out_data_index      = *index;
    uint32_t        mac_overhead    = 0;
    //portBASE_TYPE   peek_result     = pdFALSE;
    portBASE_TYPE   receive_result  = pdFALSE;
  
    //peek_result = RTOS_QUEUE_PEEK(send_queue, &msg_input_buf, 0U);
    receive_result = RTOS_QUEUE_RECEIVE(send_queue, &msg_input_buf, 0U);
    
   /* while(pdTRUE == peek_result)*/
   
    while(pdTRUE == receive_result)
    {   
        available_transport_bytes = msg_transport_buf->size - out_data_index;

        if (PD_LRCCR == msg_input_buf->pd)
        {
            mac_overhead = 2;
        }
        else
        {
             mac_overhead = 3;
        }

        if(available_transport_bytes < msg_input_buf->size + mac_overhead)
        {
            // Message does not fit this time
            // FIXME: if the message is all too large then it should be deleted
            // FIXME: if the message is all too large then it should be deleted
            //rtos_send_front(
            *index = out_data_index;
             return (bool)(out_data_index > in_data_index); 
        }
        
        // There should be space, now take out the message from queue

        //receive_result = RTOS_QUEUE_RECEIVE(send_queue, &msg_input_buf, 0U); 

        if (pdTRUE !=  receive_result)  
        {
             *index = out_data_index;
             return (bool)(out_data_index > in_data_index);     // Should not happen...
        }
         
        rn_log_msg_receive(RN_MAC,msg_input_buf);

        switch (msg_input_buf->pd)
        {
        case PD_LBCCR:  
            msg_put_uint8 (msg_transport_buf,out_data_index,MAC_LBCCR_TAG);
            lbccr_send_cnt++;
            break;

        case PD_LACCR:
            msg_put_uint8 (msg_transport_buf, out_data_index, MAC_LACCR_TAG + msg_input_buf->rn_receiver);
            laccr_send_cnt++;
            break;

        case PD_LBCCA: 
            msg_put_uint8 (msg_transport_buf, out_data_index, MAC_LBCCA_TAG);
            lbcca_send_cnt++;
            break;

        case PD_LACCA:   
            msg_put_uint8 (msg_transport_buf, out_data_index, MAC_LACCA_TAG + msg_input_buf->rn_receiver);
            lacca_send_cnt++;
            break;

        case PD_LRCCR:   
            msg_put_uint8 (msg_transport_buf, out_data_index, MAC_LRCCR_TAG);
            lrccr_send_cnt++;
            break;

        default:
            rn_log_err_msg("mac_cc_fill_transport does not handle PD",msg_input_buf);
            *index = out_data_index;
            msg_buf_free(msg_input_buf);
            return (bool)(out_data_index > in_data_index);
        }

        out_data_index++;                          // Increment after tagging

        if(msg_input_buf->pd != PD_LRCCR)          // No sender address available for Registration Channel
        {
            msg_put_uint8 (msg_transport_buf, out_data_index, msg_input_buf->rn_sender);
            out_data_index++;
        }

        msg_put_uint8 (msg_transport_buf, out_data_index, (uint8_t) msg_input_buf->size);
        out_data_index++;

        remaining_input_data = msg_input_buf->size;
        in_data_index = 0;

        while (remaining_input_data > 0)
        {
            msg_put_uint8 (msg_transport_buf, out_data_index, msg_input_buf->data[in_data_index]);
            out_data_index++;
            in_data_index++;
            remaining_input_data--;

            //msg_transport_buf->data[out_data_index] = msg_input_buf->data[in_data_index];
        }
        msg_buf_free(msg_input_buf);        // Finished with this message
        
        receive_result = pdFALSE;

        //peek_result = RTOS_QUEUE_PEEK(send_queue, &msg_input_buf, 0U); // Try next
    }
    //else
    //{
    //    return false; // Nothing was filled
    //}

    *index = out_data_index;
    return (bool)(out_data_index > in_data_index);
}
/******************************************************************************
* Function Name    : mac_phy_send
* Function Purpose : Send transport buffer to PHY            
* Input parameters : Pointer to transport buffer
* Return value     : void
******************************************************************************/
static void mac_phy_send(msg_buf_t * msg_transport_buf)
{
    //RTOS_QUEUE_SEND(mac_phy_send_queue_get(), msg_transport_buf, 0U );
    rtos_send(rn_phy_send_queue_get(),msg_transport_buf);
}

/******************************************************************************
* Function Name    : mac_dcc_send
* Function Purpose : MAC DCC send            
* Input parameters : slot
*                    frame
* Return value     : void
******************************************************************************/
 static void mac_dcc_send(uint8_t slot, uint16_t frame)
 {
    
     msg_buf_t  *msg_transport_buf = NULL;
     //msg_buf_t  *msg_data_buf = NULL;
     uint32_t    out_data_index = TCCR_DATA_POS;     // Here is where air interface data starts
     bool        more_than_sync  = false;
     //msg_data_buf               = msg_buf_alloc(TCCR_DATA_SIZE);
     msg_transport_buf          = msg_buf_alloc(TCCR_DATA_SIZE);
     msg_transport_buf->pd      = PD_TCCR;
     msg_transport_buf->mt      = TCCR_DATA;
     
     msg_put_uint8 (msg_transport_buf,TCCR_TIMESLOT_POS,slot);  // Part of transport header
     msg_put_uint16(msg_transport_buf,TCCR_FN_POS,frame);       // Part of transport header
     
     // Start fill tranport data
     out_data_index = TCCR_DATA_POS;

     
     if(true)   // Can be switched off for debug purpose
     {
         // "Receive" and build synchronization channel
         msg_put_uint8_i (msg_transport_buf,&out_data_index,MAC_LSCCR_TAG + mac_rnid);
         msg_put_uint16_i (msg_transport_buf,&out_data_index,frame);
         
            lsccr_send_cnt++;
         // Synchronization channel done
     }

    
     // Fill transport buffer
     
     more_than_sync = mac_cc_fill_transport(rn_mac_common_queue(), &out_data_index, msg_transport_buf);

     if(out_data_index < msg_transport_buf->size)
     {
         msg_transport_buf->data[out_data_index] = MAC_NULL_TAG;
         out_data_index++;
     }

     while (out_data_index < msg_transport_buf->size)
     {
         msg_transport_buf->data[out_data_index] = MAC_FILL_TAG;
         out_data_index++;
     }
     
     if(more_than_sync)
     {
         rn_log_msg_send(RN_MAC, msg_transport_buf);
         mac_slot_data[slot].tccr_stat_more_than_sync++;
     }

     //RTOS_QUEUE_SEND(rn_phy_send_queue_get(), &msg_transport_buf, 0U );

     rtos_send(rn_phy_send_queue_get(),msg_transport_buf);
     mac_slot_data[slot].tccr_stat_sent_data++;

//     rn_phy_send(&msg_transport_buf);
 }


/******************************************************************************
* Function Name    : mac_ucc_send
* Function Purpose : MAC UCC send            
* Input parameters : slot
*                    frame
* Return value     : void
*
*                   Timeslot 2 is used for registrations, it is a contention channel
*                   Timeslot 3 is scheduled between units, depending on registered address
******************************************************************************/
 static void mac_ucc_send(uint8_t slot, uint16_t frame)
 {
     //msg_buf_t          *msg_input_buf      = NULL;
     msg_buf_t          *msg_transport_buf  = NULL;
     uint32_t            out_data_index     = TCCR_DATA_POS;     // Here is where air interface data starts
     MessageQ_Handle mac_send_queue     = NULL;
     crc                 crc_16;
    /* msg_buf_t          *mac_overflow_queue = NULL;*/

     if (3 == slot)
     {
         if((mac_rn_address - 1) != (frame % RN_MAX_SLAVE_ADDRESS))
         {
             msg_transport_buf       = msg_buf_alloc(TCCR_EMPTY_SIZE);
             msg_transport_buf->pd   = PD_TCCR;
             msg_transport_buf->mt   = TCCR_EMPTY;
             msg_put_uint8 (msg_transport_buf,TCCR_TIMESLOT_POS,slot);  // Part of transport header
             msg_put_uint16(msg_transport_buf,TCCR_FN_POS,frame);       // Part of transport header
             rn_log_msg_send(RN_MAC, msg_transport_buf);
             //RTOS_QUEUE_SEND(rn_phy_send_queue_get(), &msg_transport_buf, 0U );

             rtos_send(rn_phy_send_queue_get(),msg_transport_buf);
             return;
         }
     }

     msg_transport_buf          = msg_buf_alloc(TCCR_DATA_SIZE);
     msg_transport_buf->pd      = PD_TCCR;
     msg_transport_buf->mt      = TCCR_DATA;

     msg_put_uint8 (msg_transport_buf,TCCR_TIMESLOT_POS,slot);  // Part of transport header
     msg_put_uint16(msg_transport_buf,TCCR_FN_POS,frame);       // Part of transport header

     // Start fill tranport data
     out_data_index = TCCR_DATA_POS;

     if (2 == slot)
     {
         mac_send_queue     = rn_mac_rcc_send_queue();
     }
     else if (3 == slot)
     {
         mac_send_queue     = rn_mac_common_queue();
     }
     else
     {
         // FIXME, some inconsistencý
     }

     if(!mac_cc_fill_transport(mac_send_queue,&out_data_index, msg_transport_buf))
     {
         msg_transport_buf->mt      = TCCR_EMPTY;
         msg_transport_buf->size    = TCCR_EMPTY_SIZE;
         rn_log_msg_send(RN_MAC, msg_transport_buf);
         //RTOS_QUEUE_SEND(rn_phy_send_queue_get(), &msg_transport_buf, 0U );
         mac_slot_data[slot].tccr_stat_sent_empty++;
         rtos_send(rn_phy_send_queue_get(),msg_transport_buf);
         return;
     }
     else
     {
         if(out_data_index < msg_transport_buf->size)
         {
             msg_transport_buf->data[out_data_index] = MAC_NULL_TAG;
             out_data_index++;
         }

         while (out_data_index < msg_transport_buf->size)
         {
             msg_transport_buf->data[out_data_index] = MAC_FILL_TAG;
             out_data_index++;
         }
     }

     crc_16 = crcFast(&msg_transport_buf->data[TCCR_DATA_POS],MAC_DATA_SIZE);
     
     // FIXME: Calculate CRC
     // FIXME: Encode (Copy two times, include CRC)

     rn_log_msg_send(RN_MAC, msg_transport_buf);

     //RTOS_QUEUE_SEND(rn_phy_send_queue_get(), &msg_transport_buf, 0U );
     rtos_send(rn_phy_send_queue_get(),msg_transport_buf);
     mac_slot_data[slot].tccr_stat_sent_data++;
     
     return;
 }
