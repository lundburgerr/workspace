/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_master.c
* Author            : Per Erik Sundvisson
* Description       : Implementation of RN master functions
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
            
#define RN_REGISTERED_CONNECTED_TMO     (RN_BASE_TIME_3s)
#define RN_REGISTERED_ABSENT_TMO        (RN_BASE_TIME_3s)

/*-------------------------------------------------------------------------------
RTOS dependencies
-------------------------------------------------------------------------------*/
#include "rtos.h"

/*-------------------------------------------------------------------------------
Project includes
-------------------------------------------------------------------------------*/

#include "rni.h"
#include "rn_manager.h"
#include "rn_master.h"
#include "rn_rlc_cc.h"
#include "rn_mac_cc.h"
#include "rn_phy.h"
#include "rn_log.h"

/*-------------------------------------------------------------------------------
Static variable definitions
-------------------------------------------------------------------------------*/
/******************************************************************************
*     Strings for console or log outputs
*******************************************************************************/
#define MAX_CHARS 512
static char console_text[MAX_CHARS+1];
static char log_text[MAX_CHARS+1];          // FIXME: The text strings could probably be merged
static char var_text[32];
/******************************************************************************
*     Master data
*******************************************************************************/
typedef enum
{
    RN_MASTER_STOPPED,
    RN_MASTER_STARTED
} master_state_t;

typedef struct
{
    master_state_t      master_state;
    uint8_t             default_address;
    uint8_t             rnid;  
    uint32_t            eqmid;
    RTOS_QUEUE_ID_T 	ap_rni_q_id;
} master_data_t;

static master_data_t master_data;
/******************************************************************************
*     RN Management data
*******************************************************************************/
static rn_management_data_t rn_management_data[RN_MAX_ADDRESS+1];           // Index corresponds to RN-address

/******************************************************************************
*     Poll and statistics data
*******************************************************************************/
static bool         apoll_on = false;        // Switch addressed poll on/off
static uint8_t      apoll_interval = 50;     // Default 50 frames
static uint8_t      apoll_counter = 0;       // Used to poll registered addresses

static bool         bpoll_on = false;        // Switch Broadcast poll on/off
static uint8_t      bpoll_interval = 50;     // Default 50 frames

#define RN_MAX_RESPONSE_TIME 31
typedef struct 
{
    uint32_t    apoll_req_count;
    uint32_t    apoll_rsp_count;
    uint8_t     apoll_rsp_time[RN_MAX_RESPONSE_TIME+1];       // Bucket statistics for response time
    
    uint32_t    bpoll_rsp_count;
    uint8_t     bpoll_rsp_time[RN_MAX_RESPONSE_TIME+1];       // Bucket statistics for response time

} poll_stat_t;

static poll_stat_t  poll_stat[RN_MAX_ADDRESS+1];
static uint32_t     bpoll_req_count;
/******************************************************************************
*     Synchronization and timing
*******************************************************************************/

static bool         ap_sync_on     = true; 

static uint32_t     rn_master_tick = 0;             // Used for RN timing

/*-------------------------------------------------------------------------------
Static function declarations
-------------------------------------------------------------------------------*/

static void master_data_init(uint32_t eqmid);
static bool any_connection_list_change(void);
static void update_connected_list(void);

static void receive_rpi(msg_buf_t *msg_buf);

static void receive_rni(msg_buf_t *msg_buf);
static void receive_rrc_unit_report(msg_buf_t *msg_buf);
static void receive_rrc_register_req(msg_buf_t *msg_buf);
static void receive_rrc_deregister_req(msg_buf_t *msg_buf);

static void broadcast_rrc_register_rej(uint32_t eqm);
static void broadcast_rrc_deregister_cfm(uint32_t e);

static void receive_rrc(msg_buf_t *msg_buf);

static void all_register_cfm(uint32_t frame_number);

static void broadcast_rrc_register_cfm(uint8_t n_addr, uint8_t *rn_address);
static void broadcast_rrc_reg_info(void);

static void state_timer_check(void);
static void receive_sync(msg_buf_t *msg_buf);

static void receive_plc(msg_buf_t *msg_buf);

static void send_rni_connected(uint8_t addr, uint32_t eqm, uint8_t r);
static void send_rni_disconnected(void);

static void ap_sync(uint32_t timer);

static void status_apoll(uint32_t frame);
static void status_bpoll(uint32_t frame);
static void apoll_stat_clear(void);
static void bpoll_stat_clear(void);
static void rn_stat(void);
static void rn_stat_remote(uint8_t rn_addr);

static void console_write_registrations(void);
static void remote_console_write_registrations(uint8_t remote_a);

/*-------------------------------------------------------------------------------
Global function definitions
-------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name    : rn_master_module_init
* Function Purpose : Initiation of data in module              
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_master_module_init(void)
{
    uint8_t RN_address;

    for (RN_address = RN_MIN_ADDRESS; RN_address <= RN_MAX_ADDRESS; RN_address++)
    {
        rn_management_data[RN_address].rn_registration_data.rn_registration_state   = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.reported_state          = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.eqmid                   = 0;
        rn_management_data[RN_address].state_timer                                    = 0;
    }
    
	return;
}

/******************************************************************************
* Function Name    : rn_master_module_start
* Function Purpose : Start of module                
* Input parameters : start phase 
* Return value     : void
******************************************************************************/
void rn_master_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}
/******************************************************************************
* Function Name    : rn_master
* Function Purpose : Implementation of RM master functions, 
*                    executing in RN manager task                
* Input parameters : rnm_input_queue, control input queue for task 
*                    client_queue, registered RNI client queue ID
*                    rnid, Radio Network ID specified by client
*                    eqmid, Equipment ID specified by client
* Return value     : void
******************************************************************************/

void rn_master(MessageQ_Handle rnm_input_queue, RTOS_QUEUE_ID_T client_queue, uint8_t rnid, uint32_t eqmid)
{
    msg_buf_t   *msg_buf = NULL;
  
    master_data_init(eqmid);
    
    master_data.ap_rni_q_id     = client_queue;
    master_data.rnid            = rnid;
    master_data.eqmid           = eqmid;
    master_data.default_address = RN_MASTER_ADDRESS;
    master_data.master_state    = RN_MASTER_STOPPED;

    rn_management_data[master_data.default_address].rn_registration_data.rn_registration_state   = RN_REGISTERED_CONNECTED;
    rn_management_data[master_data.default_address].rn_registration_data.reported_state          = RN_NONEX;
    rn_management_data[master_data.default_address].rn_registration_data.eqmid                   = eqmid;

    rn_management_data[master_data.default_address].state_timer  = 0;               

    rn_rlc_router_conf (true);
    rn_rlc_sniffer_conf(false);

    rn_mac_cc_master_conf(master_data.rnid);
    rn_rlc_address_conf (master_data.default_address);

    //send_rni_connected(master_data.default_address, master_data.eqmid, master_data.rnid);
    //update_connected_list();

    for( ;; )
    {
    	RTOS_QUEUE_RECEIVE(rnm_input_queue, &msg_buf, portMAX_DELAY );
        
        switch(msg_buf->pd)
        {
        case PD_RNI:
            rn_log_msg_receive(RN_RNM, msg_buf);

            if (RNI_UNDEFINE == msg_buf->mt)
            {
                rn_rlc_router_conf (false);
                rn_rlc_sniffer_conf(false);

                rn_mac_cc_idle_conf();
                rn_rlc_address_conf (RN_INVALID_ADDRESS);

                msg_buf_free(msg_buf);
                send_rni_disconnected();
                return;  // FIXME: Some cleanup maybe...
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
            rn_log_err_msg("Ignored Protocol Discriminator in RN_MASTER",msg_buf);
            msg_buf_free(msg_buf);
            break;
        }

        update_connected_list();
    }
}
/*-------------------------------------------------------------------------------
Static function definitions
-------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name    : master_data_init
* Function Purpose : Initiates network data, e.g. registrations
* Input parameters : eqmid for master
* Return value     : void
******************************************************************************/
static void master_data_init(uint32_t eqmid)
{
    uint8_t RN_address;

    for (RN_address = RN_MIN_ADDRESS; RN_address <= RN_MAX_ADDRESS; RN_address++)
    {
        rn_management_data[RN_address].rn_registration_data.rn_registration_state    = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.eqmid                    = 0;
        rn_management_data[RN_address].state_timer                                   = 0;
    }

    rn_management_data[RN_MASTER_ADDRESS].rn_registration_data.rn_registration_state = RN_REGISTERED_CONNECTED;
    rn_management_data[RN_MASTER_ADDRESS].rn_registration_data.eqmid                 = eqmid;
    rn_management_data[RN_MASTER_ADDRESS].state_timer                                = 0;
}

/******************************************************************************
* Function Name    : any_connection_list_change
* Function Purpose : Check if there has been any connection list change
*                    since last report
* Input parameters : void
* Return value     : bool, true if any change occured
******************************************************************************/
static bool any_connection_list_change(void)
{
    uint8_t addr;

    for (addr = RN_MIN_ADDRESS; addr <= RN_MAX_ADDRESS; addr++)
    {
        switch(rn_management_data[addr].rn_registration_data.rn_registration_state)
        {
        case RN_REGISTERED_CONNECTED:

            if(RN_REGISTERED_CONNECTED != rn_management_data[addr].rn_registration_data.reported_state)
            {
                return true;
            }
            break;

        case RN_REGISTERED_ABSENT:
        case RN_NONEX:
            if(RN_REGISTERED_CONNECTED == rn_management_data[addr].rn_registration_data.reported_state)
            {
                return true;
            }
            break;
        }
    }
    return false;
}

/******************************************************************************
* Function Name    : update_connected_list
* Function Purpose : Check if there has been any connection list change, then 
*                    send RNI_CONNECTED_LIST to registered application
* Input parameters : void
* Return value     : void
******************************************************************************/
static void update_connected_list(void)
{
    msg_buf_t   *msg_buf = NULL;
    uint8_t     rn_address;
    uint8_t     n_addresses;
    uint8_t     index;

    if(RN_MASTER_STOPPED == master_data.master_state)
    {
    	return;
    }

    if(!any_connection_list_change())
    {
        return;
    }

    if(NULL == master_data.ap_rni_q_id)
    {
        rn_log_err(">>>>> No RNI receiver defined");
        return;
    }
    msg_buf = msg_buf_alloc(12);

    msg_buf->pd             = PD_RNI;
    msg_buf->mt             = RNI_CONNECTED_LIST;
   
    n_addresses = 0;        
    
    index       = 1;       // Here is where the list starts
    
    for (rn_address = RN_MIN_ADDRESS; rn_address <= RN_MAX_ADDRESS; rn_address++)
    {
        rn_management_data[rn_address].rn_registration_data.reported_state = rn_management_data[rn_address].rn_registration_data.rn_registration_state;

        if(rn_management_data[rn_address].rn_registration_data.rn_registration_state == RN_REGISTERED_CONNECTED)
        {
            msg_buf->data[index] = rn_address;
            n_addresses++;
            index++;
        }
    }

    msg_buf->data[0]    = n_addresses;
    msg_buf->size       = n_addresses+1;

    rn_log_msg_send(RN_RNM,msg_buf);
    //MessageQ_put(master_data.ap_rni_q_id,msg_buf);
    rtos_send(master_data.ap_rni_q_id,msg_buf);
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
        rn_receive_rpi(msg_buf);
        //rn_log_err_msg("receive_rpi MESSAGE not implemented yet",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_rni
* Function Purpose : Receive messages on RNI               
* Input parameters : void
* Return value     : void
******************************************************************************/
static void receive_rni(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {

    case RNI_START: // Already ordered
        break;

    case RNI_LIST:
        console_write("\r\n");
        console_write(        "Main state:          RN_MASTER\r\n");
        //                    -------              --------
        if(RN_MASTER_STOPPED == master_data.master_state)
        {
            console_write(        "Sub state:           RN_MASTER_STOPPED\r\n");
            //                    -------              --------

        }
        else
        {
            console_write(        "Sub state:           RN_MASTER_STARTED\r\n");
            //                    -------              --------
        }

        sprintf(console_text,"Default address:     %d\r\n",master_data.default_address);
        console_write(console_text);
        sprintf(console_text,"RNID:                %d\r\n",master_data.rnid);
        console_write(console_text);
        sprintf(console_text,"EQMID:               %d\r\n",master_data.eqmid);
        console_write(console_text);
       
        console_write_registrations();
        sprintf(console_text,"rn_master_tick:      %u \r\n",rn_master_tick); console_write(console_text);
        break;

    case RNI_STAT:
        rn_stat();
        break;

    case RNI_BPOLL_ON:
        bpoll_on = true;
        bpoll_stat_clear();
        
        if(0 < msg_return_data_size(msg_buf))
        {
            bpoll_interval = msg_return_uint8(msg_buf,0);

            if(bpoll_interval < 5)
            {
                bpoll_interval = 5;        // Not accepted
            }
        }
        else
        {
            bpoll_interval = 50;
        }

        sprintf(console_text,"Broadcast poll switched ON with interval %d RN tick\r\n",bpoll_interval);
        console_write(console_text);

        break;

    case RNI_BPOLL_OFF:
        bpoll_on = false;
        console_write("Broadcast poll switched OFF\r\n");
        break;

    case RNI_HOHO:
        sprintf(log_text,"rn_master_tick = %d \r\n",rn_master_tick);console_write(log_text);
        broadcast_rrc_status_req(rn_master_tick);
        break;

    case RNI_APOLL_ON:
        apoll_on = true;
        apoll_stat_clear();

        if(0 < msg_return_data_size(msg_buf))
        {
            apoll_interval = msg_return_uint8(msg_buf,0);

            if(apoll_interval < 5)
            {
                apoll_interval = 5;        // Not accepted
            }
        }
        else
        {
            apoll_interval = 50;
        }

        sprintf(console_text,"Addressed poll switched ON with interval %d RN tick\r\n",apoll_interval);
        console_write(console_text);

        break;

    case RNI_APOLL_OFF:
        apoll_on = false;
        console_write("Addressed poll switched OFF");
        break;

    case RNI_SYNC_ON:
        ap_sync_on = true;
        break;

    case RNI_SYNC_OFF:
        ap_sync_on = false;
        break;

    default:
        rn_log_text_msg("Ignored RNI message in RN_MASTER: ",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_rrc_unit_report
* Function Purpose : Receive RRC_UNIT_REPORT               
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_unit_report(msg_buf_t *msg_buf)
{
    uint32_t    received_eqmid;
    uint8_t     slave_address;
    char        log_text[64];

    received_eqmid  = msg_return_uint32(msg_buf,1);
    slave_address   = msg_return_rn_sender(msg_buf);

    if (slave_address < RN_MIN_SLAVE_ADDRESS)
    {
        sprintf(log_text,"Slave address (%d) out of range, EQMID = %d",slave_address,received_eqmid);
        rn_log_vtext(log_text);
        return; 
    }

    if (slave_address > RN_MAX_SLAVE_ADDRESS)
    {
        sprintf(log_text,"Slave address (%d) out of range, EQMID = %d",slave_address,received_eqmid);
        rn_log_vtext(log_text);
        return; 
    }

    if(received_eqmid == rn_management_data[slave_address].rn_registration_data.eqmid)
    {
        rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_REGISTERED_CONNECTED;
        rn_management_data[slave_address].state_timer = 0;
    }
    else
    {
        sprintf(log_text,"Unit report inconsistent, report discarded",slave_address);
        rn_log_vtext(log_text);

        sprintf(log_text,"Unit report says address = %d, EQMID = %d",slave_address,received_eqmid);
        rn_log_vtext(log_text);

        console_write_registrations();

       /* rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_NONEX;
        rn_management_data[slave_address].state_timer = 0;*/
    }
}

/******************************************************************************
* Function Name    : receive_rrc_register_req
* Function Purpose : Receive RRC_REGISTER_REQ               
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_register_req(msg_buf_t *msg_buf)
{
    uint32_t    received_eqmid;
    uint8_t     slave_address;
    uint8_t     rn_addr[1]; 
    char        log_text[64];
    
    received_eqmid = msg_return_uint32(msg_buf,1);

    // First check if the eqmid is already registered, then confirm the address and set as registered and connected

    for (slave_address = RN_MIN_SLAVE_ADDRESS; slave_address <= RN_MAX_SLAVE_ADDRESS; slave_address++)
    {
        if(received_eqmid == rn_management_data[slave_address].rn_registration_data.eqmid)
        {
            rn_addr[0] = slave_address;
            rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_REGISTERED_CONNECTED;
            rn_management_data[slave_address].state_timer = 0;

            broadcast_rrc_register_cfm(1,rn_addr);
            broadcast_rrc_reg_info();
            return;                             // Old registration confirmed
        }
    }

    // Otherwise this is a new registration, find free address and confirm

    for (slave_address = RN_MIN_SLAVE_ADDRESS; slave_address <= RN_MAX_SLAVE_ADDRESS; slave_address++)
    {
        if(RN_NONEX == rn_management_data[slave_address].rn_registration_data.rn_registration_state)
        {
            rn_addr[0] = slave_address;
            rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_REGISTERED_CONNECTED;
            rn_management_data[slave_address].rn_registration_data.eqmid = received_eqmid;
            rn_management_data[slave_address].state_timer = 0;

            broadcast_rrc_register_cfm(1,rn_addr);
            broadcast_rrc_reg_info();
            return;                             // New registration confirmed
        }
    }
    
    sprintf(log_text,"RN Master found no free address for EQMID = %d",received_eqmid);
    rn_log_vtext(log_text);

    console_write_registrations();

    broadcast_rrc_register_rej(received_eqmid);
}

/******************************************************************************
* Function Name    : receive_rrc_deregister_req
* Function Purpose : Receive RRC_DEREGISTER_REQ               
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_deregister_req(msg_buf_t *msg_buf)
{
    uint8_t     slave_address;
    uint32_t    received_eqmid;
    
    received_eqmid = msg_return_uint32(msg_buf,1);

    // Check if the eqmid is registered, then remove the registration.

    for (slave_address = RN_MIN_SLAVE_ADDRESS; slave_address <= RN_MAX_SLAVE_ADDRESS; slave_address++)
    {
        if(received_eqmid == rn_management_data[slave_address].rn_registration_data.eqmid)
        {
            rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_NONEX;
            rn_management_data[slave_address].state_timer = 0;
        }
    }

    broadcast_rrc_deregister_cfm(received_eqmid);
    broadcast_rrc_reg_info();

    console_write_registrations();
}
/******************************************************************************
* Function Name    : broadcast_rrc_register_rej
* Function Purpose : Broadcast RRC_REGISTER_REJ               
* Input parameters : eqm, rejected EQMID
* Return value     : void
******************************************************************************/
static void broadcast_rrc_register_rej(uint32_t eqm)
{
     msg_buf_t *reg_rej;
     uint32_t index = 0; 

     reg_rej = msg_buf_alloc(5);

     msg_put_uint8_i(reg_rej,&index,RRC_REGISTER_REJ); 
     msg_put_uint32_i(reg_rej,&index,eqm);
    
     reg_rej->size = index;

     rn_rccr_broadcast(reg_rej);

}

/******************************************************************************
* Function Name    : broadcast_rrc_deregister_cfm
* Function Purpose : Broadcast RRC_DEREGISTER_CFM               
* Input parameters : e, deregistered EQMID
* Return value     : void
******************************************************************************/
static void broadcast_rrc_deregister_cfm(uint32_t e)
{
    msg_buf_t   *dereg_cfm_message;

    uint32_t index = 0;         

    dereg_cfm_message = msg_buf_alloc(5);

    msg_put_uint8_i  (dereg_cfm_message,&index,RRC_DEREGISTER_CFM); 
    msg_put_uint32_i (dereg_cfm_message,&index,e);
    msg_set_data_size(dereg_cfm_message,index);

    rn_rccr_broadcast(dereg_cfm_message);
}

/******************************************************************************
* Function Name    : receive_rrc
* Function Purpose : Receive RRC messages                
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

static void receive_rrc(msg_buf_t *msg_buf)
{
    msg_buf_t *rni_cmd;
 
    uint8_t mt;
    uint32_t response_time;
    uint32_t time_stamp;
    uint8_t rn_addr;
    mt = msg_buf->data[0]; 
    

    switch (mt)
    {
    case RRC_REGISTER_REQ:
        receive_rrc_register_req(msg_buf);
        break;

    case RRC_DEREGISTER_REQ:
        receive_rrc_deregister_req(msg_buf);
        break;

    case RRC_UNIT_REPORT:
        receive_rrc_unit_report(msg_buf);
        break;

    case RRC_TIMESLOT_REPORT:
        break;

    case RRC_APOLL_REQ:
        receive_rrc_apoll_req(msg_buf,rn_management_data[RN_MASTER_ADDRESS].rn_registration_data.eqmid);
        break;

    case RRC_BPOLL_REQ:
        receive_rrc_bpoll_req(msg_buf,rn_management_data[RN_MASTER_ADDRESS].rn_registration_data.eqmid);
        break;

    case RRC_STATUS_REQ:
        receive_rrc_status_req(msg_buf,rn_management_data[RN_MASTER_ADDRESS].rn_registration_data.eqmid);
        break;

    case RRC_APOLL_RSP:
        /*sprintf(log_text,"RN address %d responded with delay %d RN ticks",msg_return_rn_sender(msg_buf),rn_master_tick -  msg_return_uint32(msg_buf,1));
        rn_log_vtext(log_text);*/

        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_master_tick - time_stamp;

      /*  sprintf(log_text,"RN address %d responded with delay %d RN ticks",msg_return_rn_sender(msg_buf),response_time);
        rn_log_vtext(log_text);*/
        poll_stat[msg_return_rn_sender(msg_buf)].apoll_rsp_count++;

        //response_time /= 5;

        if(response_time < RN_MAX_RESPONSE_TIME)
        {
            poll_stat[msg_return_rn_sender(msg_buf)].apoll_rsp_time[response_time]++;
        }
        else
        {
            poll_stat[msg_return_rn_sender(msg_buf)].apoll_rsp_time[RN_MAX_RESPONSE_TIME]++;
        }

        break;

    case RRC_BPOLL_RSP:
      /*  sprintf(log_text,"RN address %d responded with delay %d RN ticks",msg_return_rn_sender(msg_buf),rn_master_tick -  msg_return_uint32(msg_buf,1));
        rn_log_vtext(log_text);*/

        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_master_tick - time_stamp;
        poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_count++;

        if(response_time < RN_MAX_RESPONSE_TIME)
        {
            poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_time[response_time]++;
        }
        else
        {
            poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_time[RN_MAX_RESPONSE_TIME]++;
        }

        break;

    case RRC_STATUS_RSP:
        sprintf(log_text,"RN address %d responded with delay %d RN ticks",msg_return_rn_sender(msg_buf),rn_master_tick -  msg_return_uint32(msg_buf,1));
        console_write(log_text);
        break;
       
    case RRC_LIST:
        rn_addr = (msg_return_rn_sender(msg_buf));

        if(RN_MASTER_STOPPED == master_data.master_state)
        {
            rni_console_write(rn_addr,"MASTER STOPPED");
        }
        else
        {
            rni_console_write(rn_addr,"MASTER STARTED");
        }

        sprintf(console_text,"RNID: %d",master_data.rnid);
        rni_console_write(rn_addr,console_text);

        sprintf(console_text,"EQMID: %d",master_data.eqmid);
        rni_console_write(rn_addr,console_text);

        remote_console_write_registrations(rn_addr);

        break;

    case RRC_STAT:
        rn_addr = (msg_return_rn_sender(msg_buf));
        rn_stat_remote(rn_addr);
        break;

    case RRC_RNI_CMD:
        if(2 <= msg_return_data_size(msg_buf))
        {
            rni_cmd = msg_buf_copy(msg_buf);

            msg_put_pd(PD_RNI,rni_cmd);
            msg_put_mt(rni_cmd->data[1],rni_cmd);

            msg_buf_header_remove(rni_cmd,2);
            rtos_send(rnm_queue_id(),rni_cmd);
            rni_console_write(msg_return_rn_sender(msg_buf),">>>>> Received");
        }
        else
        {
            rni_console_write(msg_return_rn_sender(msg_buf),">>>>> Fail");
        }
        break;

    default:
        rn_receive_rrc(msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : all_register_cfm
* Function Purpose : Confirm all registrations                
* Input parameters : frame_number, used to set interval
* Return value     : void
******************************************************************************/

static void all_register_cfm(uint32_t rn_base_tick)
{ 
    uint8_t     rn_address[3];
    uint8_t     n_addr = 0;
    uint8_t     rn_index = RN_MIN_ADDRESS;
 
    if(RN_MASTER_STOPPED == master_data.master_state)
    {
        return;
    }

    if(!(rn_base_tick % RN_BASE_TIME_500ms)) // Defines time interval for perioic confirmations
    {
        //printf("Periodic RRC_REGISTER_CFM sent on broadcast channel, frame =  %d \n\r",frame_number);

        //rn_address[0] = 3;

        while(rn_index <= RN_MAX_SLAVE_ADDRESS)
        {
            if(RN_NONEX != rn_management_data[rn_index].rn_registration_data.rn_registration_state)
            {
                rn_address[n_addr] = rn_index;
                n_addr++;
            }

            if(n_addr > 2)
            {
                broadcast_rrc_register_cfm(n_addr,rn_address);
                n_addr = 0;
            }
            rn_index++;
        }
        if(n_addr > 0)
        {
            broadcast_rrc_register_cfm(n_addr,rn_address);
            n_addr = 0;
        }
        broadcast_rrc_reg_info();
    }
}

/******************************************************************************
* Function Name    : broadcast_rrc_register_cfm
* Function Purpose : Broadcast RRC_REGISTER_CFM                
* Input parameters : n_addr, number of addresses to confirm
*                    *rn_address, pointer to list of addresses
* Return value     : void
******************************************************************************/

static void broadcast_rrc_register_cfm(uint8_t n_addr, uint8_t *rn_address)
{
    msg_buf_t   *reg_cfm_message;
    uint8_t     rn_index = 0;
    uint8_t     data_index = 0;

    reg_cfm_message             = msg_buf_alloc(18); // Max number of application bytes to broadcast
    
    reg_cfm_message->data[data_index]    = RRC_REGISTER_CFM; 
    data_index++;
    
    reg_cfm_message->data[data_index]    = n_addr;
    data_index++;
    
    while(rn_index < n_addr)
    {
        msg_put_uint8(reg_cfm_message,data_index,rn_address[rn_index]);     // Put RN-address in message
        data_index++;         
        
        msg_put_uint8(reg_cfm_message,data_index,
            (uint8_t) rn_management_data[rn_address[rn_index]].rn_registration_data.rn_registration_state);
        data_index++;

        msg_put_uint32(reg_cfm_message,data_index, rn_management_data[rn_address[rn_index]].rn_registration_data.eqmid);
        data_index +=4;
        
        rn_index++;
    }

    reg_cfm_message->size       = data_index;

    rn_rccr_broadcast(reg_cfm_message);
}
/******************************************************************************
* Function Name    : broadcast_rrc_reg_info
* Function Purpose : Broadcast RRC_REG_INFO               
* Input parameters : void
* Return value     : void
******************************************************************************/
static void broadcast_rrc_reg_info(void)
{
    msg_buf_t       *reg_info_message;
    uint32_t        index = 0;
    uint8_t rn_addr = RN_MIN_ADDRESS;
    uint8_t msg_data; 
    uint8_t nibble_1;
    uint8_t nibble_2;

    reg_info_message             = msg_buf_alloc(2 + RN_MAX_ADDRESS/2 + 1); 

    msg_put_uint8_i(reg_info_message,&index,RRC_REG_INFO);
    msg_put_uint8_i(reg_info_message,&index,RN_MIN_ADDRESS);

    while (rn_addr <= RN_MAX_ADDRESS)
    {
        nibble_1 = (uint8_t) rn_management_data[rn_addr].rn_registration_data.rn_registration_state;
        rn_addr++;

        if((rn_addr <= RN_MAX_ADDRESS))
        {
            nibble_2 = (uint8_t) rn_management_data[rn_addr].rn_registration_data.rn_registration_state;
        }
        else
        {
            nibble_2 = RN_NONEX;
        }

        rn_addr++;

        msg_data = (nibble_1 << 4) + nibble_2;
            
        msg_put_uint8_i(reg_info_message,&index, msg_data);
    }

    msg_set_data_size(reg_info_message,index);

    rn_rccr_broadcast(reg_info_message);
}


/******************************************************************************
* Function Name    : state_timer_check
* Function Purpose : Increment and check state timer for each slave.
*                    Act if expired               
* Input parameters : void
* Return value     : void
******************************************************************************/

static void state_timer_check(void)
{
    uint8_t slave_address;
     
    rn_management_data[RN_MASTER_ADDRESS].state_timer++;

    for (slave_address = RN_MIN_SLAVE_ADDRESS; slave_address <= RN_MAX_SLAVE_ADDRESS; slave_address++)
    {
        rn_management_data[slave_address].state_timer++;

        if(RN_REGISTERED_CONNECTED == rn_management_data[slave_address].rn_registration_data.rn_registration_state)
        {
            if (rn_management_data[slave_address].state_timer > RN_REGISTERED_CONNECTED_TMO)
            {
                rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_REGISTERED_ABSENT;
                rn_management_data[slave_address].state_timer = 0;
            }
        }

        if(RN_REGISTERED_ABSENT == rn_management_data[slave_address].rn_registration_data.rn_registration_state)
        {
            if (rn_management_data[slave_address].state_timer > RN_REGISTERED_ABSENT_TMO)
            {
                rn_management_data[slave_address].rn_registration_data.rn_registration_state = RN_NONEX;
                rn_management_data[slave_address].state_timer = 0;
            }
        }
    }
}


/******************************************************************************
* Function Name    : receive_sync
* Function Purpose : Receive message on SYNC interface           
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

static void receive_sync(msg_buf_t *msg_buf)
{
    //uint16_t    frame_number;

    //static uint8_t test_address = 0;

    switch (msg_buf->mt)
    {
    case SYNC_FRAME:
        
        //frame_number = msg_return_uint16(msg_buf,RPC_FRAME_SYNC_FN_POS);
      /*  state_timer_check();
        all_register_cfm(frame_number);
        status_apoll(frame_number);
        status_bpoll(frame_number);*/
        break;

    case SYNC_RN_TIMER:

        //rn_master_tick = msg_return_uint32(msg_buf,0);

    	rn_master_tick++;
//    	if(!(rn_master_tick % 111))
//    	{
//    		sprintf(console_text,"%u \r\n",rn_master_tick);
//    		console_write(console_text);
//    	}
        state_timer_check();
        all_register_cfm(rn_master_tick);
        status_apoll(rn_master_tick);
        status_bpoll(rn_master_tick);

        if(ap_sync_on)
        {
            ap_sync(rn_master_tick); 
        }

        //spool_remote(rn_master_tick);

        break;

    default:
        break;
    }
}

/******************************************************************************
* Function Name    : receive_plc
* Function Purpose : Receive message on PLC interface           
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/

static void receive_plc(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {
    case PLC_PHY_MASTER:
        master_data.master_state = RN_MASTER_STARTED;
        send_rni_connected(master_data.default_address,master_data.eqmid,master_data.rnid);
        update_connected_list();
        break;
    case PLC_PHY_SLAVE_SEARCHING: 
        break;
    case PLC_PHY_SLAVE_NORMAL: 
        break;
    default:
        break;
    }
}

/******************************************************************************
* Function Name    : send_rni_connected
* Function Purpose : Sends RNI_CONNECTED message
* Input parameters : addr, registered address
*                    eqm,  registered EQMID (returned)
* Return value     : void
******************************************************************************/

static void send_rni_connected(uint8_t addr, uint32_t eqm, uint8_t rnid)
{
     msg_buf_t *rni_connected;
     uint32_t   index = 0;

     rni_connected = msg_buf_alloc(RNI_CONNECTED_SIZE);
     
     rni_connected->mt = RNI_CONNECTED;
     rni_connected->pd = PD_RNI;

     msg_put_uint32_i(rni_connected,&index,eqm);
     msg_put_uint8_i(rni_connected,&index,addr);
     msg_put_uint8_i(rni_connected,&index,rnid);

     //RTOS_QUEUE_SEND( slave_data.ap_rni_q, &rni_connected, 0U );
     rtos_send(master_data.ap_rni_q_id,rni_connected);
}

/******************************************************************************
* Function Name    : send_rni_disconnected
* Function Purpose : Sends RNI_DISCONNECTED message
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rni_disconnected(void)
{
     msg_buf_t *rni_connected;
     
     rni_connected = msg_buf_alloc(0);
     
     rni_connected->mt = RNI_DISCONNECTED;
     rni_connected->pd = PD_RNI;

     //RTOS_QUEUE_SEND( slave_data.ap_rni_q_id, &rni_connected, 0U );

     rtos_send(master_data.ap_rni_q_id,rni_connected);

}
/******************************************************************************
* Function Name    : ap_sync
* Function Purpose : Send sync message to registered RNI client
* Input parameters : timer
* Return value     : void
******************************************************************************/

static void ap_sync(uint32_t timer) 
{
    msg_buf_t *rni_sync = NULL;

    rni_sync = msg_buf_alloc(4);
    rni_sync->pd = PD_SYNC;
    rni_sync->mt = RNI_SYNC;
    
    msg_put_uint32(rni_sync,0,timer);

    rtos_send(master_data.ap_rni_q_id,rni_sync);
}

/******************************************************************************
* Function Name    : status_apoll
* Function Purpose : Test function for polling of other units in the system                  
* Input parameters : frame, used to define poll interval
* Return value     : void
******************************************************************************/

static void status_apoll(uint32_t frame)
{
    msg_buf_t   *status_req;
    uint32_t    data_index;
    uint8_t     lpoll_addr;

    if(!apoll_on)
    {
        return;
    }
     
    if(0 != (frame % apoll_interval))       // Check if poll time
    {
        return;
    }

    apoll_counter++;
    apoll_counter %= RN_MAX_NO_OF_SLAVES;

    lpoll_addr = apoll_counter + 1;

    if (lpoll_addr != RN_MASTER_ADDRESS)
    {
        if (rn_management_data[lpoll_addr].rn_registration_data.rn_registration_state != RN_NONEX)
        {
            status_req = msg_buf_alloc(20); // Max number of application bytes to send
            data_index = 0;

            msg_put_uint8_i     (status_req,&data_index,RRC_APOLL_REQ);
            msg_put_uint32_i    (status_req,&data_index,rn_master_tick);
            msg_set_data_size   (status_req,data_index);
            
            poll_stat[lpoll_addr].apoll_req_count++;

            rn_rccr_send(lpoll_addr, status_req);

            //rni_console_write(lpoll_addr,"Remote text test");
        }
    }
}

/******************************************************************************
* Function Name    : status_bpoll
* Function Purpose : Test function for polling of other units in the system                  
* Input parameters : frame, used to define poll interval
* Return value     : void
******************************************************************************/
static void status_bpoll(uint32_t frame)
{
    msg_buf_t   *status_req;
    uint32_t    data_index;
  /*  uint8_t     lpoll_addr;*/

    if(!bpoll_on)
    {
        return;
    }

    if(0 != (frame % bpoll_interval))       // Check if poll time
    {
        return;
    }

    status_req = msg_buf_alloc(20); // Max number of application bytes to send
    data_index = 0;

    msg_put_uint8_i     (status_req,&data_index,RRC_BPOLL_REQ);
    msg_put_uint32_i    (status_req,&data_index,rn_master_tick);
    msg_set_data_size   (status_req,data_index);
    
    bpoll_req_count++;
    rn_rccr_broadcast(status_req);

}

/******************************************************************************
* Function Name    : apoll_stat_clear
* Function Purpose : Clear apoll statistics              
* Input parameters : void
* Return value     : void
******************************************************************************/
static void apoll_stat_clear(void)
{
    uint8_t i,j;

    for(i=RN_MIN_ADDRESS;i <= RN_MAX_ADDRESS;i++)
    {
        poll_stat[i].apoll_req_count = 0;
        poll_stat[i].apoll_rsp_count = 0;

        for(j=0;j <= RN_MAX_RESPONSE_TIME;j++)
        {
            poll_stat[i].apoll_rsp_time[j] = 0;
        }
    }
}

/******************************************************************************
* Function Name    : bpoll_stat_clear
* Function Purpose : Clear bpoll statistics              
* Input parameters : void
* Return value     : void
******************************************************************************/
static void bpoll_stat_clear(void)
{
    uint8_t i,j;

    for(i=RN_MIN_ADDRESS;i <= RN_MAX_ADDRESS;i++)
    {
        bpoll_req_count = 0;
        poll_stat[i].bpoll_rsp_count = 0;

        for(j=0;j <= RN_MAX_RESPONSE_TIME;j++)
        {
            poll_stat[i].bpoll_rsp_time[j] = 0;
        }
    }
}

/******************************************************************************
* Function Name    : rn_stat
* Function Purpose : RN statistics print              
* Input parameters : void
* Return value     : void
******************************************************************************/
static void  rn_stat(void)
{
    uint8_t i,j;

   /* if (!ap_sim_data.connected)
    { 
        sprintf(log_text,"Not connected");
        rn_log_vtext(log_text);
        return;
    }
*/
    for(i=RN_MIN_ADDRESS;i<=RN_MAX_ADDRESS;i++)
    {
        if(i != master_data.default_address)
        {
            if(rn_management_data[i].rn_registration_data.rn_registration_state != RN_NONEX)
            {
                sprintf(log_text,"A = %d \r\n",i); console_write(log_text);

                sprintf(log_text,"apoll_req  count %d\r\n",poll_stat[i].apoll_req_count); console_write(log_text);

                sprintf(log_text,"apoll_rsp  count %d\r\n",poll_stat[i].apoll_rsp_count);
                console_write(log_text);

                sprintf(log_text,"apoll diff count %d\r\n",poll_stat[i].apoll_req_count - poll_stat[i].apoll_rsp_count);
                console_write(log_text);

                sprintf(log_text,"Delay Intervals\r\n");console_write(log_text);

                sprintf(log_text,"    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15\r\n"); console_write(log_text);
                //                ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
                sprintf(log_text,"");

                for (j=0;j<=15;j++)
                {
                    sprintf(var_text,"%5d ",poll_stat[i].apoll_rsp_time[j]);
                    strcat(log_text,var_text);
                }
                strcat(log_text,"\r\n"); strcat(log_text,"\r\n");
                console_write(log_text);

                sprintf(log_text,"   16    17    18    19    20    21    22    23    24    25    26    27    28    29    30   >30 \r\n"); console_write(log_text);
                //                ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
                
                sprintf(log_text,"");

                 for (j=16;j<=31;j++)
                {
                    sprintf(var_text,"%5d ",poll_stat[i].apoll_rsp_time[j]);
                    strcat(log_text,var_text);
                }
                strcat(log_text,"\r\n"); strcat(log_text,"\r\n");
                console_write(log_text);

                sprintf(log_text,"bpoll_req  count %d\r\n",bpoll_req_count); console_write(log_text);

                sprintf(log_text,"bpoll_rsp  count %d\r\n",poll_stat[i].bpoll_rsp_count);
                console_write(log_text);

                sprintf(log_text,"bpoll diff count %d\r\n",bpoll_req_count - poll_stat[i].bpoll_rsp_count);
                console_write(log_text);

                sprintf(log_text,"Delay Intervals\r\n");console_write(log_text);

                sprintf(log_text,"    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15\r\n"); console_write(log_text);
                //                ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
                sprintf(log_text,"");

                for (j=0;j<=15;j++)
                {
                    sprintf(var_text,"%5d ",poll_stat[i].bpoll_rsp_time[j]);
                    strcat(log_text,var_text);
                }
                strcat(log_text,"\r\n"); strcat(log_text,"\r\n");
                console_write(log_text);

                sprintf(log_text,"   16    17    18    19    20    21    22    23    24    25    26    27    28    29    30   >30 \r\n"); console_write(log_text);
                //                ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
                sprintf(log_text,"");

                 for (j=16;j<=31;j++)
                {
                    sprintf(var_text,"%5d ",poll_stat[i].bpoll_rsp_time[j]);
                    strcat(log_text,var_text);
                }
                strcat(log_text,"\r\n");
                console_write(log_text);
            }
        }
    }
}
/******************************************************************************
* Function Name    : rn_stat_remote
* Function Purpose : RN statistics print, remote              
* Input parameters : void
* Return value     : void
******************************************************************************/
static void  rn_stat_remote(uint8_t rn_addr)
{
    uint8_t i,j;
    uint32_t    max_delay = 0;  
    uint32_t    min_delay = 31;
    uint32_t    sum_delay = 0;
    
    for(i=RN_MIN_ADDRESS;i<=RN_MAX_ADDRESS;i++)
    {
        if(i != master_data.default_address)
        {
            if(rn_management_data[i].rn_registration_data.rn_registration_state != RN_NONEX)
            {
                sprintf(log_text,"A = %d\r\n---",i); rni_console_write(rn_addr,log_text);

                sprintf(log_text,"apoll cnt %d",poll_stat[i].apoll_req_count); rni_console_write(rn_addr,log_text);
                sprintf(log_text,"rsp   cnt %d",poll_stat[i].apoll_rsp_count);rni_console_write(rn_addr,log_text);
                sprintf(log_text,"diff  cnt %d",poll_stat[i].apoll_req_count - poll_stat[i].apoll_rsp_count);rni_console_write(rn_addr,log_text);

                for (j=0;j<=31;j++)
                {
                    if (poll_stat[i].apoll_rsp_time[j] > 0)
                    {
                        max_delay = j;
                        sum_delay += j*poll_stat[i].apoll_rsp_time[j];
                    }
                }
             
               
                for (j=31;j>0;j--)
                {
                    if (poll_stat[i].apoll_rsp_time[j] > 0)
                    {
                         min_delay = j; 
                    }
                }

                if(poll_stat[i].apoll_rsp_count > 0)
                {
                    sprintf(log_text,"Min  delay %d",min_delay);                                rni_console_write(rn_addr,log_text);
                    sprintf(log_text,"Max  delay %d",max_delay);                                rni_console_write(rn_addr,log_text); 
                    sprintf(log_text,"Sum  delay %d",sum_delay);                                rni_console_write(rn_addr,log_text); 
                    sprintf(log_text,"Mean delay %d",sum_delay/poll_stat[i].apoll_rsp_count);   rni_console_write(rn_addr,log_text);        
                }
                sprintf(log_text,"---");                                        rni_console_write(rn_addr,log_text);
                sprintf(log_text,"bpoll cnt %d",bpoll_req_count);               rni_console_write(rn_addr,log_text);
                sprintf(log_text,"rsp   cnt %d",poll_stat[i].bpoll_rsp_count);  rni_console_write(rn_addr,log_text);

                sprintf(log_text,"diff  cnt %d",bpoll_req_count - poll_stat[i].bpoll_rsp_count); rni_console_write(rn_addr,log_text);
                  
                sum_delay = 0;

                for (j=0;j<=31;j++)
                {
                    if (poll_stat[i].bpoll_rsp_time[j] > 0)
                    {
                        max_delay = j;
                        sum_delay += j*poll_stat[i].bpoll_rsp_time[j];
                    }
                }
             
                for (j=31;j>0;j--)
                {
                    if (poll_stat[i].bpoll_rsp_time[j] > 0)
                    {
                         min_delay = j; 
                    }
                }

                if(poll_stat[i].bpoll_rsp_count > 0)
                {
                    sprintf(log_text,"Min  delay %d",min_delay);                 rni_console_write(rn_addr,log_text);
                    sprintf(log_text,"Max  delay %d",max_delay);                 rni_console_write(rn_addr,log_text);
                    sprintf(log_text,"Sum  delay %d",sum_delay);                 rni_console_write(rn_addr,log_text); 
                    sprintf(log_text,"Mean delay %d",sum_delay/poll_stat[i].bpoll_rsp_count); rni_console_write(rn_addr,log_text);     
                }
            }
        }
    }
}

/******************************************************************************
* Function Name    : console_write_registrations
* Function Purpose : Send registrations to console                  
* Input parameters : void
* Return value     : void
******************************************************************************/
static void console_write_registrations(void)
{
    char        line_text[128];
    char        var_text[64];
    uint8_t     rn_address;
  
    console_write("------------------\r\n");
    console_write("Registration Data:\r\n");
    console_write("Address    EQMID State       Timer Timeout\r\n");

    for (rn_address = RN_MIN_ADDRESS; rn_address <= RN_MAX_ADDRESS; rn_address++)
    {   
        sprintf(line_text,"");

        sprintf(var_text,"%7d ",rn_address);
        strcat(line_text,var_text);
        
        sprintf(var_text,"%8X ",rn_management_data[rn_address].rn_registration_data.eqmid);
        strcat(line_text,var_text);

        switch(rn_management_data[rn_address].rn_registration_data.rn_registration_state)
        {
        case RN_NONEX:                  sprintf(var_text,"NONEX    ");break;
        case RN_REGISTERED_CONNECTED:   sprintf(var_text,"CONNECTED");break;
        case RN_REGISTERED_ABSENT:      sprintf(var_text,"ABSENT   ");break;
        }

        strcat(line_text,var_text);

        sprintf(var_text,"%8d ",rn_management_data[rn_address].state_timer);
        strcat(line_text,var_text);

        if (RN_MASTER_ADDRESS != rn_address)
        {
            if(RN_REGISTERED_CONNECTED == rn_management_data[rn_address].rn_registration_data.rn_registration_state)
            {
                sprintf(var_text,"%7d ",RN_REGISTERED_CONNECTED_TMO);
                strcat(line_text,var_text);
            }

            if(RN_REGISTERED_ABSENT == rn_management_data[rn_address].rn_registration_data.rn_registration_state)
            {
                sprintf(var_text,"%7d ",RN_REGISTERED_ABSENT_TMO);
                strcat(line_text,var_text);
            }
        }

        if(RN_NONEX != rn_management_data[rn_address].rn_registration_data.rn_registration_state)
        {
            strcat(line_text,"\r\n");
            console_write(line_text);
        }
    }
}

/******************************************************************************
* Function Name    : remote_console_write_registrations
* Function Purpose : Send registration info to remote console                  
* Input parameters : void
* Return value     : void
******************************************************************************/
static void remote_console_write_registrations(uint8_t remote_a)
{
    char        line_text[128];
    char        var_text[64];
    uint8_t     rn_address;
  
  /*  console_write("------------------\r\n");
    console_write("Registration Data:\r\n");
    console_write("Address    EQMID State       Timer Timeout\r\n");
*/
    rni_console_write(remote_a,"Reg Info:");

    for (rn_address = RN_MIN_ADDRESS; rn_address <= RN_MAX_ADDRESS; rn_address++)
    {  
      

        sprintf(line_text,"");

        sprintf(var_text,"%d ",rn_address);
        strcat(line_text,var_text);
        
        switch(rn_management_data[rn_address].rn_registration_data.rn_registration_state)
        {
        case RN_NONEX:                  sprintf(var_text,"N ");break;
        case RN_REGISTERED_CONNECTED:   sprintf(var_text,"C ");break;
        case RN_REGISTERED_ABSENT:      sprintf(var_text,"A ");break;
        }

        strcat(line_text,var_text);

        /*sprintf(var_text,"%8d ",rn_management_data[rn_address].state_timer);
        strcat(line_text,var_text);*/

       /* if (RN_MASTER_ADDRESS != rn_address)
        {
            if(RN_REGISTERED_CONNECTED == rn_management_data[rn_address].rn_registration_data.rn_registration_state)
            {
                sprintf(var_text,"%7d ",RN_REGISTERED_CONNECTED_TMO);
                strcat(line_text,var_text);
            }

            if(RN_REGISTERED_ABSENT == rn_management_data[rn_address].rn_registration_data.rn_registration_state)
            {
                sprintf(var_text,"%7d ",RN_REGISTERED_ABSENT_TMO);
                strcat(line_text,var_text);
            }
        }*/
        
        sprintf(var_text,"E=%d",rn_management_data[rn_address].rn_registration_data.eqmid);
        strcat(line_text,var_text);

        rni_console_write(remote_a,line_text);

    }
}
