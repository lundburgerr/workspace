/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : maintenance.c
* Author            : Roger Vilmunen ÅF
* Description       : Maintenance functions/misc functions
*                   : -CLI motor
********************************************************************************
* Revision History
* 2013-05-02        : First Issue
*******************************************************************************/

#include <trace/trace.h>
#include <CLI/CLI.h>
#include <SPIcom/SPIcom.h>
#include <DigitalIO/digitalIO.h>
#include <xdc/runtime/knl/Cache.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/ipc/MessageQ.h>
#include <traceMessage.h>

#include <I2Cgen/i2cgen.h>

/* Polling variables, see uart */
extern int uartNewString; /* In uartHandle */
extern char inBuffer[]; /* In uartHandle */

/* Spi test variables */
static volatile int radioDone = 0;
static int spiTestAcive = 0;

/* DSP connection check done */
int dspConnectionCheckDone = 1;

// Predef
extern void i2cgen_test(UInt8 led);

/* Spi test callback function */
/* Spi interface call this function when hw process is done */
void setRadioDone(Int8 status) {
	radioDone = 1;
}

/* Digital io IRQ test function */
void radioIRQ(Uint8 io) {
	TRACE1("Radio IRQ1 called io %d\r\n",io);
}

void radioIRQ2(Uint8 io) {
	TRACE1("Radio IRQ2 called io %d\r\n",io);
}

/* CLI Command callback */
portBASE_TYPE maintenanceCmdCbk(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t * pcCommandString ) {
	portBASE_TYPE len;
	char *par1;
	MessageQ_QueueId remoteQueueId;
	int status;
	MessageQ_Msg     msg;

	/* Get param */
	par1 = (char *) (FreeRTOS_CLIGetParameter(pcCommandString, 1, &len ));

	/* Check param */
	if(par1) {
		if(par1[0] == '1') {
			spiTestAcive = 1;
			System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nspitest is %d \r\n", spiTestAcive);
		} else if(par1[0] == 'd') {
			TRACE("\r\nSleep starts 5 min\r\n")
			Task_sleep(1000*60*5);
			TRACE("Sleep done\r\n")
			System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nSleep done \r\n");
		} else if(par1[0] == 'c') {
	        status = MessageQ_open("DSPQueue", &remoteQueueId);
	        if(status >= 0) {
	        	msg = MessageQ_alloc(0, sizeof(MessageQ_MsgHeader));
	        	if (msg == NULL) {
	        		System_abort("MessageQ_alloc failed\n" );
	        	}
	        	MessageQ_setMsgId(msg, 2);
	        	/* send the message to the remote processor */
	        	status = MessageQ_put(remoteQueueId, msg);
	        	dspConnectionCheckDone = 0;
	        	System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nConnection test \r\n");
	        } else {
	        	System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nNo DSP Message Queue found \r\n");
	        }
		} else if (par1[0] == 'l') {
			i2cgen_test(0);
			System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nLED on logic board on\r\n");
		} else if (par1[0] == 'L') {
			i2cgen_test(3);
			System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nLED on logic board off\r\n");
		} else {
			spiTestAcive = 0;
			System_sprintf((xdc_Char *)pcWriteBuffer, "\r\nspitest is %d \r\n", spiTestAcive);
		}
	}

	return pdFALSE;
}

extern PHY_radio_test();
#define testloop 1
void maintenanceMainTask(void) {

    MessageQ_Msg     msg;
    //MessageQ_Handle  messageQ;
    MessageQ_Handle  maintMessageQ;
    //MessageQ_QueueId remoteQueueId;
    int status;
    static int dspConnectionCheckCnt = 0;

    i2cgen_init();
    PHY_radio_test();
	System_printf("Maintenance Task started\n");
	System_flush();
#if testloop
	Uint8 stro[20] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T'};
	volatile Uint8 stri[20];
	int cnt = 0;
	int i;
	xdc_runtime_Error_Block eb;
	if(!Cache_wbInv((xdc_Ptr)stro, 20, TRUE, &eb)) while(1);
	Uint8 timeCnt = 0;
#endif

	/* Register maintenance command(s) */
	{
		const CLI_Command_Definition_t cmd = {
				(  int8_t *  )"maint",
				(  int8_t *  ) "maint 0|1 to run spitest\r\n"
				               "maint c to make a DSP connection control\r\n"
							   "maint l led on logic board",
				maintenanceCmdCbk,
				1
		};

		FreeRTOS_CLIRegisterCommand( &cmd );
	}
	*FreeRTOS_CLIGetOutputBuffer() ='\0';
	TRACE("Maintenance Task Started\r\n");

	/* Install radio irq test */
#if 0
	dio_subscribeIO(DIO_RADIO_IRQ1, radioIRQ);
	dio_subscribeIO(DIO_RADIO_IRQ2, radioIRQ2);
#endif

	/* Register a queue */
	/*
	messageQ = MessageQ_create("MaintenanceQueue", NULL);
	if (messageQ == NULL) {
	        System_abort("MessageQ_create failed\n" );
	}
*/
	/* Register maintenance queue */
	maintMessageQ = MessageQ_create(TRACE_QUEUE_NAME, NULL);
	if (maintMessageQ == NULL) {
	        System_abort("Maintenance MessageQ_create failed\n" );
	}

	/* Handle CLI and stuff, main loop*/
	while(1) {
		if(uartNewString) {
			uartNewString = 0;
			while(FreeRTOS_CLIProcessCommand(( int8_t * ) inBuffer, FreeRTOS_CLIGetOutputBuffer(), configCOMMAND_INT_MAX_OUTPUT_SIZE) != pdFALSE)
			{
				trace((char *)FreeRTOS_CLIGetOutputBuffer());
			}
			trace((char *)FreeRTOS_CLIGetOutputBuffer());
		}

#if testloop
		if(spiTestAcive){
			if(timeCnt++>=20) {
			timeCnt = 0;
			cnt++;
			for(i=0; i<10; i++) stri[i]=i;
			radioDone = 0;
			radio_exchangeData(18,(UInt8 *)stro, (UInt8 *)stri, &setRadioDone);
			while(!radioDone);
			if(!Cache_inv((xdc_Ptr)stri, 20, TRUE, &eb)) while(1);
			TRACE5("Radiodata 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", stri[0],stri[1],stri[2],stri[3],stri[4]);
			System_printf("Task client: SPI Write Done\n");
			System_flush();
			}
		}
#endif

		/* DSP connection check */
		/* Just send a signal to DSP to see its alive */
		if(!dspConnectionCheckDone) {
			if(dspConnectionCheckCnt++ >= 4) {
				TRACE("DSP dead??\r\n");
				dspConnectionCheckCnt = 0;
				dspConnectionCheckDone = 1;
			}
		}

		/* Handle Signals from DSP, also control main loop idel speed */
		status = MessageQ_get(maintMessageQ, &msg, 200000);
		if (status == MessageQ_S_SUCCESS) {
			/* Handle trace message if we got one*/
			if (MessageQ_getMsgId(msg) == TRACE_MESSAGE_ID) {
				trace(((struct traceMessage *) msg)->traceStr);
				MessageQ_free(msg);
			/* Handle DSP alive check */
			} else if (MessageQ_getMsgId(msg) == 3) {
				if (dspConnectionCheckDone) { TRACE1("Unexp signal ID %d\r\n", MessageQ_getMsgId(msg));}
				else {
					dspConnectionCheckDone = 1;
					TRACE("DSP replyed on connection test\r\n");
					MessageQ_free(msg);
				}
			} else {
				TRACE1("Unknown signal ID %d\r\n", MessageQ_getMsgId(msg));
			}
		}
	}
}
