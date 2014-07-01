/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : rn_cmd.c
* Author            : Per Erik Sundvisson
* Description       : Definition of command interface
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

/*
-----------------------------------------------------------
RTOS includes
-------------------------------------------------------------
*/

#include "rtos.h"

/*-----------------------------------------------------------
Project includes
-------------------------------------------------------------
*/

#include "rni.h"
#include "rn_log.h"
#include "rn_rlc_cc.h"
#include "rn_cmd.h"
#include "rn_phy.h"
#include "rn_mac_cc.h"
/*
-----------------------------------------------------------
Static variables
-------------------------------------------------------------
*/
typedef enum
{
    RN_SEND_UNDEF,
    RN_SEND_AA,
    RN_SEND_BA,
    RN_SEND_AR,
    RN_SEND_BR,
    RN_SEND_RR,
    RN_SEND_HELP
} rn_send_t;                // rn_send  variants

rn_send_t   rn_send     = RN_SEND_UNDEF;

typedef enum
{
    RN_PHY_UNDEF,
    RN_PHY_I,
    RN_PHY_M,
    RN_PHY_S,
    RN_PHY_HELP
} rn_phy_cmd_t;             // rn_phy_cmd  variants

rn_phy_cmd_t    rn_phy      = RN_PHY_UNDEF;

typedef enum
{
    RN_MAC_UNDEF,
    RN_MAC_S,
    RN_MAC_L,
    RN_MAC_HELP
} rn_mac_cmd_t;             // rn_mac_cmd  variants

rn_mac_cmd_t    rn_mac      = RN_MAC_UNDEF;

typedef enum
{
    RN_UNDEF_CMD,
    RN_L_CMD,
    RN_S_CMD,

    RN_APOLL_ON_CMD,
    RN_APOLL_OFF_CMD,

    RN_BPOLL_ON_CMD,
    RN_BPOLL_OFF_CMD,

    RN_HOHO_CMD,
    RN_HELP_CMD
} rn_cmd_t;             // rn_cmd  variants

rn_cmd_t   rn_cmd = RN_UNDEF_CMD;

static uint32_t             eqmid           = 0x00;
static uint8_t              rnid            = 0x00;

#define MAX_CHARS 512    
static char help_text[MAX_CHARS + 1];   // Defined here to save stack. Only to be used by help command

/*
-----------------------------------------------------------
Static function declarations
-------------------------------------------------------------
*/

static void send_rni_list();

/*-----------------------------------------------------------
Command declarations, including static function declarations
-------------------------------------------------------------*/

// ----------------------------------------------------------
// rni-start
// ----------------------------------------------------------
static portBASE_TYPE rn_start_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_start_cmd =
{
	( const int8_t * const ) "rni-start",
	( const int8_t * const ) "rni-start   <rnid> <eqmid>      Start Radio Network with specified rnid and eqmid\r\n",
    //                        ---------   -----------------   ---------
	rn_start_command, /* The function to run. */
	2 /* Number of parameters expected */
};

// ----------------------------------------------------------
// rni-connect
// ----------------------------------------------------------
static portBASE_TYPE rni_connect_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rni_connect_cmd =
{
	( const int8_t * const ) "rni-connect", /* The command string to type. */
	( const int8_t * const ) "rni-connect <rnid> <eqmid>      Connect to Radio Network with specified rnid and eqmid\r\n",
    //                        ---------   -----------------   ---------
	rni_connect_command, /* The function to run. */
	2 /* Number of parameters expected  */
};

// ----------------------------------------------------------
// rni-listen
// ----------------------------------------------------------
static portBASE_TYPE rni_sniff_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rni_sniff_cmd =
{
	( const int8_t * const ) "rni-sniff", /* The command string to type. */
	( const int8_t * const ) "rni-sniff   <rnid>              Start radio sniffer for specified rnid\r\n",
    //                        ---------   -----------------   ---------
	rni_sniff_command, /* The function to run. */
	1 /* Number of parameters expected  */
};
// ----------------------------------------------------------
// rni-undef
// ----------------------------------------------------------
static portBASE_TYPE rni_undef_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rni_undef_cmd =
{
	( const int8_t * const ) "rni-undef", /* The command string to type. */
	( const int8_t * const ) "rni-undef                       Undefine (Stop/Disconnect) unit from Radio Network \r\n",
    //                        ---------   -----------------   ---------
	rni_undef_command, /* The function to run. */
	0 /* Number of parameters expected */
};

// ----------------------------------------------------------
// rn-send
// ----------------------------------------------------------
static portBASE_TYPE rn_send_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_send_cmd =
{
	( const int8_t * const ) "rn-send",
	( const int8_t * const ) "rn-send     <parameter(s)>      Send radio message. \"rn-send ?\" lists parameters\r\n",
    //                        ---------   -----------------   ---------
	rn_send_command, /* The function to run. */
	-1 /* The user can enter any number of parameters  */
};
// ----------------------------------------------------------
// rn
// ----------------------------------------------------------
static portBASE_TYPE rn_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_test_cmd =
{
	( const int8_t * const ) "rn",
	( const int8_t * const ) "rn          <parameter(s)>      RN Test Commands. \"rn ?\" lists parameters  \r\n",
    //                        ---------   -----------------   ---------
	rn_command, /* The function to run. */
	-1 /* The user can enter any number of parameters  */
};

// ----------------------------------------------------------
// phy
// ----------------------------------------------------------
static portBASE_TYPE phy_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_phy_cmd =
{
	( const int8_t * const ) "phy",
	( const int8_t * const ) "phy         <parameter(s)>      PHY test commands. \"phy ?\" lists parameters\r\n",
//                            ---------   -----------------   ---------
	phy_command, /* The function to run. */
	-1 /* The user can enter any number of parameters  */
};


// ----------------------------------------------------------
// mac
// ----------------------------------------------------------
static portBASE_TYPE mac_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

static const CLI_Command_Definition_t rn_mac_cmd =
{
	( const int8_t * const ) "mac",
	( const int8_t * const ) "mac         <parameter(s)>      MAC test commands. \"mac ?\" lists parameters\r\n",
//                            ---------   -----------------   ---------
	mac_command, /* The function to run. */
	-1 /* The user can enter any number of parameters  */
};

/*----------------------------------------------------------------------------
Global function definitions
-------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name    : rn_cmd_module_init
* Function Purpose : Initiation of data in module, if any                
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_cmd_module_init(void)
{
	return;
}

/******************************************************************************
* Function Name    : rn_cmd_module_start
* Function Purpose : Start of module                
* Input parameters : start phase 
* Return value     : void
******************************************************************************/

void rn_cmd_module_start(uint8_t start_phase)
{
    if (1 == start_phase)
    {

        FreeRTOS_CLIRegisterCommand( &rn_start_cmd );
        FreeRTOS_CLIRegisterCommand( &rni_connect_cmd );
        FreeRTOS_CLIRegisterCommand( &rni_sniff_cmd );
        FreeRTOS_CLIRegisterCommand( &rni_undef_cmd );
        FreeRTOS_CLIRegisterCommand( &rn_send_cmd );
        FreeRTOS_CLIRegisterCommand( &rn_test_cmd );
        FreeRTOS_CLIRegisterCommand( &rn_phy_cmd );
        FreeRTOS_CLIRegisterCommand( &rn_mac_cmd );

    }

    return;
}
/*-----------------------------------------------------------------------------
Static function definitions
-------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
TEST COMMAND FUNCTIONS
-------------------------------------------------------------------------------*/

/******************************************************************************
* Function Name    : rni_connect_command
* Function Purpose : Command to connect RN unit to RN                 
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rni_connect_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	 int8_t                  *pcParameter;
    portBASE_TYPE           lParameterStringLength, xReturn;
    static portBASE_TYPE    lParameterNumber = 0;
    rn_result_t             result;

	configASSERT( pcWriteBuffer );

    memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    
    if( lParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
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

        /* Sanity check something was returned. */
        configASSERT( pcParameter );

        if(1 == lParameterNumber)
        {
            rnid = (uint8_t) atoi((const char*) pcParameter);
        }

        if(2 == lParameterNumber)
        {
            eqmid = (uint32_t) atol((const char*) pcParameter);
        }

        /* If this is the last of the two parameters then there are no more
        strings to return after this one. */
        if( lParameterNumber == 2L )
        {
            /* If this is the last of the two parameters then there are no more
			strings to return after this one. */
            result = rni_connect(rnid,eqmid);
            //console_write(rn_result_text(result));
            sprintf(help_text,">>>>> %s (%d) \r\n",rn_result_text(result),result); console_write(help_text);
            xReturn = pdFALSE;
            lParameterNumber = 0L;
         
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
	}

	return xReturn;
}

/******************************************************************************
* Function Name    : rni_undef_command
* Function Purpose : Command for undefining RN unit                 
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rni_undef_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{

    rn_result_t result;

    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    
    configASSERT( pcWriteBuffer );
    
    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    result = rni_undefine();

    // sprintf(help_text,">>>>> %s \r\n",rn_result_text(result)); console_write(help_text);
    sprintf(help_text,">>>>> %s (%d) \r\n",rn_result_text(result),result); console_write(help_text);
    return pdFALSE;
}
/******************************************************************************
* Function Name    : rn_start_command
* Function Purpose : Command to start RN, unit will become master                 
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rn_start_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
    int8_t                  *pcParameter;
    portBASE_TYPE           lParameterStringLength, xReturn;
    static portBASE_TYPE    lParameterNumber = 0;
    rn_result_t             result;

	configASSERT( pcWriteBuffer );

    memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    
    if( lParameterNumber == 0 )
	{
		/* Next time the function is called the first parameter will be echoed back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoe back yet. */
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

        /* Sanity check something was returned. */
        configASSERT( pcParameter );

        if(1 == lParameterNumber)
        {
            rnid = (uint8_t) atoi((const char*) pcParameter);
        }

        if(2 == lParameterNumber)
        {
            eqmid = (uint32_t) atol((const char*) pcParameter);
        }

        if( lParameterNumber == 2L )
        {
            /* If this is the last of the two parameters then there are no more
			strings to return after this one. */
            result = rni_start(rnid,eqmid);
          
            sprintf(help_text,">>>>> %s (%d) \r\n",rn_result_text(result),result); console_write(help_text);
            //sprintf(help_text,">>>>> %s \r\n",rn_result_text(result)); console_write(help_text);
            //send_rni_list();

          /*  sprintf( ( char * ) pcWriteBuffer,">>>>> ");
            strcat( (char  * ) pcWriteBuffer,rn_result_text(result));
            strcat( ( char * ) pcWriteBuffer, "\r\n");*/

            xReturn = pdFALSE;
            lParameterNumber = 0L;
         
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
	}

	return xReturn;
}

/******************************************************************************
* Function Name    : rn_send_help
* Function Purpose : Help text for rn-send_command                 
* Input parameters : void
* Return value     : void
******************************************************************************/
void rn_send_help(void)
{
    sprintf(help_text,"Command Format: rn-send <parameters>\r\n\r\n"); console_write( help_text);
   
    sprintf(help_text,"Parameters                       Meaning\r\n"); console_write( help_text);
    sprintf(help_text,"----------                       -------\r\n"); console_write( help_text);
    sprintf(help_text,"aa <address> <byte> <byte> ...   Send Addressed APP message \r\n"); console_write( help_text);
    sprintf(help_text,"ba <byte> <byte> ...             Send Broadcast APP message \r\n"); console_write( help_text);

    sprintf(help_text,"ar <address> <byte> <byte> ...   Send Addressed RRC message \r\n"); console_write( help_text);
    sprintf(help_text,"br <byte> <byte> ...             Send Broadcast RRC message \r\n"); console_write( help_text);

    sprintf(help_text,"rr <byte> <byte> ...             Send Registration RRC message \r\n"); console_write( help_text);
}

/******************************************************************************
* Function Name    : rn_send_command
* Function Purpose : rn-send_command callback implementation                
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rn_send_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
    rn_result_t rn_result = RN_OK;
    int8_t *pcParameter;
    portBASE_TYPE lParameterStringLength, xReturn;
    static portBASE_TYPE lParameterNumber = 0;
    static msg_buf_t *send_buf;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
    write buffer length is adequate, so does not check for buffer overflows. */

    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;

    configASSERT( pcWriteBuffer );
     
    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    if( lParameterNumber == 0 )
    {
        /* The first time the function is called after the command has been
        entered just a header string is returned. */
        //sprintf( ( char * ) pcWriteBuffer, "The parameters were:\r\n" );
        rn_send = RN_SEND_UNDEF;
		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
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
              

                if      (0 == strncmp((const char *)pcParameter,"aa",max(2,lParameterStringLength)))
                {
                     rn_send = RN_SEND_AA;
                }
                else if (0 == strncmp((const char *)pcParameter,"ba",max(2,lParameterStringLength)))
                {
                     rn_send = RN_SEND_BA;
                } 
                else if (0 == strncmp((const char *)pcParameter,"ar",max(2,lParameterStringLength)))
                {
                     rn_send = RN_SEND_AR;

                }
                else if (0 == strncmp((const char *)pcParameter,"br",max(2,lParameterStringLength)))
                {
                     rn_send = RN_SEND_BR;
                } 
                else if (0 == strncmp((const char *)pcParameter,"rr",max(2,lParameterStringLength)))
                {
                     rn_send = RN_SEND_RR;
                }
                else if (0 == strncmp((const char *)pcParameter,"?",max(1,lParameterStringLength)))
                {
                    rn_send = RN_SEND_HELP;
                }
                else
                {
                    rn_send = RN_SEND_HELP;
                    
                    //return pdFALSE;
                }
                send_buf = msg_buf_alloc(21);   // Max size is 20 + destination address
                msg_set_data_size(send_buf,0);  // No data yet
            }
            else
            {
                /* Obtain the NEXT parameter string. */

                if (lParameterStringLength > 2)
                {
                    sprintf(help_text,">>>>> Illegal parameter size %d, par no %d \r\n",lParameterStringLength,lParameterNumber);
                    console_write(help_text);
                    rn_send_help();
                    return pdFALSE;
                }

                if(lParameterNumber <= 23)
                {
                    send_buf->data[lParameterNumber-2] = (uint8_t)strtol ((const char *)pcParameter,NULL,16);

                    msg_set_data_size(send_buf,(uint32_t) lParameterNumber-1);
                }

                if(lParameterNumber == 2)
                {
                    send_buf->rn_receiver = send_buf->data[0]; // Saving destination address in case it is an addressed message
                }

                memset( pcWriteBuffer, 0x00, xWriteBufferLen );
              
            }
            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            switch(rn_send)
            {
            case RN_SEND_UNDEF:
                rn_send_help();
                break;

            case RN_SEND_HELP:
                rn_send_help();
                msg_buf_free(send_buf);
                break;

            case RN_SEND_AA:
                msg_buf_header_remove(send_buf,1); // Remove receiver from sent data
                rn_result = cci_send(send_buf->rn_receiver,send_buf);
                break;

            case RN_SEND_BA:
                rn_result = cci_broadcast(send_buf);
                break;
            case RN_SEND_AR:
                msg_buf_header_remove(send_buf,1); // Remove receiver from sent data
                rn_result = rn_rccr_send(send_buf->rn_receiver,send_buf);
                break;
            case RN_SEND_BR:
                rn_result = rn_rccr_broadcast(send_buf);
                break;
            case RN_SEND_RR:
                rn_result = rn_rcc_send(send_buf);
                break;
            }
            /* No more parameters were found.  Make sure the write buffer does
            not contain a valid string. */

            if(rn_result != RN_OK)
            {
                sprintf(help_text,">>>>> %s (%d) \r\n",rn_result_text(rn_result),rn_result); console_write(help_text);
               /* sprintf(help_text,"rn_result = %s (%d)\r\n",rn_result_text(rn_result),rn_result);
                console_write(help_text);*/
            }

            pcWriteBuffer[ 0 ] = 0x00;

            /* No more data to return. */
            xReturn = pdFALSE;

            /* Start over the next time this command is executed. */
            lParameterNumber = 0;
        }
    }

    return xReturn;
}

/******************************************************************************
* Function Name    : rni_sniff_command
* Function Purpose : rni-sniff_command implementation, 
*                    configure unit as sniffer                
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rni_sniff_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	int8_t                  *pcParameter;
    portBASE_TYPE           lParameterStringLength, xReturn;
    static portBASE_TYPE    lParameterNumber = 0;
    rn_result_t             result;

	configASSERT( pcWriteBuffer );

    memset( pcWriteBuffer, 0x00, xWriteBufferLen );
    
    if( lParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
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

        /* Sanity check something was returned. */
        configASSERT( pcParameter );

        if(1 == lParameterNumber)
        {
            rnid = (uint8_t) atoi((const char*) pcParameter);
        }

        /* If this is the last of the two parameters then there are no more
        strings to return after this one. */
        if( lParameterNumber == 1L )
        {
            /* There are no more strings to return after this one. */
            result = rni_sniff(rnid);
            sprintf(help_text,">>>>> %s (%d) \r\n",rn_result_text(result),result); console_write(help_text);
            
            xReturn = pdFALSE;
            lParameterNumber = 0L;
         
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
	}

	return xReturn;
}

/******************************************************************************
* Function Name    : rn_help
* Function Purpose : Help text for rn command             
* Input parameters : void
* Return value     : void
******************************************************************************/
void  rn_help(void)
{
     sprintf(help_text,"Command Format: rn <parameters>\r\n\r\n");              console_write(help_text);
     sprintf(help_text,"Parameter       Meaning\r\n");                          console_write(help_text);
     sprintf(help_text,"---------        -------\r\n");                         console_write(help_text);
     sprintf(help_text,"l <a>           List RN management information \r\n");  console_write(help_text);
     sprintf(help_text,"apon <p>|<p a>  RN addressed Polling ON \r\n");         console_write(help_text);
     sprintf(help_text,"apoff <a>       RN addressed Polling OFF \r\n");        console_write(help_text);
     sprintf(help_text,"bpon <p>|<p a>  RN Broadcast Polling ON \r\n");         console_write(help_text);
     sprintf(help_text,"bpoff <a>       RN Broadcast Polling OFF \r\n");        console_write(help_text);
     sprintf(help_text,"s <a>           List RN Statistics\r\n");               console_write(help_text);
     sprintf(help_text,"hoho            Who is there?\r\n");                    console_write(help_text);

     sprintf(help_text,"<p> denotes optional period in RN ticks\r\n");          console_write(help_text);
     sprintf(help_text,"<a> denotes optional remote RN-address\r\n");           console_write(help_text);
    
}
/******************************************************************************
* Function Name    : send_rni_list
* Function Purpose : Send RNI_LIST message to RN manager       
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rni_list()
{
    msg_buf_t *test_message;

    test_message = msg_buf_alloc(0);   // No data in message
    test_message->pd = PD_RNI;
    test_message->mt = RNI_LIST;
    
    rn_log_msg_send(RN_CMD,test_message);
    rtos_send(rnm_queue_id(),test_message);
}

/******************************************************************************
* Function Name    : send_rni_stat
* Function Purpose : Send RNI_STAT message to RN manager       
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_rni_stat()
{
    msg_buf_t *test_message;

    test_message = msg_buf_alloc(0);   // No data in message
    test_message->pd = PD_RNI;
    test_message->mt = RNI_STAT;
    
    rn_log_msg_send(RN_CMD,test_message);
    rtos_send(rnm_queue_id(),test_message);
}

/******************************************************************************
* Function Name    : rn_command
* Function Purpose : rn command implementation                
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE rn_command( 
    int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
    int8_t *pcParameter;
    portBASE_TYPE lParameterStringLength, xReturn;
    static portBASE_TYPE lParameterNumber = 0;
    char parstring[8];
    static msg_buf_t *test_message;
    uint8_t rn_addr;
    uint8_t poll_period;
    
    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
    write buffer length is adequate, so does not check for buffer overflows. */

    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;

    configASSERT( pcWriteBuffer );

    if( lParameterNumber == 0 )
    {
		lParameterNumber = 1L;
        rn_cmd = RN_UNDEF_CMD;
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
            if (1 == lParameterNumber)
            {
                memset( pcWriteBuffer, 0x00, xWriteBufferLen );
               
                /* Obtain the first parameter string. */
                sprintf(parstring,"");
                strncat(parstring, ( const char * ) pcParameter, lParameterStringLength );

                if      (0 == strcmp((const char *)parstring,"l"))
                {
                    rn_cmd = RN_L_CMD;
                }
                else if (0 == strcmp((const char *)parstring,"s"))
                {
                    rn_cmd = RN_S_CMD;
                } 
                else if (0 == strcmp((const char *)parstring,"apon"))
                {
                    rn_cmd = RN_APOLL_ON_CMD;
                } 
                else if (0 == strcmp((const char *)parstring,"apoff"))
                {
                    rn_cmd = RN_APOLL_OFF_CMD;
                }   
                else if (0 == strcmp((const char *)parstring,"bpon"))
                {
                    rn_cmd = RN_BPOLL_ON_CMD;
                } 
                else if (0 == strcmp((const char *)parstring,"bpoff"))
                {
                    rn_cmd = RN_BPOLL_OFF_CMD;
                }  
                else if (0 == strcmp((const char *)parstring,"hoho"))
                {
                    rn_cmd = RN_HOHO_CMD;
                } 
                else if (0 == strcmp((const char *)parstring,"?"))
                {
                    rn_cmd = RN_HELP_CMD;
                } 
                else
                {
                   sprintf(help_text,"Parameter not decoded (%s) \r\n\r\n",parstring); console_write(help_text);
                   rn_cmd = RN_HELP_CMD;
                }
                test_message = msg_buf_alloc(20);   // Max size of test message, actual size set later
                msg_set_data_size(test_message,0);  // No data so far
            }
            else
            {
                /* Obtain the NEXT parameter string. */

                if (lParameterStringLength > 3)
                {
                    return pdFALSE;
                }

                sprintf(parstring,"");
                strncat(parstring, ( const char * ) pcParameter, lParameterStringLength );

                if(lParameterNumber <= 10)  // LIMIT FOR ACCEPTED PARAMETERS so far
                {
                    test_message->data[lParameterNumber-2] = (uint8_t)strtol (parstring,NULL,10);

                    msg_set_data_size(test_message,(uint32_t) lParameterNumber-1);
                }

                memset( pcWriteBuffer, 0x00, xWriteBufferLen );
               
            }
            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            // Execute command
            switch(rn_cmd)
            {
            case RN_HELP_CMD:
                rn_help();
                msg_buf_free(test_message);
                break;

            case RN_UNDEF_CMD:
                rn_help();
                break;

            case RN_L_CMD:
               
                if(0 == msg_return_data_size(test_message))
                {
                    send_rni_list();
                    msg_buf_free(test_message); 
                } 
                else if (1 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,0);
                    msg_put_uint8(test_message,0,RRC_LIST);
                    rn_rccr_send(rn_addr,test_message);
                    sprintf(help_text,"RRC_LIST sent to addr %d \r\n",rn_addr);console_write(help_text);
                }
                else
                {
                    rn_help();
                }
                break;

            case RN_S_CMD:
                if(0 == msg_return_data_size(test_message))
                {
                    send_rni_stat();
                    msg_buf_free(test_message);
                }
                else if (1 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,0);
                    msg_put_uint8(test_message,0,RRC_STAT);
                    rn_rccr_send(rn_addr,test_message);
                    sprintf(help_text,"RRC_STAT sent to addr %d \r\n",rn_addr);console_write(help_text);
                }
                else
                {
                    rn_help();
                }
                break;

            case RN_APOLL_ON_CMD:
                if(1 >= msg_return_data_size(test_message))
                {
                    test_message->pd = PD_RNI;
                    test_message->mt = RNI_APOLL_ON;
                    rn_log_msg_send(RN_CMD,test_message);
                    rtos_send(rnm_queue_id(),test_message);
                }  
                else if (2 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,1);
                    poll_period = msg_return_uint8(test_message,0);

                    msg_buf_header_add(test_message,1);
                    
                    msg_put_uint8(test_message,0,RRC_RNI_CMD);  // Container
                    msg_put_uint8(test_message,1,RNI_APOLL_ON); // Remote command
                    msg_put_uint8(test_message,2,poll_period);
                     
                    rn_rccr_send(rn_addr,test_message);

                }
                break;

            case RN_APOLL_OFF_CMD:

                if(0 == msg_return_data_size(test_message))
                {
                    test_message->pd = PD_RNI;
                    test_message->mt = RNI_APOLL_OFF;
                    rn_log_msg_send(RN_CMD,test_message);
                    rtos_send(rnm_queue_id(),test_message);
                }
                else if (1 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,0);
                    msg_buf_header_add(test_message,1);

                    msg_put_uint8(test_message,0,RRC_RNI_CMD);  // Container
                    msg_put_uint8(test_message,1,RNI_APOLL_OFF); // Remote command
                    rn_rccr_send(rn_addr,test_message);
                }
                break;

            case RN_BPOLL_ON_CMD:
                if(1 >= msg_return_data_size(test_message))
                {
                    test_message->pd = PD_RNI;
                    test_message->mt = RNI_BPOLL_ON;
                    rn_log_msg_send(RN_CMD,test_message);
                    rtos_send(rnm_queue_id(),test_message);
                }
                else if (2 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,1);
                    poll_period = msg_return_uint8(test_message,0);

                    msg_buf_header_add(test_message,1);

                    msg_put_uint8(test_message,0,RRC_RNI_CMD);  // Container
                    msg_put_uint8(test_message,1,RNI_BPOLL_ON); // Remote command
                    msg_put_uint8(test_message,2,poll_period);

                    rn_rccr_send(rn_addr,test_message);

                }
                break;

            case RN_BPOLL_OFF_CMD:
                if(0 == msg_return_data_size(test_message))
                {
                    test_message->pd = PD_RNI;
                    test_message->mt = RNI_BPOLL_OFF;
                    rn_log_msg_send(RN_CMD,test_message);
                    rtos_send(rnm_queue_id(),test_message);
                }
                else if (1 == msg_return_data_size(test_message))
                {

                    rn_addr = msg_return_uint8(test_message,0);
                    msg_buf_header_add(test_message,1);

                    msg_put_uint8(test_message,0,RRC_RNI_CMD);  // Container
                    msg_put_uint8(test_message,1,RNI_BPOLL_OFF); // Remote command
                    rn_rccr_send(rn_addr,test_message);
                }
                break;

            case RN_HOHO_CMD:
                test_message->pd = PD_RNI;
                test_message->mt = RNI_HOHO;
                rn_log_msg_send(RN_CMD,test_message);
                rtos_send(rnm_queue_id(),test_message);
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

/******************************************************************************
* Function Name    : rn_phy_help
* Function Purpose : Help text for phy command             
* Input parameters : void
* Return value     : void
******************************************************************************/
void  rn_phy_help()
{
    sprintf(help_text,"Command Format: phy <parameters>\r\n\r\n"); console_write( help_text);

    sprintf(help_text,"Parameter Meaning\r\n"); console_write( help_text);
    sprintf(help_text,"--------- -------\r\n"); console_write( help_text);
    sprintf(help_text,"i         Set PHY to IDLE \r\n"); console_write( help_text);
    sprintf(help_text,"m <rnid>  Set PHY to MASTER with specified RNID \r\n"); console_write( help_text);
    sprintf(help_text,"s <rnid>  Set PHY to SLAVE  with specified RNID \r\n"); console_write( help_text);
}

/******************************************************************************
* Function Name    : phy_command
* Function Purpose : phy command implementation              
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE phy_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
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
		/* Next time the function is called the first parameter will be echoed back. */
        
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed back yet. */

        rn_phy = RN_PHY_UNDEF;
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

                if      (0 == strcmp((const char *)parstring,"i"))
                {
                    rn_phy = RN_PHY_I;
                }
                else if (0 == strcmp((const char *)parstring,"m"))
                {
                    rn_phy = RN_PHY_M;
                } 
                else if (0 == strcmp((const char *)parstring,"s"))
                {
                    rn_phy = RN_PHY_S;

                }
                else
                {
                    rn_phy = RN_PHY_HELP;
                }
                par_buf = msg_buf_alloc(21);    // Max size is 20 + destination address
                msg_set_data_size(par_buf,0);           // No data yet
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
                    par_buf->data[lParameterNumber-2] = (uint8_t) strtol (parstring,NULL,16);

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
            // Execute command
            switch(rn_phy)
            {
            case RN_PHY_UNDEF:
                rn_phy_help();
                break;

            case RN_PHY_HELP:
                rn_phy_help();
                msg_buf_free(par_buf);
                break;

            case RN_PHY_I:
                rn_phy_idle_conf();
                msg_buf_free(par_buf);
                break;

            case RN_PHY_M:
                if(msg_return_data_size(par_buf) >= 1) 
                {
                   rn_phy_master_conf(par_buf->data[0]);
                }
                else
                {
                    sprintf(help_text,"RNID must be specified \r\n");console_write(help_text);
                }
                msg_buf_free(par_buf);
                break;

            case RN_PHY_S:
                if(msg_return_data_size(par_buf) >= 1) 
                {
                    rn_phy_slave_conf(par_buf->data[0]);
                }
                else
                {
                    sprintf(help_text,"RNID must be specified \r\n");console_write(help_text);
                }
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

/******************************************************************************
* Function Name    : rn_mac_help
* Function Purpose : Help text for mac command             
* Input parameters : void
* Return value     : void
******************************************************************************/
void  rn_mac_help()
{
    sprintf(help_text,"Command Format: mac <parameters>\r\n\r\n"); console_write( help_text);

    sprintf(help_text,"Parameters Meaning\r\n"); console_write( help_text);
    sprintf(help_text,"---------  -------\r\n"); console_write( help_text);
    sprintf(help_text,"l <a>      Display MAC configuration\r\n"); console_write( help_text);
    sprintf(help_text,"s <a>      Display MAC Statistics\r\n"); console_write( help_text);
    sprintf(help_text,"<a> denotes optional remote RN-address\r\n");           console_write(help_text);
}

/******************************************************************************
* Function Name    : send_mac_stat_cmd
* Function Purpose : Send RPI_MAC_STAT_CMD message to MAC CC      
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_mac_stat_cmd(uint8_t console_address)
{
    msg_buf_t *test_message;

    test_message = msg_buf_alloc(1);
    test_message->pd = PD_RPI;
    test_message->mt = RPI_MAC_STAT_CMD;
    msg_put_uint8(test_message,0,console_address);

    rn_log_msg_send(RN_CMD,test_message);
    rtos_send(rn_mac_cc_input_queue_id(),test_message);
}
/******************************************************************************
* Function Name    : send_mac_list_cmd
* Function Purpose : Send RPI_MAC_LIST_CMD message to MAC CC      
* Input parameters : void
* Return value     : void
******************************************************************************/
static void send_mac_list_cmd(uint8_t console_address)
{
    msg_buf_t *test_message;

    test_message = msg_buf_alloc(1);
    test_message->pd = PD_RPI;
    test_message->mt = RPI_MAC_LIST_CMD;
    msg_put_uint8(test_message,0,console_address);

    rn_log_msg_send(RN_CMD,test_message);
    rtos_send(rn_mac_cc_input_queue_id(),test_message);
}
/******************************************************************************
* Function Name    : mac_command
* Function Purpose : mac command implementation              
* Input parameters : As specified by CLI callback type
* Return value     : As specified by CLI callback type
******************************************************************************/
static portBASE_TYPE mac_command( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
    int8_t *pcParameter;
    portBASE_TYPE lParameterStringLength, xReturn;
    static portBASE_TYPE lParameterNumber = 0;
    char parstring[8];
    static msg_buf_t *test_message;
    uint8_t rn_addr;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
    write buffer length is adequate, so does not check for buffer overflows. */
     
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;

    configASSERT( pcWriteBuffer );
    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    if( lParameterNumber == 0 )
    {
		/* Next time the function is called the first parameter will be echoed back. */
        
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed back yet. */

        rn_phy = RN_MAC_UNDEF;
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

                if      (0 == strcmp((const char *)parstring,"l"))
                {
                    rn_mac = RN_MAC_L;
                }
                else if (0 == strcmp((const char *)parstring,"s"))
                {
                    rn_mac = RN_MAC_S;
                } 
                else
                {
                    rn_mac = RN_MAC_HELP;
                }
                test_message = msg_buf_alloc(21);    // Max size is 20 + destination address
                msg_set_data_size(test_message,0);           // No data yet
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
                    test_message->data[lParameterNumber-2] = (uint8_t) strtol (parstring,NULL,16);

                    msg_set_data_size(test_message,(uint32_t) lParameterNumber-1);
                }

                memset( pcWriteBuffer, 0x00, xWriteBufferLen );

            }

            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            // Execute command
            switch(rn_mac)
            {
            case RN_MAC_UNDEF:
                rn_mac_help();
                break;

            case RN_MAC_HELP:
                rn_mac_help();
                msg_buf_free(test_message);
                break;

            case RN_MAC_S:
                if(0 == msg_return_data_size(test_message))
                {
                    send_mac_stat_cmd(RN_OWN_ADDRESS);
                    msg_buf_free(test_message); 
                } 
                else if (1 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,0);
                    msg_put_uint8(test_message,0,RRC_MAC_STAT);
                    rn_rccr_send(rn_addr,test_message);
                    sprintf(help_text,"RRC_MAC_STAT sent to addr %d \r\n",rn_addr);console_write(help_text);
                }
                else
                {
                    rn_mac_help();
                }
                break;
                
            case RN_MAC_L:
                if(0 == msg_return_data_size(test_message))
                {
                    send_mac_list_cmd(RN_OWN_ADDRESS);
                    msg_buf_free(test_message); 
                } 
                else if (1 == msg_return_data_size(test_message))
                {
                    rn_addr = msg_return_uint8(test_message,0);
                    msg_put_uint8(test_message,0,RRC_MAC_LIST);
                    rn_rccr_send(rn_addr,test_message);
                    sprintf(help_text,"RRC_MAC_LIST sent to addr %d \r\n",rn_addr);console_write(help_text);
                }
                else
                {
                    rn_mac_help();
                }
                break;
            default:
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
