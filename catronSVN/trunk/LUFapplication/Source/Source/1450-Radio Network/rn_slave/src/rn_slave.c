/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_slave.c
* Author            : Per Erik Sundvisson
* Description       : Implementation of RN slave functions
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

/*
-----------------------------------------------------------
Macros
-------------------------------------------------------------
*/

#define RN_MAX_TIME_WITHOUT_RRC_REGISTRATION_CFM    (RN_BASE_TIME_5s)

#define RN_SLAVE_REGISTER_TIMEOUT_MIN               (RN_BASE_TIME_60ms)     // Min 60  ms
#define RN_SLAVE_REGISTER_TIMEOUT_RANGE             (RN_BASE_TIME_240ms)    // Max 60 + 240 = 300 ms

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
#include "rn_manager.h"
#include "rn_slave.h"
#include "rn_phy.h"
#include "rn_rlc_cc.h"
#include "rn_mac_cc.h"
#include "rn_log.h"

/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
/******************************************************************************
*     Strings for console or log outputs
*******************************************************************************/
#define MAX_CHARS 512
static char console_text[MAX_CHARS+1];
static char log_text[MAX_CHARS+1];          // FIXME: The text strings could probably be merged
static char var_text[32];
/******************************************************************************
*     Slave data
*******************************************************************************/
typedef enum
{
    RN_SLAVE_IDLE,
    RN_SLAVE_SEARCHING,
    RN_SLAVE_REGISTERING,
    RN_SLAVE_REGISTERED,
    RN_SLAVE_DEREGISTERING
} slave_state_t;

typedef struct
{
    slave_state_t       state;
    uint32_t            resend_timer;
    uint32_t            resend_timeout;
    uint32_t            resend_counter;
    uint8_t             registered_address;
    uint8_t             rnid;  
    uint32_t            eqmid;
    MessageQ_QueueId 	ap_rni_q_id;
} slave_data_t;

static slave_data_t slave_data;
/******************************************************************************
*     RN Management data
*******************************************************************************/
static rn_management_data_t rn_management_data[RN_MAX_ADDRESS+1]; // Mirrored data from the RN-master

/******************************************************************************
*     Poll and statistics data
*******************************************************************************/
static bool         apoll_on         = false;    // Addressed status poll on/off
static uint8_t      apoll_interval   = 50;       // Default 50 frames
static uint8_t      apoll_counter    = 0;        // Used to poll registered addresses

static bool         bpoll_on         = false;    // Broadcast status poll on/off
static uint8_t      bpoll_interval   = 50;       // Default 50 frames


#define RN_MAX_RESPONSE_TIME RN_BASE_TIME_620ms

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
static bool         ap_sync_on     = true;     // For test purpose
static uint32_t     rn_tick         = 0;        // Used for RN timing
//static uint32_t     frame_counter   = 0;        // Counts received frame sync

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/
static void clear_mirror(void);
static void console_write_registrations(void);
static void remote_console_write_registrations(uint8_t remote_a);

static const char * text_for_slave_state(slave_state_t state);
static void apoll_stat_clear(void);
static void bpoll_stat_clear(void);
static void rn_stat(void);
static void rn_stat_remote(uint8_t rn_addr);

static void receive_rni    (msg_buf_t *msg_buf);
static void receive_rpi    (msg_buf_t *msg_buf);

static void receive_rrc_reg_info(msg_buf_t *msg_buf);
static void receive_rrc_register_cfm(msg_buf_t *msg_buf);
static void receive_rrc_deregister_cfm(msg_buf_t *msg_buf);

static void receive_rrc    (msg_buf_t *msg_buf);

static void send_rrc_register_req(void);
static void send_rrc_deregister_req(void);
static void send_rrc_unit_report(void);
static void send_rni_connected(uint8_t addr, uint32_t eqm,uint8_t r);
static void send_rni_disconnected(void);

static void set_slave_state(slave_state_t state);
static void rn_slave_undefined(void);
static void handle_rn_slave_state(uint32_t frame);

static void status_apoll(uint32_t tick);
static void status_bpoll(uint32_t tick);
static void ap_sync(uint32_t frame);

static void receive_sync   (msg_buf_t *msg_buf);
static void receive_plc    (msg_buf_t *msg_buf);

static bool any_connection_list_change(void);
static void update_connected_list(void);

/*
-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------
*/

/*-----------------------------------------------------------
Function: rn_slave_module_init
-------------------------------------------------------------
*/
void rn_slave_module_init(void)
{
    clear_mirror();
 
    slave_data.state                = RN_SLAVE_IDLE;
    slave_data.resend_timer         = 0;
    slave_data.resend_counter       = 0;
    slave_data.eqmid                = 0;
    slave_data.registered_address   = RN_INVALID_ADDRESS;
    slave_data.ap_rni_q_id          = NULL;

    return;
}

/*-----------------------------------------------------------
Function: rn_slave_module_start
-------------------------------------------------------------*/
void rn_slave_module_start(uint8_t start_phase)
{
	(void)start_phase;

	return;
}
/*-----------------------------------------------------------
Function: rn_slave
-------------------------------------------------------------*/

void rn_slave(MessageQ_Handle rnm_input_queue, RTOS_QUEUE_ID_T rni_out_q, uint8_t rnid, uint32_t eqmid)
{
	msg_buf_t   *msg_buf = NULL;
   
    slave_data.eqmid     = eqmid;
    slave_data.rnid      = rnid;
    slave_data.ap_rni_q_id  = rni_out_q;

    set_slave_state(RN_SLAVE_IDLE);   // Until PHY reports otherwise
    
    rn_mac_cc_slave_conf(rnid);         // Will also configer PHY
  
    for( ;; )
    {
    	RTOS_QUEUE_RECEIVE(rnm_input_queue, &msg_buf, portMAX_DELAY );

        switch(msg_buf->pd)
        {
        case PD_RNI:
            rn_log_msg_receive(RN_RNM, msg_buf);
            if (RNI_UNDEFINED == msg_buf->mt)
            {
                set_slave_state(RN_SLAVE_IDLE);   
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
            rn_log_err_msg("Ignored Protocol Discriminator in RN_SLAVE",msg_buf);
            msg_buf_free(msg_buf);
            break;
        }
    }
}

/*
------------------------------------------------------------------------------
Static function definitions
------------------------------------------------------------------------------
*/
/******************************************************************************
* Function Name    : clear_mirror
* Function Purpose : Clear mirrored data
* Input parameters : void
* Return value     : void
******************************************************************************/
static void clear_mirror(void)
{
    uint8_t RN_address;

    for (RN_address = RN_MIN_ADDRESS; RN_address <= RN_MAX_ADDRESS; RN_address++)
    {
        rn_management_data[RN_address].rn_registration_data.rn_registration_state   = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.reported_state          = RN_NONEX;
        rn_management_data[RN_address].rn_registration_data.eqmid                   = RN_EQMID_INVALID;
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
  
    console_write("---------------------------\r\n");
    console_write("Mirrored registration data:\r\n");
    console_write("Address    EQMID State     Reported to APP\r\n");
     
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
        
        switch(rn_management_data[rn_address].rn_registration_data.reported_state)
        {
        case RN_NONEX:                  sprintf(var_text," DISCONNECTED");break;
        case RN_REGISTERED_CONNECTED:   sprintf(var_text," CONNECTED");break;
        case RN_REGISTERED_ABSENT:      sprintf(var_text," DISCONNECTED");break;
        }

        strcat(line_text,var_text);

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
 
    rni_console_write(remote_a,"Reg Mirror:");

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

        switch(rn_management_data[rn_address].rn_registration_data.reported_state)
        {
        case RN_NONEX:                  sprintf(var_text,"D ");break;
        case RN_REGISTERED_CONNECTED:   sprintf(var_text,"C ");break;
        case RN_REGISTERED_ABSENT:      sprintf(var_text,"D ");break;
        }

        strcat(line_text,var_text);
        
        sprintf(var_text,"E=%d",rn_management_data[rn_address].rn_registration_data.eqmid);
        strcat(line_text,var_text);

        rni_console_write(remote_a,line_text);
    }
}

/******************************************************************************
* Function Name    : text_for_slave_state
* Function Purpose : Return text for slave state
* Input parameters : slave_state_t
* Return value     : pointer to text
******************************************************************************/
static const char * text_for_slave_state(slave_state_t state)
{
    switch (state)
    {
    case RN_SLAVE_IDLE:         return "SLAVE IDLE";
    case RN_SLAVE_SEARCHING:    return "SLAVE SEARCHING";
    case RN_SLAVE_REGISTERING:  return "SLAVE REGISTERING";
    case RN_SLAVE_REGISTERED:   return "SLAVE REGISTERED";
    case RN_SLAVE_DEREGISTERING:return "SLAVE DEREGISTERING";
    default:                    return "UNDECODED";
    }
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
        if(i != slave_data.registered_address)
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
        if(i != slave_data.registered_address)
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
* Function Name    : receive_rni
* Function Purpose : Receives RNI messages for slave
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/
static void receive_rni(msg_buf_t *msg_buf)
{
 
    switch (msg_buf->mt)
    {
    case RNI_CONNECT:  // Already ordered
        break;

    case RNI_UNDEFINE:
        set_slave_state(RN_SLAVE_DEREGISTERING);
        slave_data.resend_counter = 0;
        slave_data.resend_timeout = 
            RN_SLAVE_REGISTER_TIMEOUT_MIN + 
            rand() % RN_SLAVE_REGISTER_TIMEOUT_RANGE;  // Sets delay range
        break;

    case RNI_LIST:
        console_write("\r\n");
        console_write(         "Main state:          RN_SLAVE\r\n");
        //                    -------              --------
        sprintf(console_text,"Sub state:           %s\r\n",text_for_slave_state(slave_data.state));
        console_write(console_text);

        if (RN_SLAVE_REGISTERED == slave_data.state)
        {
            sprintf(console_text,"Registered address:  %d\r\n",slave_data.registered_address);
            //                    -------              --------
            console_write(console_text);
        }

        sprintf(console_text,"Re-send timer:       %d (frames)\r\n",slave_data.resend_timer);
        //                   -------              --------
        console_write(console_text);

        if ((RN_SLAVE_REGISTERING == slave_data.state) || (RN_SLAVE_REGISTERED == slave_data.state))
        {
            sprintf(console_text,"Re-send timeout:     %d (frames)\r\n",slave_data.resend_timeout);
            //                   -------              --------
            console_write(console_text);
        }

        if(RN_SLAVE_DEREGISTERING == slave_data.state)
        {
            sprintf(console_text,"Re-send timeout:     %d (frames)\r\n",slave_data.resend_timeout);
            //                   -------              --------
            console_write(console_text);

            sprintf(console_text,"Re-send counter:     %d (times)\r\n",slave_data.resend_counter);
            console_write(console_text);
        }

        sprintf(console_text,"RNID:                %d\r\n",slave_data.rnid);
        console_write(console_text);

        sprintf(console_text,"EQMID:               %d\r\n",slave_data.eqmid);
        console_write(console_text);

        sprintf(console_text,"RN_SLAVE_REGISTERED timeout: %d (frames)\r\n",RN_MAX_TIME_WITHOUT_RRC_REGISTRATION_CFM);
        console_write(console_text);

        sprintf(console_text,"RN_SLAVE_REGISTERING timeout range: %d to %d (frames)\r\n",
            RN_SLAVE_REGISTER_TIMEOUT_MIN,
            RN_SLAVE_REGISTER_TIMEOUT_MIN + RN_SLAVE_REGISTER_TIMEOUT_RANGE);

        console_write(console_text);
        sprintf(console_text,"rn_tick       = %d\r\n",rn_tick);         console_write(console_text);
        //sprintf(console_text,"frame_counter = %d\r\n",frame_counter);   console_write(console_text);
        //sprintf(console_text,"Diff          = %d\r\n",rn_tick - frame_counter);   console_write(console_text);
        console_write_registrations();

        break;
    case RNI_APOLL_ON:

        apoll_on = true;
        apoll_stat_clear();

        if(0 < msg_return_data_size(msg_buf))
        {
            apoll_interval = msg_return_uint8(msg_buf,0);

            if(apoll_interval < 10)
            {
                apoll_interval = 10;        // Not accepted
            }
        }
        else
        {
            apoll_interval = 50;
        }

        sprintf(console_text,"Addressed poll switched ON with interval %d frames\r\n",apoll_interval);
        console_write(console_text);
        break;

    case RNI_STAT:
        rn_stat();
        break;

    case RNI_APOLL_OFF:
        apoll_on = false;
        console_write("Addressed poll switched OFF\r\n");
        break;

    case RNI_BPOLL_ON:

        bpoll_on = true;
        bpoll_stat_clear();

        if(0 < msg_return_data_size(msg_buf))
        {
            bpoll_interval = msg_return_uint8(msg_buf,0);

            if(bpoll_interval < 10)
            {
                bpoll_interval = 10;        // Not accepted
            }
        }
        else
        {
            bpoll_interval = 50;
        }

        sprintf(console_text,"Broadcast poll switched ON with interval %d frames\r\n",bpoll_interval);
        console_write(console_text);
        break;

    case RNI_BPOLL_OFF:
        bpoll_on = false;
        console_write("Broadcast poll switched OFF\r\n");
        break;

    case RNI_SYNC_ON:
        ap_sync_on = true;
        break;
          
    case RNI_SYNC_OFF:
        ap_sync_on = false;
        break;

    case RNI_HOHO:
        sprintf(log_text,"rn_tick = %d \r\n",rn_tick);console_write(log_text);
        broadcast_rrc_status_req(rn_tick);
        break;

    default:
        rn_log_err_msg("Ignored RNI message in RN_SLAVE",msg_buf);
        break;
    }
}
/******************************************************************************
* Function Name    : receive_rpi
* Function Purpose : Implements RPI interface for slave
* Input parameters : Pointer to message
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
        break;
    }
}

/******************************************************************************
* Function Name    : receive_rrc_reg_info
* Function Purpose : Receive RRC_REG_INFO message
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_reg_info(msg_buf_t *msg_buf)
{
     uint32_t index     = 1;    // Message type is already decoded
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

      update_connected_list();
}
/******************************************************************************
* Function Name    : receive_rrc_register_cfm
* Function Purpose : Receive RRC_REGISTER_CFM message
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_register_cfm(msg_buf_t *msg_buf)
{
    uint32_t index = 1;         // Message Type decoded
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

        if(rn_management_data[rn_addr].rn_registration_data.eqmid == slave_data.eqmid)
        {
            slave_data.registered_address    = rn_addr;
            set_slave_state(RN_SLAVE_REGISTERED);       // Renewal of registration confirmation
        
            slave_data.resend_timeout = RN_MAX_TIME_WITHOUT_RRC_REGISTRATION_CFM;
        }
    }
}
/******************************************************************************
* Function Name    : receive_rrc_deregister_cfm
* Function Purpose : Receive RRC_REGISTER_CFM message
* Input parameters : Pointer to message
* Return value     : void
******************************************************************************/
static void receive_rrc_deregister_cfm(msg_buf_t *msg_buf)
{
    uint32_t received_eqmid = RN_INVALID_EQMID;         // Message Type decoded

    received_eqmid = msg_return_uint32(msg_buf, 1);

    if(received_eqmid == slave_data.eqmid)
    {
        rn_slave_undefined();
    }
}

/******************************************************************************
* Function Name    : receive_rrc
* Function Purpose : Receives RRC messages
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
    case RRC_REGISTER_CFM:
        receive_rrc_register_cfm(msg_buf);
        break;

    case RRC_REG_INFO:
        receive_rrc_reg_info(msg_buf);
        break;

    case RRC_DEREGISTER_CFM:
        receive_rrc_deregister_cfm(msg_buf);
        break;

    case RRC_APOLL_REQ:
        receive_rrc_apoll_req(msg_buf,slave_data.eqmid);
        break;

    case RRC_BPOLL_REQ:
        receive_rrc_bpoll_req(msg_buf,slave_data.eqmid);
        break;

    case RRC_STATUS_REQ:
        receive_rrc_status_req(msg_buf,slave_data.eqmid);
        break;

    case RRC_APOLL_RSP:

        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_tick - time_stamp;

        poll_stat[msg_return_rn_sender(msg_buf)].apoll_rsp_count++;

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
        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_tick - time_stamp;
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
        sprintf(log_text,"RN address %d responded with delay %d RN ticks",msg_return_rn_sender(msg_buf),rn_tick -  msg_return_uint32(msg_buf,1));
        console_write(log_text);
        break;

    case RRC_REGISTER_REJ:
        break;

    case RRC_STAT:
        rn_addr = (msg_return_rn_sender(msg_buf));
        rn_stat_remote(rn_addr);
        break;

    case RRC_LIST:
        rn_addr = (msg_return_rn_sender(msg_buf));

        rni_console_write(rn_addr,"RN_SLAVE");
        //                    -------              --------
        sprintf(console_text,"%s ",text_for_slave_state(slave_data.state));
        rni_console_write(rn_addr,console_text);

        if (RN_SLAVE_REGISTERED == slave_data.state)
        {
            sprintf(console_text,"Reg addr %d",slave_data.registered_address);
            rni_console_write(rn_addr,console_text);
        }

        sprintf(console_text,"RNID: %d",slave_data.rnid);
        rni_console_write(rn_addr,console_text);

        sprintf(console_text,"EQMID: %d",slave_data.eqmid);
        rni_console_write(rn_addr,console_text);

        remote_console_write_registrations(rn_addr);

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
* Function Name    : send_rrc_register_req
* Function Purpose : Sends RRC_REGISTER_REQ message
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rrc_register_req(void)
{
     msg_buf_t  *reg_req;
     uint32_t   index = 0; 

     reg_req = msg_buf_alloc(5);

     msg_put_uint8(reg_req,index,RRC_REGISTER_REQ);
     index++;
     msg_put_uint32(reg_req,index,slave_data.eqmid);
     index +=4;
     reg_req->size = index;

     rn_rcc_send(reg_req);

}
/******************************************************************************
* Function Name    : send_rrc_deregister_req
* Function Purpose : Sends RRC_DEREGISTER_REQ message
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rrc_deregister_req(void)
{
     msg_buf_t  *reg_req;
     uint32_t   index = 0; 

     reg_req = msg_buf_alloc(5);

     msg_put_uint8(reg_req,index,RRC_DEREGISTER_REQ);
     index++;
     msg_put_uint32(reg_req,index,slave_data.eqmid);
     index +=4;
     reg_req->size = index;

     rn_rcc_send(reg_req);

}
/******************************************************************************
* Function Name    : send_rrc_unit_report
* Function Purpose : Sends RRC_UNIT_REPORT message
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rrc_unit_report(void)
{
    msg_buf_t *unit_report;
    uint32_t    index = 0;

    unit_report = msg_buf_alloc(7);

    msg_put_uint8_i     (unit_report,&index,RRC_UNIT_REPORT);
    msg_put_uint32_i    (unit_report,&index,slave_data.eqmid);
    msg_put_uint8_i     (unit_report,&index,0XAA);
    msg_put_uint8_i     (unit_report,&index,0XCC);

    msg_set_data_size(unit_report,index);

    rn_rccr_send(RN_MASTER_ADDRESS, unit_report);
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
     uint32_t index = 0;

     rni_connected = msg_buf_alloc(RNI_CONNECTED_SIZE);
     
     rni_connected->mt = RNI_CONNECTED;
     rni_connected->pd = PD_RNI;

     msg_put_uint32_i(rni_connected,&index,eqm);
     msg_put_uint8_i(rni_connected,&index,addr);
     msg_put_uint8_i(rni_connected,&index,rnid);

     //RTOS_QUEUE_SEND( slave_data.ap_rni_q, &rni_connected, 0U );
     rtos_send(slave_data.ap_rni_q_id,rni_connected);
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

     rtos_send(slave_data.ap_rni_q_id,rni_connected);

}

/******************************************************************************
* Function Name    : set_slave_state
* Function Purpose : Sets state as requested. 
*                    Performs some actions depending on old and new state
* Input parameters : slave_state_t, enum defining available states
* Return value     : void
******************************************************************************/
static void set_slave_state(slave_state_t state)
{
    char        line_text[128];
    //char        var_text[64];

    if(slave_data.state != state)
    {
        switch(slave_data.state)             // Actions when leaving old state
        {
            case RN_SLAVE_IDLE:
                break;

            case RN_SLAVE_REGISTERED:       // Leaving registered state
                send_rni_disconnected();    // Inform application
                break;

            default:
                break;
        }
        switch(state)                       // Actions when entering new state
        {
        case RN_SLAVE_IDLE:
            clear_mirror();
            rn_mac_cc_idle_conf();          // Will also configer PHY
            rn_rlc_address_conf (RN_INVALID_ADDRESS);
            break;

        case RN_SLAVE_SEARCHING:
            clear_mirror();
            break;

        case RN_SLAVE_REGISTERING:
            clear_mirror();
           /* if(RN_SLAVE_REGISTERED == slave_data.state)
            {
                send_rni_disconnected();
            }*/
            //rn_log_text("RN_SLAVE_REGISTERING");
            break;

        case RN_SLAVE_REGISTERED:
            rn_rlc_address_conf (slave_data.registered_address);
            send_rni_connected(slave_data.registered_address,slave_data.eqmid,slave_data.rnid);
            //rn_log_text("RN_SLAVE_REGISTERED");

            break;

        case RN_SLAVE_DEREGISTERING:
            send_rrc_deregister_req();
            slave_data.resend_counter  = 0;
            slave_data.resend_timer  = 0;
            break;
        
        default: rn_log_err("rn_slave_state undecoded"); break;
        }

        sprintf(line_text,"%s -> %s",text_for_slave_state(slave_data.state),text_for_slave_state(state));
        rn_log_vtext(line_text);

      /* rn_log_text(text_for_slave_state(slave_data.state));
       rn_log_text(text_for_slave_state(state));*/
    }

    slave_data.state            = state;
    slave_data.resend_timer     = 0;

    if(RN_SLAVE_REGISTERED == slave_data.state)
    {
      /*  rn_rlc_address_conf (slave_data.registered_address);*/
    }
    else if(RN_SLAVE_IDLE == slave_data.state)
    {
        //rn_mac_cc_idle_conf();          // Will also configer PHY
        //rn_rlc_address_conf (RN_INVALID_ADDRESS);
    }

    rn_rlc_router_conf(false);       // No message routing in any state in slave
    rn_rlc_sniffer_conf(false);      // No message sniffing in any state in slave

}
/******************************************************************************
* Function Name    : rn_slave_undefined
* Function Purpose : Sends internal message RNI_UNDEFINED when the slave has 
*                    finished undefining itself                   
* Input parameters : void
* Return value     : void
******************************************************************************/
static void rn_slave_undefined(void)
{
    msg_buf_t *msg_buf = NULL;

    msg_buf = msg_buf_alloc(0);
    msg_buf->pd     = PD_RNI;
    msg_buf->mt     = RNI_UNDEFINED;

    rn_log_msg_send(RN_RNM, msg_buf);
    rtos_send(rnm_queue_id(),msg_buf);

}
/******************************************************************************
* Function Name    : handle_rn_slave_state
* Function Purpose : Handles slave state timer controlled actions                  
* Input parameters : frame, used for time interval dependent actions
* Return value     : void
******************************************************************************/
static void handle_rn_slave_state(uint32_t frame)
{
    slave_data.resend_timer++;

    switch (slave_data.state)
    {
    case RN_SLAVE_SEARCHING:
        break;
    case RN_SLAVE_REGISTERING:      
        if (slave_data.resend_timer > slave_data.resend_timeout)
        {
            send_rrc_register_req();
            slave_data.resend_counter++;
            slave_data.resend_timer = 0;
            slave_data.resend_timeout = 
                RN_SLAVE_REGISTER_TIMEOUT_MIN + 
                rand() % RN_SLAVE_REGISTER_TIMEOUT_RANGE;  // Sets delay range

            //printf("resend_timeout = %d \r\n",slave_data.resend_timeout);
        }
        break;

    case RN_SLAVE_DEREGISTERING:

        if(slave_data.resend_counter < 5)
        {
            if (slave_data.resend_timer > slave_data.resend_timeout)
            {
                send_rrc_deregister_req();
                slave_data.resend_counter++;
                slave_data.resend_timer = 0;
                slave_data.resend_timeout = 
                    RN_SLAVE_REGISTER_TIMEOUT_MIN + 
                    rand() % RN_SLAVE_REGISTER_TIMEOUT_RANGE;  // Sets delay range

            }
        }
        else
        {
            // Enough attempts, undefine anyway
            rn_slave_undefined();
        }
        break;

    case RN_SLAVE_REGISTERED:
        if (slave_data.resend_timer > slave_data.resend_timeout)
        {
            send_rrc_register_req();            
            set_slave_state(RN_SLAVE_REGISTERING);
            slave_data.resend_timeout = 
                RN_SLAVE_REGISTER_TIMEOUT_MIN + 
                rand() % RN_SLAVE_REGISTER_TIMEOUT_RANGE;  // Sets delay range
        }
        else
        {
            if (0 == (frame % 25))          // Defines time between rrc_unit_report
            {
                send_rrc_unit_report();
            }
        }
       
        break;
    case RN_SLAVE_IDLE:
        break;
    default:
        break;
    }
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
    
    lpoll_addr = apoll_counter;

    //lpoll_addr = RN_MASTER_ADDRESS;

    if (lpoll_addr != slave_data.registered_address)
    {
        if (rn_management_data[lpoll_addr].rn_registration_data.rn_registration_state != RN_NONEX)
        {
            status_req = msg_buf_alloc(20); // Max number of application bytes to send
            data_index = 0;

            msg_put_uint8_i     (status_req,&data_index,RRC_APOLL_REQ);
            msg_put_uint32_i    (status_req,&data_index,rn_tick);
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
    msg_put_uint32_i    (status_req,&data_index,rn_tick);
    msg_set_data_size   (status_req,data_index);

    bpoll_req_count++;
    rn_rccr_broadcast(status_req);

}

/******************************************************************************
* Function Name    : ap_sync
* Function Purpose : Send ap sync message (for test purpose)                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void   ap_sync(uint32_t frame) 
{
    msg_buf_t *rni_sync = NULL;

    rni_sync = msg_buf_alloc(4);
    rni_sync->pd = PD_SYNC;
    rni_sync->mt = RNI_SYNC;
    
    msg_put_uint32(rni_sync,0,frame);

    rtos_send(slave_data.ap_rni_q_id,rni_sync);
}
/******************************************************************************
* Function Name    : receive_sync
* Function Purpose : Reception of messages on SYNC interface                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_sync(msg_buf_t *msg_buf)
{
    uint32_t    frame_number;

    switch (msg_buf->mt)
    {
    case SYNC_FRAME:
   /*     frame_counter++;*/
        frame_number = msg_return_uint16(msg_buf,0);
     /*   handle_rn_slave_state(frame_number);
        status_poll(frame_number);*/
        break;

    case SYNC_RN_TIMER:
        rn_tick = msg_return_uint32(msg_buf,0);
        
        handle_rn_slave_state(rn_tick);
        
        status_apoll(rn_tick);
        status_bpoll(rn_tick);

        if(ap_sync_on)
        {
            ap_sync(rn_tick); 
        }
        break;

    default:
        //rn_log_err_msg("Ignored SYNC message in RN_SLAVE",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : receive_plc
* Function Purpose : Reception of messages on Physical Layer Interface                  
* Input parameters : pointer to message
* Return value     : void
******************************************************************************/
static void receive_plc(msg_buf_t *msg_buf)
{
    switch (msg_buf->mt)
    {
    case PLC_PHY_SLAVE_SEARCHING:
        set_slave_state(RN_SLAVE_SEARCHING);
        break;

    case PLC_PHY_SLAVE_NORMAL:
        set_slave_state(RN_SLAVE_REGISTERING);
        send_rrc_register_req();

        slave_data.resend_timeout = 
            RN_SLAVE_REGISTER_TIMEOUT_MIN + 
            rand() % RN_SLAVE_REGISTER_TIMEOUT_RANGE;  // Sets delay range

        break;

    default:
        rn_log_err_msg("RN_SLAVE: Ignored PLC message: ",msg_buf);
        break;
    }
}

/******************************************************************************
* Function Name    : any_connection_list_change
* Function Purpose : Check if there has been any connection list change
*                    since last report
* Input parameters : pointer to message
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

    if(!any_connection_list_change())
    {
         
        return;
    }

    if(NULL == slave_data.ap_rni_q_id)
    {
        rn_log_err(">>>>> No RNI receiver defined");
        return;
    }
    msg_buf = msg_buf_alloc(12);

    msg_buf->pd             = PD_RNI;
    msg_buf->mt             = RNI_CONNECTED_LIST;
   
    n_addresses = 0;        
    
    index       = 1;       // Here is where the list starts

   
    //msg_buf->data[index] = RN_MASTER_ADDRESS;
    
    //index++;
    
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
    //RTOS_QUEUE_SEND( slave_data.ap_rni_q_id, &msg_buf, 0U );  // FIXME, this should happen later
    rtos_send(slave_data.ap_rni_q_id,msg_buf);
}

