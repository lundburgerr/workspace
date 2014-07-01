/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

#pragma comment( lib, "ws2_32.lib" )

/* Win32 includes. */
#include <WinSock2.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "rni.h"
#include "rtos.h"
#include "rn_phy.h"
#include "phy_sim.h"
#include "rn_log.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE	60

/* Dimensions the buffer into which string outputs can be placed. */
#define cmdMAX_OUTPUT_SIZE	1024

/* Dimensions the buffer passed to the recvfrom() call. */
#define cmdSOCKET_INPUT_BUFFER_SIZE 60

int xClientAddressLength = sizeof( struct sockaddr_in );

static SOCKET comm_Socket = INVALID_SOCKET;



static uint16_t             phy_send_port           =   0;
static uint16_t             phy_receive_port        =   0;
static RTOS_QUEUE_HANDLE_T  tccr_receiver_queue     = NULL;
static RTOS_QUEUE_HANDLE_T  tuca_receiver_queue     = NULL;
static SOCKET prvOpenUDPSocket( uint16_t port );

bool rn_phy_UDP_register_tccr_message_receiver(RTOS_QUEUE_HANDLE_T queue)
{
    tccr_receiver_queue = queue;
    return true;
}
bool rn_phy_UDP_register_tuca_message_receiver(RTOS_QUEUE_HANDLE_T queue)
{
    tuca_receiver_queue = queue;
    return true;
}

/*
 * Open and configure the UDP socket.
 */
void rn_phy_send_port(uint16_t send_port)
{
    phy_send_port = send_port;
    printf("Send port %d \n\r",phy_send_port);

}
void rn_phy_receive_port(uint16_t receive_port)
{
    phy_receive_port = receive_port;
    printf("Receive port %d \n\r",phy_receive_port);
}
void send_to_port(msg_buf_t *transport)
{
    char asciihex[256];
  
    //uint32_t index = TCCR_DATA_POS;

    struct sockaddr_in xClient;
    
    memset( asciihex, 0x00, 256);

    xClient.sin_family = AF_INET;
    xClient.sin_port = htons(phy_send_port);

    /* Assign the loopback address */
    xClient.sin_addr.S_un.S_un_b.s_b1 = 127;
    xClient.sin_addr.S_un.S_un_b.s_b2 = 0;
    xClient.sin_addr.S_un.S_un_b.s_b3 = 0;
    xClient.sin_addr.S_un.S_un_b.s_b4 = 1;

    sendto( comm_Socket, (const char *) transport->data,  transport->size, 0, ( SOCKADDR * ) &xClient, xClientAddressLength );
    msg_buf_free(transport);

}

/*-----------------------------------------------------------*/

/* 
* Task that provides the input for the host commumication between 
*  applications. A UDP port is used. 
*/

void rn_phy_comm_task( void *pvParameters )
{
    long lBytes;
   
    static signed char cInputString[ cmdMAX_INPUT_SIZE ], cOutputString[ cmdMAX_OUTPUT_SIZE ], cLocalBuffer[ 128 ];
  
    volatile int iErrorCode = 0;
    struct sockaddr_in xClient;
    int xClientAddressLength = sizeof( struct sockaddr_in );
    msg_buf_t   *transport;

	/* Just to prevent compiler warnings. */
	( void ) pvParameters;

	/* Attempt to open the socket. */
	comm_Socket = prvOpenUDPSocket(phy_receive_port);

	if( comm_Socket != INVALID_SOCKET )
	{
        for( ;; )
        {
            /* Wait for incoming data on the opened socket. */
            transport = msg_buf_alloc(128);
            //transport = msg_buf_alloc(TCCR_DATA_SIZE);
            lBytes = recvfrom( comm_Socket, (char *) transport->data, transport->size, 0, ( struct sockaddr * ) &xClient, &xClientAddressLength );

            if( lBytes == SOCKET_ERROR )
            {
                /* Something went wrong, but it is not handled by this simple
                example. */
                iErrorCode = WSAGetLastError();
                //ReportError(iErrorCode, "recvfrom()");
                msg_buf_free(transport); 
            }
            else
            {	
                if(transport->data[TUCA_TIMESLOT_POS] > 15)
                {
                    rn_log_text_msg("Corrupt message?",transport);
                    msg_buf_free(transport);
                }

                if(transport->data[TUCA_TIMESLOT_POS] <= 3)
                {
                    transport->mt = TCCR_DATA;
                    transport->pd = PD_TCCR;
                    transport->size = TCCR_DATA_SIZE;
                     
                    //rn_log_text_msg("",transport);

                    //rn_log_msg_all        ("AIR", transport);
                 RTOS_QUEUE_SEND(tccr_receiver_queue, &transport, 0U );
                }
                else
                {
                    transport->mt = TCI_DATA;
                    transport->pd = PD_TCI;
                    transport->size = TCI_DATA_SIZE;
                    
                    //rn_log_text_msg("",transport);

                    RTOS_QUEUE_SEND(tuca_receiver_queue, &transport, 0U );
                }
            }
        } 
    }
    else
	{
		/* The socket could not be opened. */
		vTaskDelete( NULL );
	}
}
/*-----------------------------------------------------------*/

static SOCKET prvOpenUDPSocket( uint16_t port )
{
    WSADATA xWSAData;
    WORD wVersionRequested;
    struct sockaddr_in xServer;
    SOCKET xSocket = INVALID_SOCKET;

    wVersionRequested = MAKEWORD( 2, 2 );
    
    (void) port;

    /* Prepare to use WinSock. */
    if( WSAStartup( wVersionRequested, &xWSAData ) != 0 )
	{
		fprintf( stderr, "Could not open Windows connection.\n" );
	}
	else
	{   
		xSocket = socket( AF_INET, SOCK_DGRAM, 0 );
		if( xSocket == INVALID_SOCKET)
		{
			fprintf( stderr, "Could not create socket.\n" );
			WSACleanup();
		}
		else
		{
			/* Zero out the server structure. */
			memset( ( void * ) &xServer, 0x00, sizeof( struct sockaddr_in ) );

			/* Set family and port. */
			xServer.sin_family = AF_INET;
			xServer.sin_port = htons(phy_receive_port);

			/* Assign the loopback address */
			xServer.sin_addr.S_un.S_un_b.s_b1 = 127;
			xServer.sin_addr.S_un.S_un_b.s_b2 = 0;
			xServer.sin_addr.S_un.S_un_b.s_b3 = 0;
			xServer.sin_addr.S_un.S_un_b.s_b4 = 1;

			/* Bind the address to the socket. */
			if( bind( xSocket, ( struct sockaddr * ) &xServer, sizeof( struct sockaddr_in ) ) == -1 )
			{
				fprintf( stderr, "Could not socket to port %d.\n", phy_receive_port );
				closesocket( xSocket );
				xSocket = INVALID_SOCKET;
				WSACleanup();
			}
		}
	}

	return xSocket;
}

SOCKET prvCloseUDPSocket(SOCKET xSocket)
{  
 
    closesocket( xSocket );			
    WSACleanup();

    return xSocket;
}

