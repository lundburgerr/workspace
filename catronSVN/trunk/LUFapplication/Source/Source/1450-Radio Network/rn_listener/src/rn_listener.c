/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_listener.c
* Author            : Per Erik Sundvisson
* Description       : Implementation of RN listener functions
********************************************************************************
* Revision History
* 2013-03-21        : First Issue
*******************************************************************************/
/*-------------------------------------------------------------------------------
Standard include
-------------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdlib.h>

/*-------------------------------------------------------------------------------
Macros
-------------------------------------------------------------------------------*/

#define FRAMES_PER_SECOND                           (50)

/*-------------------------------------------------------------------------------
RTOS dependencies
-------------------------------------------------------------------------------*/

#include "rtos.h"

/*-------------------------------------------------------------------------------
Project includes
-------------------------------------------------------------------------------*/

#include "rni.h"
#include "rn_manager.h"
#include "rn_listener.h"
#include "rn_phy.h"
#include "rn_rlc_cc.h"
#include "rn_mac_cc.h"
#include "rn_log.h"

/*-------------------------------------------------------------------------------
Static variable definitions
-------------------------------------------------------------------------------*/

#define MAX_CHARS 128
static char console_text[MAX_CHARS+1];

/*-----------------------------------------------------------------
Managenemt data is defined by the master and mirrored to the sniffer
-------------------------------------------------------------------*/

static rn_management_data_t rn_management_data[RN_MAX_ADDRESS+1]; // Mirrored data from the RN-master

typedef enum
{
    RN_SNIFFER_IDLE,
    RN_SNIFFER_SEARCHING,
    RN_SNIFFER_FOLLOWING
} sniffer_state_t;

typedef struct
{
    sniffer_state_t state;
    uint32_t        state_timer;
    uint8_t         rnid;
    RTOS_QUEUE_ID_T ap_rni_q;
} sniffer_data_t;

static sniffer_data_t sniffer_data;


/*-------------------------------------------------------------------------------
Static function declarations
-------------------------------------------------------------------------------*/


static void receive_rpi  (msg_buf_t *msg_buf);
static void receive_rrc  (msg_buf_t *msg_buf);
static void receive_sync (msg_buf_t *msg_buf);
static void receive_plc  (msg_buf_t *msg_buf);
static void receive_rni  (msg_buf_t *msg_buf);

static void set_sniffer_state(sniffer_state_t state);
static void console_write_registrations(void);
static void r_console_registrations(uint8_t remote_a);

/*-------------------------------------------------------------------------------
Global function definitions
-------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name    : rn_listener_module_init
* Function Purpose : Initiation of data in module              
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_listener_module_init(void)
{
    uint8_t RN_address;

    for (RN_address = RN_MIN_ADDRESS; RN_address <= RN_MAX_ADDRESS; RN_address++)
    {
        rn_management_data[RN_address].rn_registration_data.rn_registration_state     = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.eqmid                     = 0;
    }

    sniffer_data.state          = RN_SNIFFER_IDLE;
    sniffer_data.state_timer    = 0;
    sniffer_data.ap_rni_q       = NULL;

    return;
}

/******************************************************************************
* Function Name    : rn_listener_module_start
* Function Purpose : Start of module                
* Input parameters : start phase 
* Return value     : void
******************************************************************************/
void rn_listener_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}
/******************************************************************************
* Function Name    : rn_sniffer
* Function Purpose : Implementation of RM sniffer functions, 
*                    executing in RN manager task                
* Input parameters : rnm_input_queue, control input queue for task 
*                    client_queue, registered RNI client queue ID
*                    rnid, Radio Network ID specified by client
*                    eqmid, Equipment ID specified by client
* Return value     : void
******************************************************************************/

void rn_sniffer(RTOS_QUEUE_HANDLE_T rnm_input_queue, RTOS_QUEUE_ID_T rni_out_q, uint8_t rnid)
{
	msg_buf_t   *msg_buf = NULL;
   
    sniffer_data.rnid      = rnid;
    sniffer_data.ap_rni_q  = rni_out_q;

    set_sniffer_state(RN_SNIFFER_IDLE);   // Until PHY reports otherwise
    
    rn_mac_cc_sniffer_conf(rnid);         // Will also configer PHY
  
    for( ;; )
    {
        //MESSAGEQ_GET(rnm_input_queue, &msg_buf, portMAX_DELAY );
        RTOS_QUEUE_RECEIVE(rnm_input_queue, &msg_buf, portMAX_DELAY);
        switch(msg_buf->pd)
        {
        case PD_RNI:
            rn_log_msg_receive(RN_RNM, msg_buf);
            if (RNI_UNDEFINE == msg_buf->mt)
            {
               
                set_sniffer_state(RN_SNIFFER_IDLE);
                msg_buf_free(msg_buf);
                return;  
            }
            else
            {
                receive_rni(msg_buf);
                msg_buf_free(msg_buf); 
            }
            break;

        case PD_RPI:
            rn_log_msg_receive(RN_RNM, msg_buf);
            receive_rpi(msg_buf);
            msg_buf_free(msg_buf);
            break;

        case PD_RCCR:
            receive_rrc(msg_buf);
            msg_buf_free(msg_buf);
            break;
        case PD_PLC:
            rn_log_msg_receive(RN_RNM, msg_buf);
            receive_plc(msg_buf);
            msg_buf_free(msg_buf);
            break;

        case PD_SYNC:
            rn_log_msg_receive(RN_RNM, msg_buf);
            receive_sync(msg_buf);
            msg_buf_free(msg_buf);
            break;

        default:
            rn_log_err_msg("Ignored Protocol Discriminator in rn_sniffer",msg_buf);
            msg_buf_free(msg_buf);
            break;
        }
    }
}

/*-------------------------------------------------------------------------------
Static function definitions
-------------------------------------------------------------------------------*/
/******************************************************************************
* Function Name    : text_for_sniffer_state
* Function Purpose : Returns pointer to text for sniffer state
* Input parameters : state, sniffer state
* Return value     : char * text
******************************************************************************/
static char * text_for_sniffer_state(sniffer_state_t state)
{
    switch (state)
    {
    case RN_SNIFFER_IDLE:       return "SNIFFER IDLE";
    case RN_SNIFFER_SEARCHING:  return "SNIFFER SEARCHING";
    case RN_SNIFFER_FOLLOWING:  return "SNIFFER FOLLOWING";
    default:                    return "UNDECODED";
    }
}

/******************************************************************************
* Function Name    : receive_rrc_register_cfm
* Function Purpose : Reception of RRC_REGISTER_CFM                 
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_register_cfm(msg_buf_t *msg_buf)
{
    uint32_t index = 1;
    uint8_t n_cfm;
    uint8_t n_cfm_read = 0;
    uint8_t rn_addr = 0xFF;

    n_cfm = msg_return_uint8(msg_buf, index);
    index++;

    while (n_cfm_read < n_cfm)
    {
        rn_addr =  msg_return_uint8(msg_buf, index);
        index++;

        if (rn_addr > RN_MAX_ADDRESS)
        {
            rn_log_err_msg("receive_rrc_register_cfm",msg_buf);
            return;
        }
        rn_management_data[rn_addr].rn_registration_data.rn_registration_state =  
            (rn_registration_state_t) msg_return_uint8(msg_buf, index);
        index++;
        rn_management_data[rn_addr].rn_registration_data.eqmid = msg_return_uint32(msg_buf, index);
        index +=4;
        n_cfm_read++;
    }

}

/******************************************************************************
* Function Name    : any_reg_change
* Function Purpose : Check if there has been any registration state change since 
*                    last report
* Input parameters : void
* Return value     : bool, true if any change occured
******************************************************************************/
static bool any_reg_change(void)
{
    uint8_t addr;

    for (addr = RN_MIN_ADDRESS; addr <= RN_MAX_ADDRESS; addr++)
    {
        if(rn_management_data[addr].rn_registration_data.rn_registration_state != rn_management_data[addr].rn_registration_data.reported_state)
        {
            return true;
        }
    }

    return false;
}

/******************************************************************************
* Function Name    : console_write_registrations
* Function Purpose : Send registration mirror to console                  
* Input parameters : void
* Return value     : void
******************************************************************************/

static void console_write_registrations(void)
{
    char        line_text[512];
    char        var_text[64];
    uint8_t     rn_address;
        
    console_write("---------------------------\r\n");
    console_write("Mirrored registration data:\r\n");
    console_write("Address    EQMID State     \r\n");
    
    for (rn_address = RN_MIN_ADDRESS; rn_address <= RN_MAX_ADDRESS; rn_address++)
    {
        // Sniffer logs changes different from master/slave:
        // FIXME: HOW SHALL REGISTRATIONS BE DISPLAYED? 
        // In this implementation any registraion change is considered reported when it has been logged
        
        rn_management_data[rn_address].rn_registration_data.reported_state = rn_management_data[rn_address].rn_registration_data.rn_registration_state;

        sprintf(line_text,"");

        sprintf(var_text,"%7d ",rn_address);
        strcat(line_text,var_text);

        sprintf(var_text,"%8d ",rn_management_data[rn_address].rn_registration_data.eqmid);
        strcat(line_text,var_text);

        switch(rn_management_data[rn_address].rn_registration_data.rn_registration_state)
        {
        case RN_NONEX:                  sprintf(var_text,"NONEX    ");break;
        case RN_REGISTERED_CONNECTED:   sprintf(var_text,"CONNECTED");break;
        case RN_REGISTERED_ABSENT:      sprintf(var_text,"ABSENT   ");break;
        }

        strcat(line_text,var_text);
        strcat(line_text,"\r\n");
        console_write(line_text);
    }
}

/******************************************************************************
* Function Name    : receive_rrc_reg_info
* Function Purpose : receive RRC_REG_INFO message                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/

static void receive_rrc_reg_info(msg_buf_t *msg_buf)
{
     uint32_t index     = 1;    // Message type is skipped
     uint8_t rn_addr;
     uint8_t msg_data; 
     uint8_t nibble_1;
     uint8_t nibble_2;

     rn_addr = msg_return_uint8_i(msg_buf, &index);

     while ((index < msg_buf->size) && (rn_addr < RN_MAX_ADDRESS))
     {
         msg_data = msg_return_uint8_i(msg_buf, &index);

         nibble_1 = (msg_data >> 4) &0x0F;
         nibble_2 = (msg_data >> 0) &0x0F;

         rn_management_data[rn_addr].rn_registration_data.rn_registration_state = (rn_registration_state_t) nibble_1;
         rn_addr++;

         rn_management_data[rn_addr].rn_registration_data.rn_registration_state = (rn_registration_state_t) nibble_2;
         rn_addr++;
     }

     if(any_reg_change())
     {
         console_write_registrations();
     }
}

/******************************************************************************
* Function Name    : receive_rrc_reg_info
* Function Purpose : receive RRC messages                  
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

static void receive_rrc(msg_buf_t *msg_buf)
{
    msg_buf->mt = msg_buf->data[0];

    switch (msg_buf->mt)
    {

    case RRC_REGISTER_CFM:
        receive_rrc_register_cfm(msg_buf);
        break;
        
    case RRC_REG_INFO:
        receive_rrc_reg_info(msg_buf);
        break;
    
    case RRC_REGISTER_REQ:
        rn_log_text_msg("RRC_REGISTER_REQ received in rn_sniffer",msg_buf);
        break;

    case RRC_DEREGISTER_REQ:
        rn_log_text_msg("RRC_UNIT_REPORT received in rn_sniffer",msg_buf);
        break;

    case RRC_UNIT_REPORT:
        rn_log_text_msg("RRC_UNIT_REPORT received in rn_sniffer",msg_buf);
        break;

    case RRC_TIMESLOT_REPORT:
        rn_log_text_msg("RRC_TIMESLOT_REPORT received in rn_sniffer",msg_buf);
        break;
        
    case RRC_APOLL_REQ:
        //rn_log_text_msg("RRC_APOLL_REQ received in rn_sniffer",msg_buf);
        break;

    case RRC_APOLL_RSP:
        rn_log_text_msg("RRC_APOLL_RSP received in rn_sniffer",msg_buf);
        break;

    case RRC_DEREGISTER_CFM:
        //printf("RRC_DEREGISTER_CFM  %02X \n\r",msg_buf->mt                  );
        break;
    
    default:
        rn_log_text_msg("RRC message received in rn_sniffer",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_sync
* Function Purpose : Reception of messages on SYNC interface                 
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_sync(msg_buf_t *msg_buf)
{
    uint16_t    frame_number;

    switch (msg_buf->mt)
    {
    case SYNC_FRAME:
        frame_number = msg_return_uint16(msg_buf,0);
        sniffer_data.state_timer++;
        break;

    case SYNC_RN_TIMER:
        break;

    default:
        //rn_log_err_msg("Ignored SYNC message in RN_SNIFFER",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_plc
* Function Purpose : Reception of messages on PLC interface                 
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_plc(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {
    case PLC_PHY_SLAVE_SEARCHING:
        set_sniffer_state(RN_SNIFFER_SEARCHING);
        break;

    case PLC_PHY_SLAVE_NORMAL:
        set_sniffer_state(RN_SNIFFER_FOLLOWING);
        break;

    default:
        rn_log_err_msg("RN_SNIFFER: Ignored PLC message: ",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_rni
* Function Purpose : Reception of messages on RNI interface                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rni(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {
    case RNI_LIST:
        console_write(         "Main state:          RN_SNIFFER\r\n");

        sprintf(console_text,"Sub state:           %s\r\n",text_for_sniffer_state(sniffer_data.state));console_write(console_text);
        sprintf(console_text,"Sub state timer:     %d (frames)\r\n",sniffer_data.state_timer);    console_write(console_text);
        sprintf(console_text,"RNID:                %d\r\n",sniffer_data.rnid);        console_write(console_text);
        
        console_write_registrations();

        break;

    case RNI_SNIFF:
            break;

    default:
        rn_log_err_msg("Ignored RNI message in RN_SNIFFER",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_rpi
* Function Purpose : Reception of messages on RPI interface                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rpi(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {
    case RPI_STARTED:
        break;

    case RPI_STOPPED:
        break;

    default:
        break;
    }
}
/******************************************************************************
* Function Name    : set_sniffer_state
* Function Purpose : Set sniffer state                 
* Input parameters : sniffer_state_t
* Return value     : void
******************************************************************************/
static void set_sniffer_state(sniffer_state_t state)
{
    if(sniffer_data.state != state)
    {
        sprintf(console_text,"%s -> %s",text_for_sniffer_state(sniffer_data.state),text_for_sniffer_state(state));
        rn_log_vtext(console_text);
    }

    sniffer_data.state        = state;
    sniffer_data.state_timer    = 0;

    if(RN_SNIFFER_FOLLOWING == sniffer_data.state)
    {
        rn_rlc_sniffer_conf(true);  
    }
    else if(RN_SNIFFER_IDLE == sniffer_data.state)
    {
        rn_mac_cc_idle_conf();              // Will also configer PHY slots
        rn_rlc_sniffer_conf(false);         // Is this necessary?  
    }

    rn_rlc_address_conf (RN_INVALID_ADDRESS);   // Listener does not have an address in the system

    rn_rlc_router_conf(false);      // No message routing in any state in sniffer
   
}
