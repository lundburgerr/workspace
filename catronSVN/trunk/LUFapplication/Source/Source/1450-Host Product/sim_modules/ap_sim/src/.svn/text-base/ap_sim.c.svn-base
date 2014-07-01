/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : ap_sim.c
* Author            : Per Erik Sundvisson
* Description       : Application simulator, 
*                     Intended for test of RN API etc.
*                     Not intended for release
********************************************************************************
* Revision History
* 2013-04-03        : First Issue
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

/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/

//#include "rtos.h"

/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rni.h"
#include "ap_sim.h"
#include "rn_log.h"
#include "true_voice_SvnRev96.h"
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/



typedef struct
{
    bool                connected;
    uint8_t             registered_address;
    uint8_t             rnid;  
    uint32_t            eqmid;

} ap_sim_data_t;

static ap_sim_data_t ap_sim_data;

#define MAX_CHARS 512    
static char help_text[MAX_CHARS + 1];   // Defined here to save stack. Only to be used by help function
static char log_text[MAX_CHARS+1];      // FIXME: Probably this can be merged with help_text
static char var_text[32];               // Text to be appended to other text, for example with strcat

typedef enum
{
    TCI_UNDEF,
    TCI_S_ACT,
    TCI_R_ACT,
    TCI_D_ACT,
    TCI_MON,
    TCI_MOFF
} ap_act_t;

static ap_act_t ap_act = TCI_UNDEF;

typedef enum
{
    AP_TEST_UNDEF,
    AP_TEST_1,
    AP_TEST_2,
    AP_SYNC_ON,
    AP_SYNC_OFF,
    AP_APOLL_ON,                 // APP Addressed message Poll ON
    AP_APOLL_OFF,                // APP Addressed message Poll OFF
    AP_BPOLL_ON,                 // APP Broadcast message Poll ON
    AP_BPOLL_OFF,                // APP Broadcast message Poll OFF
    AP_LIST,
    AP_STAT,
    AP_HOHO,
    AP_TEST_HELP
} ap_test_t;

ap_test_t    ap_test  = AP_TEST_UNDEF;

static uint8_t              channel         = 0x00;

static bool tci_mon[TCI_MAX_CHANNEL_NUMBER+1];

//static bool cci_mon = true;
//static bool rni_mon = true;

static bool ap_apoll_on = false;
static bool ap_bpoll_on = false;

static uint8_t      apoll_interval   = 50;     
static uint8_t      bpoll_interval   = 100;    
static uint8_t      poll_counter     = 0;      // Used to poll connected addresses

#define RN_MAX_RESPONSE_TIME 31

typedef struct 
{
    uint32_t    apoll_req_count;
    uint32_t    apoll_rsp_count;
    uint8_t     apoll_rsp_time[RN_MAX_RESPONSE_TIME+1];       // Bucket statistics for response time
    
    uint32_t    bpoll_rsp_count;
    uint8_t     bpoll_rsp_time[RN_MAX_RESPONSE_TIME+1];       // Bucket statistics for response time

} poll_stat_t;

static poll_stat_t poll_stat[RN_MAX_ADDRESS+1];
static uint32_t bpoll_req_count;

static uint32_t     rn_tick = 0;
static bool         ap_connected[RN_MAX_ADDRESS+1];

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/
static  void pd_rni_test(msg_buf_t *msg_buf);
static  void pd_tci_test(msg_buf_t *msg_buf);
static  void pd_cci_test(msg_buf_t *msg_buf);
static  void pd_rni_sync(msg_buf_t *msg_buf);

static char * app_string_for_mt(uint8_t mt);

/*-----------------------------------------------------------
Command declarations, including related static function 
declarations
-------------------------------------------------------------*/

// ----------------------------------------------------------
// tci
// Commands for excercising the Traffic Channel Interface (TCI)
// ----------------------------------------------------------
static portBASE_TYPE tci_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t tci_cmd =
{
	( const int8_t * const ) "tci",
	( const int8_t * const ) "tci         <parameters>        TCI test commands. \"tci\" lists parameters (only with AP simulator) \r\n",
	 //                       ---------   -----------------   ---------
    tci_command, /* The function to run. */
	-1 /* Number of parameters expected */
};
// ----------------------------------------------------------
// apt - ap test commands
// ----------------------------------------------------------
static portBASE_TYPE ap_test_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t ap_test_cmd =
{
	( const int8_t * const ) "ap",
	( const int8_t * const ) "ap                              AP test commands,  \"ap\" lists parameters (only with AP simulator) \r\n\r\n",
    //                        ---------   -----------------   ---------
	ap_test_command, /* The function to run. */
	-1 /* The user can enter any number of parameters  */
};
/*-----------------------------------------------------------
Global function definitions
-------------------------------------------------------------*/
/*-----------------------------------------------------------
-------------------------------------------------------------*/
void ap_sim_module_init(void)
{
    uint32_t    i;

    for (i = 0; i <= TCI_MAX_CHANNEL_NUMBER; i++)
    {
        tci_mon[i] = false;
    }

    for (i = RN_MIN_ADDRESS; i <= RN_MAX_ADDRESS; i++)
    {
        ap_connected[i] = false;
    }

	return;
}

/*-----------------------------------------------------------
-------------------------------------------------------------*/
void ap_sim_module_start(uint8_t start_phase)
{
    tci_channel_t tci_channel;

    tvRxChannels_t tvRxChannels;
    tvMicChannels_t tvMicChannels;
    tvTxChannels_t tvTxChannels;
    tvSpeakerChannels_t tvSpeakerChannels;
    
    tvRxChannels.psClt = NULL;
    tvMicChannels.psMicNoise = NULL;
    tvTxChannels.psCltToLine = NULL;
    tvSpeakerChannels.psPilotMono = NULL;
    
    //memset(&tvRxChannels,0,sizeof(tvRxChannels_t));
    
    tci_voice(&tci_channel);
    
    tvRxChannels.psClt          = tci_channel.channel[0];
    tvRxChannels.psClt2wire     = tci_channel.channel[1];
    tvRxChannels.psClt4wire     = tci_channel.channel[2];
    
    // etc.

    true_voice( tvRxChannels,  tvMicChannels,  tvTxChannels,  tvSpeakerChannels);

    switch(start_phase)
    {
    case 1:
        FreeRTOS_CLIRegisterCommand( &tci_cmd );
        FreeRTOS_CLIRegisterCommand( &ap_test_cmd );
        cci_mt_translator_reg(app_string_for_mt);
        break;

    case 2:
        rni_register_message_receiver	(rn_ap_sim_queue_id());
        cci_register_message_receiver	(rn_ap_sim_queue_id());
        tci_register_sender	            (rn_ap_sim_queue_id());
        tci_register_receive_queue      (rn_ap_sim_queue_id());
        break;
    }

	return;
}

/*
-----------------------------------------------------------
Static function definitions
-------------------------------------------------------------
*/

static char * app_string_for_mt(uint8_t mt)
{
    switch(mt)
    {   
    case APP_APOLL_REQ:      return "APP_APOLL_REQ";
    case APP_APOLL_RSP:      return "APP_APOLL_RSP";
    case APP_BPOLL_REQ:      return "APP_BPOLL_REQ";
    case APP_BPOLL_RSP:      return "APP_BPOLL_RSP";
    case APP_STATUS_REQ:     return "APP_STATUS_REQ";
    case APP_STATUS_RSP:     return "APP_STATUS_RSP";

    }
    return "APP MT?";
}

static  void pd_tci_test(msg_buf_t *msg_buf)
{
    msg_buf_t *tci_data = NULL;
    uint8_t channel;
    uint32_t i;

    channel = msg_return_uint8(msg_buf,TCI_CHANNEL_POS);

    if(channel > TCI_MAX_CHANNEL_NUMBER)
    {
        rn_log_err_msg("pd_tci_test: Channel out of range",msg_buf);
        return;
    }
    
    if(tci_mon[channel])
    {
        rn_log_text_msg("->TCI",msg_buf);
    }

    switch(msg_buf->mt)
    {
    case TCI_READY_TO_SEND:

        tci_data = msg_buf_alloc(TCI_DATA_SIZE);

        tci_data->pd = PD_TCI;
        tci_data->mt = TCI_DATA;

        for (i = 0;i < TCI_DATA_SIZE;i++)
        {
            tci_data->data[i] = (uint8_t)i;  // Dummy set data  
        }

        msg_put_uint8 (tci_data,TCI_CHANNEL_POS, channel);
        msg_put_uint16(tci_data,TCI_FN_POS, msg_return_uint16(msg_buf,TCI_FN_POS));

        if(tci_mon[channel])
        {
            rn_log_text_msg("TCI->",tci_data);
        }
        
        //RTOS_QUEUE_SEND(tci_send_queue_get(),&tci_data,0);
        rtos_send(tci_send_queue_get(),tci_data);
        break;

    case TCI_DATA:
        break;

    case TCI_EMPTY:
        break;

    default:
        rn_log_err_msg("pd_tci_test: MT not handled",msg_buf);
        break;
    }
}

static void receive_app(msg_buf_t *msg_buf)
{
    msg_buf_t   *rsp_msg;
    uint32_t    data_index;
    uint32_t    time_stamp;
    uint32_t    response_time;

    switch(msg_buf->data[0])
    {
    case APP_APOLL_REQ:
        time_stamp = msg_return_uint32(msg_buf,1);
        
        rsp_msg = msg_buf_alloc(20); // Max number of application bytes to send
        data_index = 0;

        msg_put_uint8_i     (rsp_msg,&data_index,APP_APOLL_RSP);
        msg_put_uint32_i    (rsp_msg,&data_index,time_stamp);
        msg_set_data_size   (rsp_msg,data_index);

        cci_send(msg_return_rn_sender(msg_buf), rsp_msg);

        break;
         
    case APP_BPOLL_REQ:
        time_stamp = msg_return_uint32(msg_buf,1);
        rsp_msg = msg_buf_alloc(20); // Max number of application bytes to send
        data_index = 0;

        msg_put_uint8_i     (rsp_msg,&data_index,APP_BPOLL_RSP);
        msg_put_uint32_i    (rsp_msg,&data_index,time_stamp);
        msg_set_data_size   (rsp_msg,data_index);

        cci_send(msg_return_rn_sender(msg_buf), rsp_msg);

        break;
       
    case APP_STATUS_REQ:

        time_stamp = msg_return_uint32(msg_buf,1);
        rsp_msg = msg_buf_alloc(20); // Max number of application bytes to send
        data_index = 0;

        msg_put_uint8_i     (rsp_msg,&data_index,APP_STATUS_RSP);
        msg_put_uint32_i    (rsp_msg,&data_index,time_stamp);
        msg_set_data_size   (rsp_msg,data_index);

        cci_send(msg_return_rn_sender(msg_buf), rsp_msg);

        break;

    case APP_STATUS_RSP:
        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_tick - time_stamp;

        sprintf(log_text,"RN address %d responded with APP_STATUS_RSP delay %d RN ticks\r\n",msg_return_rn_sender(msg_buf),response_time);
        console_write(log_text);

        break;

    case APP_APOLL_RSP:
        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_tick - time_stamp;
        
        sprintf(log_text,"RN address %d responded with APP_APOLL_RSP delay %d RN ticks\r\n",msg_return_rn_sender(msg_buf),response_time);
        console_write(log_text);
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

    case APP_BPOLL_RSP:
        time_stamp = msg_return_uint32(msg_buf,1);
        response_time = rn_tick - time_stamp;
        
        sprintf(log_text,"RN address %d responded with APP_BPOLL_RSP delay %d RN ticks\r\n",msg_return_rn_sender(msg_buf),response_time);
        console_write(log_text);

        poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_count++;
          
        //response_time /= 5;

        if(response_time < RN_MAX_RESPONSE_TIME)
        {
            poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_time[response_time]++;
        }
        else
        {
            poll_stat[msg_return_rn_sender(msg_buf)].bpoll_rsp_time[RN_MAX_RESPONSE_TIME]++;
        }

        break;

    default:
        break;
    }
}

static  void pd_cci_test(msg_buf_t *msg_buf)
{
    msg_buf_t *tci_data = NULL;
   
    switch(msg_buf->mt)
    {
    case ACC_UDATA:
        receive_app(msg_buf);
        break;

    case BCC_UDATA:
        receive_app(msg_buf);
        break;

    default:
        rn_log_err_msg("PD_CCI_TEST: MT not handled",msg_buf);
        break;
    }
}

static  void pd_rni_test(msg_buf_t *msg_buf)
{
    //char        text[64];
    char        atext[8];
    uint32_t    i;
     
   
    switch(msg_buf->mt)
    {
    case RNI_CONNECTED:
        
        /*sprintf(text,">>>>> CONNECTED, EQMID = %d, ADDR = %d",
            msg_return_uint32(msg_buf,RNI_CONNECTED_EQMID_POS),
            msg_return_uint8(msg_buf,RNI_CONNECTED_ADDR_POS));
            rn_log_vtext(text);*/

        i = 0;

        ap_sim_data.connected           = true;
        ap_sim_data.eqmid               = msg_return_uint32_i(msg_buf,&i);
        ap_sim_data.registered_address  = msg_return_uint8_i (msg_buf,&i);
        ap_sim_data.rnid                = msg_return_uint8_i (msg_buf,&i);

        sprintf(log_text,"ap_sim received RNI_CONNECTED with RN-ADDRESS =  %d (DEC), EQMID = %d, RNID = %d\r\n",
            ap_sim_data.registered_address,
            ap_sim_data.eqmid,
            ap_sim_data.rnid);

        console_write(log_text);
        break;

    case RNI_DISCONNECTED:
        
        ap_sim_data.connected = false;
        sprintf(log_text,"ap_sim received RNI_DISCONNECTED \r\n");
        console_write(log_text);
        break;

    case RNI_CONNECTED_LIST:
        sprintf(log_text,"ap_sim received RNI_CONNECTED_LIST, connected addresses:");

        for(i = RN_MIN_ADDRESS;i<= RN_MAX_ADDRESS;i++)
        {
            ap_connected[i] = false;
        }

        i = 1;

        while(i <= msg_return_uint8(msg_buf,0))
        {
            if(RN_MAX_ADDRESS >= msg_return_uint8(msg_buf,i))
            {
                ap_connected[msg_return_uint8(msg_buf,i)] = true;
            }

            sprintf(atext,"%d ",msg_return_uint8(msg_buf,i));
            strcat(log_text,atext);
            i++;
        }

        strcat(log_text,"\r\n");
        console_write(log_text);
        break;

    case RNI_STARTED:
       /* sprintf(text,">>>>> STARTED");
        rn_log_vtext(text);*/
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

    if(!ap_apoll_on)
    {
        return;
    }

    if (!ap_sim_data.connected)
    {
        return;
    }

    if(0 != (frame % apoll_interval))       // Check if poll time
    {
        return;
    }

    poll_counter++;
    poll_counter %= RN_MAX_NO_OF_SLAVES;

    lpoll_addr = poll_counter;

    //lpoll_addr = RN_MASTER_ADDRESS;

    
    /*sprintf(log_text,"status_apoll, frame = %d, lpoll_addr = %d ",frame,lpoll_addr);
    rn_log_vtext(log_text);*/

    if (lpoll_addr != ap_sim_data.registered_address)
    {
        if (ap_connected[lpoll_addr])
        {
            status_req = msg_buf_alloc(20); // Max number of application bytes to send
            data_index = 0;

            msg_put_uint8_i     (status_req,&data_index,APP_APOLL_REQ);
            msg_put_uint32_i    (status_req,&data_index,frame);
            msg_set_data_size   (status_req,data_index);

            cci_send(lpoll_addr, status_req);
            poll_stat[lpoll_addr].apoll_req_count++;
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
 

    if(!ap_bpoll_on)
    {
        return;
    }

    if (!ap_sim_data.connected)
    {
        return;
    }

    if(0 != (frame % bpoll_interval))       // Check if poll time
    {
        return;
    }

   /* sprintf(log_text,"status_bpoll, frame = %d",frame);
    rn_log_vtext(log_text);*/

    status_req = msg_buf_alloc(20); // Max number of application bytes to send
    data_index = 0;

    msg_put_uint8_i     (status_req,&data_index,APP_BPOLL_REQ);
    msg_put_uint32_i    (status_req,&data_index,frame);
    msg_set_data_size   (status_req,data_index);
    
    bpoll_req_count++;

    cci_broadcast(status_req);
}

/******************************************************************************
* Function Name    : ap_hoho
* Function Purpose : Test function for checking response from other units in the system                  
* Input parameters : frame
* Return value     : void
******************************************************************************/
static void ap_hoho(uint32_t time)
{
    msg_buf_t   *status_req;
    uint32_t    data_index;

    status_req = msg_buf_alloc(20); // Max number of application bytes to send
    data_index = 0;

    msg_put_uint8_i     (status_req,&data_index,APP_STATUS_REQ);
    msg_put_uint32_i    (status_req,&data_index,time);
    msg_set_data_size   (status_req,data_index);

    cci_broadcast(status_req);
}

/* ----------------------------------------------------------- */

static  void pd_rni_sync(msg_buf_t *msg_buf)
{
    
    switch(msg_buf->mt)
    {

    case RNI_SYNC:
        rn_tick = msg_return_uint32(msg_buf,0);
        status_apoll(rn_tick);
        status_bpoll(rn_tick);
        break;

    default:
        break;
    }
}
void ap_sim_msg(msg_buf_t *msg_buf)
{
    
    switch(msg_buf->pd)
    {
    case PD_RNI:
        rn_log_msg_receive(RN_APP,msg_buf);
        pd_rni_test(msg_buf);
        //printf("AP received RNI message %02d)\n\r",msg_buf->mt); 
        break; 

    case PD_CCI:
        pd_cci_test(msg_buf);
        //printf("AP received CCI message %02d\n\r",msg_buf->mt); 
        break; 

    case PD_TCI:
        pd_tci_test(msg_buf);
        break; 

    case PD_SYNC:
        pd_rni_sync(msg_buf);
        break; 

    default:printf("AP received unknown PD %d\n\r",msg_buf->pd); break;
    }
}

/*
-------------------------------------------------------------
TEST COMMAND FUNCTION DEFINITIONS
-------------------------------------------------------------
*/

/* ----------------------------------------------------------- */
//-act/r-act/d-act/mon/moff> <channel> 

void    tci_help(void)
{
    sprintf(help_text,"Command Format: tci <parameters>\r\n\r\n"); console_write(help_text);

    sprintf(help_text,"Parameters       Meaning\r\n"); console_write( help_text);
    sprintf(help_text,"--------------   -------\r\n"); console_write( help_text);
    
    sprintf(help_text,"s-act channel    Activate specified channel for sending\r\n"); console_write( help_text);
    sprintf(help_text,"r-act channel    Activate specified channel for receiving\r\n"); console_write( help_text);
    sprintf(help_text,"d-act channel    De-activate specified channel \r\n"); console_write( help_text);
    
    sprintf(help_text,"mon   channel    Monitor ON for specified channel\r\n"); console_write( help_text);
    sprintf(help_text,"moff  channel    Monitor OFF for specified channel\r\n"); console_write( help_text);
}
static portBASE_TYPE tci_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
#define MAX_PAR_LEN         10
    int8_t                  *pcParameter;
    portBASE_TYPE           lParameterStringLength, xReturn;
    static portBASE_TYPE    lParameterNumber = 0;
    rn_result_t             result;
    char                    par[MAX_PAR_LEN+1];

    configASSERT( pcWriteBuffer );

    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    if( lParameterNumber == 0 )
    {
        ap_act = TCI_UNDEF;
        /* Next time the function is called the first parameter will be echoed back. */
        lParameterNumber = 1L;

        /* There may be more data to be returned as no parameters have been echoed back yet. */
        xReturn = pdPASS;
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

        //        configASSERT( pcParameter ); /* Sanity check something was returned. */

        if( pcParameter != NULL )
        {
            if(1 == lParameterNumber)
            {

                if(MAX_PAR_LEN > lParameterStringLength)
                {
                    strncpy(par,(const char*)pcParameter,MAX_PAR_LEN);
                    par[lParameterStringLength] = 0;
                }
                else
                {
                    par[0] = 0;
                }

                if (0 == strcmp((const char *)par,"s-act"))
                {
                    ap_act = TCI_S_ACT;
                }
                else if (0 == strcmp((const char *)par,"r-act"))
                {
                    ap_act = TCI_R_ACT;
                }
                else if (0 == strcmp((const char *)par,"d-act"))
                {
                    ap_act = TCI_D_ACT;
                }
                else if (0 == strcmp((const char *)par,"mon"))
                {
                    ap_act = TCI_MON;
                }
                else if (0 == strcmp((const char *)par,"moff"))
                {
                    ap_act = TCI_MOFF;
                }
                else
                {
                    // PARAMETER FAULT
                    ap_act = TCI_UNDEF;
                }

                //rnid = (uint8_t) atoi((const char*) pcParameter);
            }

            if(2 == lParameterNumber)
            {
                channel = (uint8_t) atoi((const char*) pcParameter);
            }

            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            /* If this is the last of the parameters then execute */

            result = RN_OK;
            if(lParameterNumber > 2)
            {
                switch(ap_act)
                {
                case TCI_S_ACT:
                    result = tci_activate_send_channel(channel);
                    break;

                case TCI_R_ACT:
                    result = tci_activate_receive_channel(channel);
                    break;

                case TCI_D_ACT:
                    result = tci_deactivate_channel(channel);
                    break;

                case TCI_MON:

                    if (channel > TCI_MAX_CHANNEL_NUMBER)
                    {
                        result = RN_VALUE_OUT_OF_RANGE;
                    } 
                    else if (channel < TCI_MIN_CHANNEL_NUMBER)
                    {
                        result = RN_VALUE_OUT_OF_RANGE;
                    }
                    else
                    {
                        tci_mon[channel] = true;
                         
                    }

                    break;

                case TCI_MOFF:

                    if (channel > TCI_MAX_CHANNEL_NUMBER)
                    {
                        result = RN_VALUE_OUT_OF_RANGE;
                    } 
                    else if (channel < TCI_MIN_CHANNEL_NUMBER)
                    {
                        result = RN_VALUE_OUT_OF_RANGE;
                    }
                    else
                    {
                        tci_mon[channel] = false;
                    }

                    break;

                case TCI_UNDEF:
                    tci_help();
                    break;
                }
            }
            else
            {
                tci_help();
            }
            
            if(result != RN_OK)
            {
                sprintf(help_text,">>>>> %s \r\n",rn_result_text(result));console_write(help_text);
            }

            xReturn = pdFALSE;
            lParameterNumber = 0L;
        }
    }
    return xReturn;
}    
/* ----------------------------------------------------------- */

uint8_t decode_uint8(uint8_t b1, uint8_t b2, uint8_t b3)
{
    return (b1 & b2) | (b1 & b3) | (b2 & b3);
}
uint32_t testmax(uint32_t a,uint32_t b)
{
    return(a<b)? b:a;
}
static void  ap_list(void)
{

    if (ap_sim_data.connected)
    {
        sprintf(log_text,"Connected address:  %d\r\n",ap_sim_data.registered_address);          console_write(log_text);
        sprintf(log_text,"EQMID %d \r\n",ap_sim_data.eqmid);    console_write(log_text);
        sprintf(log_text,"RNID  %d \r\n",ap_sim_data.rnid);                                 console_write(log_text);
    }
    else
    {
        sprintf(log_text,"Not connected\r\n");
        console_write(log_text);
    }
}

static void  ap_stat(void)
{
    uint8_t i,j;

    if (!ap_sim_data.connected)
    { 
        sprintf(log_text,"Not connected\r\n"); console_write(log_text);
        return;
    }

    for(i=RN_MIN_ADDRESS;i<=RN_MAX_ADDRESS;i++)
    {
        if(i != ap_sim_data.registered_address)
        {
            if(ap_connected[i])
            {
                sprintf(log_text,"A = %d \r\n",i); console_write(log_text);
                sprintf(log_text,"apoll_req  count %d\r\n",poll_stat[i].apoll_req_count); console_write(log_text);
                sprintf(log_text,"apoll_rsp  count %d\r\n",poll_stat[i].apoll_rsp_count); console_write(log_text);
                sprintf(log_text,"apoll diff count %d\r\n",poll_stat[i].apoll_req_count - poll_stat[i].apoll_rsp_count); console_write(log_text);

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
static void ap_help(void)
{
    sprintf(help_text,"Command Format: ap <parameters>\r\n\r\n"); console_write(help_text);

    sprintf(help_text,"Parameter(s)     Meaning\r\n"); console_write( help_text);
    sprintf(help_text,"--------------   -------\r\n"); console_write( help_text);
    sprintf(help_text,"l                List ap_sim data\r\n"); console_write( help_text);
    sprintf(help_text,"apon             Addressed polling ON \r\n"); console_write( help_text);
    sprintf(help_text,"apoff            Addressed polling OFF \r\n"); console_write( help_text);
    sprintf(help_text,"bpon             Broadcast polling ON \r\n"); console_write( help_text);
    sprintf(help_text,"bpoff            Broadcast polling OFF \r\n"); console_write( help_text);
    sprintf(help_text,"s                List ap_sim statistics\r\n"); console_write( help_text);
    sprintf(help_text,"hoho             Who is there?\r\n"); console_write( help_text);
}

static portBASE_TYPE ap_test_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

    int8_t *pcParameter;
    portBASE_TYPE lParameterStringLength, xReturn;
    static portBASE_TYPE lParameterNumber = 0;
    char parstring[8];
    static msg_buf_t *par_buf;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
    write buffer length is adequate, so does not check for buffer overflows. */

    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;

    configASSERT( pcWriteBuffer );
    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    if( lParameterNumber == 0 )
    {
        ap_test = AP_TEST_UNDEF;
		/* Next time the function is called the first parameter will be echoed back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed back yet. */
		xReturn = pdPASS;
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
            if (1 == lParameterNumber)
            {
                memset( pcWriteBuffer, 0x00, xWriteBufferLen );
                
                /* Obtain the first parameter string. */
                sprintf(parstring,"");
                strncat(parstring, ( const char * ) pcParameter, lParameterStringLength );
                
                if      (0 == strcmp((const char *)parstring,"t"))
                {
                    ap_test = AP_TEST_1;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"m"))
                {
                    ap_test = AP_TEST_2;
                    par_buf = msg_buf_alloc(21);
                } 
                else  if      (0 == strcmp((const char *)parstring,"s"))
                {
                    ap_test = AP_STAT;
                    par_buf = msg_buf_alloc(21);
                } 
                else if      (0 == strcmp((const char *)parstring,"l"))
                {
                    ap_test = AP_LIST;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"hoho"))
                {
                    ap_test = AP_HOHO;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"soff"))
                {
                    ap_test = AP_SYNC_OFF;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"apon"))
                {
                    ap_test = AP_APOLL_ON;
                    par_buf = msg_buf_alloc(21);
                }
               
                else if      (0 == strcmp((const char *)parstring,"apoff"))
                {
                    ap_test = AP_APOLL_OFF;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"bpon"))
                {
                    ap_test = AP_BPOLL_ON;
                    par_buf = msg_buf_alloc(21);
                }
                else if      (0 == strcmp((const char *)parstring,"bpoff"))
                {
                    ap_test = AP_BPOLL_OFF;
                    par_buf = msg_buf_alloc(21);
                }
                else
                {
                    ap_test = AP_TEST_UNDEF;
                    sprintf(help_text,"Parstring not decoded (%s) \r\n\r\n",parstring); console_write(help_text);
                    //sprintf(help_text,"Syntax error \r\n"); console_write(help_text);
                    return pdFALSE;
                }
            }
            else
            {
                /* Obtain the NEXT parameter string. */

                if (lParameterStringLength > 2)
                {
                    return pdFALSE;
                }

                sprintf(parstring,"");
                strncat(parstring, ( const char * ) pcParameter, lParameterStringLength );

                if(lParameterNumber <= 23)
                {
                    par_buf->data[lParameterNumber-2] = strtol (parstring,NULL,16);

                    msg_set_data_size(par_buf,(uint32_t) lParameterNumber-1);
                }

                memset( pcWriteBuffer, 0x00, xWriteBufferLen );
            }
            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            // Execute order
            switch(ap_test)
            {
            case AP_TEST_UNDEF:
                //sprintf(help_text,"Syntax error \r\n"); console_write(help_text);
                ap_help();
                break;

            case AP_HOHO:
                ap_hoho(rn_tick);
                msg_buf_free(par_buf);
                break;

            case AP_SYNC_ON:
                rni_sync_on();
                msg_buf_free(par_buf);
                break;

            case AP_SYNC_OFF:
                rni_sync_off();
                msg_buf_free(par_buf);
                break;

            case AP_BPOLL_ON:
                ap_bpoll_on = true;
                msg_buf_free(par_buf);
                break;

            case AP_BPOLL_OFF:
                ap_bpoll_on = false;
                msg_buf_free(par_buf);
                break;

            case AP_APOLL_ON:
                ap_apoll_on = true;
                apoll_stat_clear();
                msg_buf_free(par_buf);
                break;

            case AP_LIST:
                ap_list();
                msg_buf_free(par_buf);
                break;

             case AP_STAT:
                ap_stat();
                msg_buf_free(par_buf);
                break;

            case AP_APOLL_OFF:
                ap_apoll_on = false;
                msg_buf_free(par_buf);
                break;

            case AP_TEST_1:
                printf(" %02X %02X %02X %02X \r\n",decode_uint8(par_buf->data[0],par_buf->data[1],par_buf->data[2]),par_buf->data[0],par_buf->data[1],par_buf->data[2]);
                msg_buf_free(par_buf);
                break;

            case AP_TEST_2:
                sprintf(help_text,"%d \r\n",testmax(par_buf->data[0],par_buf->data[1])); 
                console_write(help_text);
                 
                sprintf(help_text," %d %d %d \r\n", testmax(par_buf->data[0],par_buf->data[1]),par_buf->data[0],par_buf->data[1]);
                console_write(help_text);
                msg_buf_free(par_buf);
                break;
            }
            
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
void true_voice(tvRxChannels_t tvRxChannels, tvMicChannels_t tvMicChannels, tvTxChannels_t tvTxChannels, tvSpeakerChannels_t tvSpeakerChannels)
{
    //tci_voice(
    tvRxChannels.psPilot = NULL;
}

void tci_voice(tci_channel_t *tci_channel)
{
    int i;

    for(i=0;i < 12;i++)
    {
        tci_channel->channel[i] = NULL;
    }
}