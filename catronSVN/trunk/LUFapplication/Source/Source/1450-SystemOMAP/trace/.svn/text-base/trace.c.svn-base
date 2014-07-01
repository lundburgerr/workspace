/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : trace.c
* Author            : Roger Vilmunen ÅF
* Description       : Trace functions. Traces is string messages that are stored
*                   : in memory or flushed out over a bus without locking the client.
*                   :
********************************************************************************
* Revision History
* 2013-04-03        : First Issue
*******************************************************************************/
#include "psc.h"
#include "uart.h"
#include "edma.h"
#include "string.h"
#include "hw_types.h"
#include "uartStdio.h"
#include "evmOMAPL138.h"
#include "interrupt.h"
#include "edma_event.h"
#include "soc_OMAPL138.h"
#include "hw_psc_OMAPL138.h"

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include "../dma/dma.h"

/****************************************************************************/
/*                      INTERNAL MACRO DEFINITIONS                          */
/****************************************************************************/
#define UART_RBR_THR_REG           ((0x01D0D000u) + (0u))

#define MAX_ACNT                   1
#define MAX_CCNT                   1

#define RX_BUFFER_SIZE             20

/* EDMA3 Event queue number. */
#define EVT_QUEUE_NUM	           0

#ifdef DSPCORE
/* DSP core */
#define TRACE_EDMA3_CHA_RX EDMA3_CHA_UART2_RX
#define TRACE_EDMA3_CHA_TX EDMA3_CHA_UART2_TX
#else
/* Main core */
#define TRACE_EDMA3_CHA_RX EDMA3_CHA_UART2_RX
#define TRACE_EDMA3_CHA_TX EDMA3_CHA_UART2_TX
#endif





/****************************************************************************/
/*                      LOCAL FUNCTION PROTOTYPES                           */
/****************************************************************************/
static void UartTransmitData(unsigned int tccNum, unsigned int chNum,
                             volatile char *buffer, unsigned int buffLength);

/* Callback Function Declaration*/
//void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);

/*static void UartReceiveData(unsigned int tccNum, unsigned int chNum,
                            volatile char *buffer);*/
static void callback(unsigned int tccNum, unsigned int status);
//static void ConfigureIntEDMA3(void);
void Edma3ComplHandlerTraceIsr(void);
void Edma3CCErrHandlerTraceIsr(void);
//static void EDMA3Initialize(void);
/*static void SetupInt(void);*/

/* Defined in uart.c */
extern void uartStart(void);

/****************************************************************************/
/*                      GLOBAL VARIABLES                                    */
/****************************************************************************/
char fstr[200];

volatile unsigned int flag = 0;

#define TRACE_BUFFER_LENGTH (1000)
UInt8 traceBuffer[TRACE_BUFFER_LENGTH]; /* ringbuffer */
UInt8 *bufferEnd  = (UInt8 *) traceBuffer; /* Points to the next free byte in buffer */
UInt8 *bufferBeg  = (UInt8 *) traceBuffer;   /* Points to first byte in buffer*/
UInt8 *bufferNewBeg = 0;          /* New bufferBeg after transfer */
Bool  traceFlushInProgress = false;

static int initDone = 0;
/****************************************************************************/
/*                   LOCAL FUNCTION DEFINITIONS                             */
/****************************************************************************/

/*
** Init trace.
*/

int trace_init(void)
{

    /* Run init once (not very good protection but ok) */
    if(initDone) return 0;
    else initDone = 1;

    /* Init uart if trace is using it*/
    uartStart();

    /* Initialize EDMA3 Controller */
    EDMA3Initialize();


    /* Initialize UART */
    //UARTStdioInit();

    /* Request DMA Channel and TCC for UART Transmit*/
    EDMA3RequestChannel(SOC_EDMA30CC_0_REGS, EDMA3_CHANNEL_TYPE_DMA,
                        TRACE_EDMA3_CHA_TX, TRACE_EDMA3_CHA_TX,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for TX*/
    cb_Fxn[TRACE_EDMA3_CHA_TX] = &callback;

    /* Request DMA Channel and TCC for UART Receive */
    EDMA3RequestChannel(SOC_EDMA30CC_0_REGS, EDMA3_CHANNEL_TYPE_DMA,
                        TRACE_EDMA3_CHA_RX, TRACE_EDMA3_CHA_RX,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for RX*/
    cb_Fxn[TRACE_EDMA3_CHA_RX] = &callback;

    /* Release semaphore */
    Semaphore_post(traceSem);

    return 0;

#if 0
    /* Used for bCnt */
    buffLength = strlen((const char *) enter);

    /* Transmit Data for Enter Message */
    UartTransmitData(EDMA3_CHA_UART2_TX, EDMA3_CHA_UART2_TX,
                     enter, buffLength);

    /* Enabling UART in DMA Mode*/
    UARTDMAEnable(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1 |  \
                                   UART_DMAMODE |          \
                                   UART_FIFO_MODE );

    /* Wait for control to return from call-back function */
    while(0 == flag);
    flag = 0;

    /* Receive Data for Input */
    UartReceiveData(EDMA3_CHA_UART2_RX, EDMA3_CHA_UART2_RX, buffer);


    /* Enabling UART in DMA Mode*/
    UARTDMAEnable(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1 | \
                                   UART_DMAMODE |         \
                                   UART_FIFO_MODE );

    /* Wait for return from callback */
    while(0 == flag);
    flag = 0;

    /* Transmit Data for Entered value */
    UartTransmitData(EDMA3_CHA_UART2_TX, EDMA3_CHA_UART2_TX,
                     buffer, RX_BUFFER_SIZE);

    /* Enabling UART in DMA Mode*/
    UARTDMAEnable(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1 |  \
                                   UART_DMAMODE |           \
                                   UART_FIFO_MODE );

    /* Wait for return from callback */
    while(0 == flag);
    flag = 0;

    /* Free EDMA3 Channels for TX and RX */
    EDMA3FreeChannel(SOC_EDMA30CC_0_REGS, EDMA3_CHANNEL_TYPE_DMA,
                     EDMA3_CHA_UART2_TX, EDMA3_TRIG_MODE_EVENT,
                     EDMA3_CHA_UART2_TX, EVT_QUEUE_NUM);

    EDMA3FreeChannel(SOC_EDMA30CC_0_REGS, EDMA3_CHANNEL_TYPE_DMA,
                     EDMA3_CHA_UART2_RX, EDMA3_TRIG_MODE_EVENT,
                     EDMA3_CHA_UART2_RX, EVT_QUEUE_NUM);

    while(1);
#endif

}

/*
** This function is used to set the PaRAM entries in EDMA3 for the Transmit Channel
** of UART. EDMA3 Enable Transfer is also called within this API.
*/

static void UartTransmitData(unsigned int tccNum, unsigned int chNum,
                             volatile char *buffer, unsigned int buffLength)
{
    EDMA3CCPaRAMEntry paramSet;

    /* Fill the PaRAM Set with transfer specific information */
    paramSet.srcAddr = (unsigned int) buffer;
    paramSet.destAddr = UART_RBR_THR_REG;
    paramSet.aCnt = MAX_ACNT;
    paramSet.bCnt = (unsigned short) buffLength;
    paramSet.cCnt = MAX_CCNT;

    /* The src index should increment for every byte being transferred. */
    paramSet.srcBIdx = (short) 1u;

    /* The dst index should not be increment since it is a h/w register*/
    paramSet.destBIdx = (short) 0u;

    /* A sync Transfer Mode */
    paramSet.srcCIdx = (short) 0u;
    paramSet.destCIdx = (short) 0u;
    paramSet.linkAddr = (unsigned short)0xFFFFu;
    paramSet.bCntReload = (unsigned short)0u;
    paramSet.opt = 0x00000000u;
    paramSet.opt |= (EDMA3CC_OPT_DAM );
    paramSet.opt |= ((tccNum << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);
    paramSet.opt |= (1 << EDMA3CC_OPT_TCINTEN_SHIFT);

    /* Now write the PaRAM Set */
    EDMA3SetPaRAM(SOC_EDMA30CC_0_REGS, chNum, &paramSet);

    /* Enable EDMA Transfer */
    EDMA3EnableTransfer(SOC_EDMA30CC_0_REGS, chNum, EDMA3_TRIG_MODE_EVENT);
}

/*
** This function is used to set the PARAM SET of EDMA3 for the Receive Channel
** of UART. EDMA3 Enable Transfer is also called within this API.
*/
#if 0
static void UartReceiveData(unsigned int tccNum, unsigned int chNum,
                            volatile char *buffer)
{
    EDMA3CCPaRAMEntry paramSet;

    /* Fill the PaRAM Set with transfer specific information */
    paramSet.srcAddr = UART_RBR_THR_REG;
    paramSet.destAddr = (unsigned int) buffer;
    paramSet.aCnt = MAX_ACNT;
    paramSet.bCnt = RX_BUFFER_SIZE;
    paramSet.cCnt = MAX_CCNT;

    /* The src index should not be increment since it is a h/w register*/
    paramSet.srcBIdx = 0;
    /* The dest index should incremented for every byte */
    paramSet.destBIdx = 1;

    /* A sync Transfer Mode */
    paramSet.srcCIdx = 0;
    paramSet.destCIdx = 0;
    paramSet.linkAddr = (unsigned short)0xFFFFu;
    paramSet.bCntReload = 0;
    paramSet.opt = 0x00000000u;
    paramSet.opt |= ((EDMA3CC_OPT_SAM) << EDMA3CC_OPT_SAM_SHIFT);
    paramSet.opt |= ((tccNum << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);
    paramSet.opt |= (1 << EDMA3CC_OPT_TCINTEN_SHIFT);

    /* Now write the PaRAM Set */
    EDMA3SetPaRAM(SOC_EDMA30CC_0_REGS, chNum, &paramSet);

    /* Enable EDMA Transfer */
    EDMA3EnableTransfer(SOC_EDMA30CC_0_REGS, chNum, EDMA3_TRIG_MODE_EVENT);
}
#endif

/* Function used to Initialize EDMA3 */
#if 0
static void EDMA3Initialize(void)
{
    /* Enabling the PSC for EDMA3CC_0.*/
#if 1
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON,
		     PSC_MDCTL_NEXT_ENABLE);

    /* Enabling the PSC for EDMA3TC_0.*/
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON,
		     PSC_MDCTL_NEXT_ENABLE);

    /* Initialization of EDMA3 */
    EDMA3Init(SOC_EDMA30CC_0_REGS, EVT_QUEUE_NUM);

    /*
    ** Enable AINTC to handle interuppts. Also enable IRQ interuppt in
    ** ARM processor.
    */
    //SetupInt();
#endif

    /* Register EDMA3 Interrupts */
    ConfigureIntEDMA3();

}
#endif


/* Function used to Setup ARM Interuppt Controller */
#if 0
static void SetupInt(void)
{
#ifdef _TMS320C6X
	// Initialize the DSP Interrupt controller
	IntDSPINTCInit();

	// Enable DSP interrupts globally
	IntGlobalEnable();
#else
    /*Initialize the ARM Interrupt Controller(AINTC). */
    IntAINTCInit();

    /* Enable IRQ in CPSR.*/
    IntMasterIRQEnable();

    /* Enable the interrupts in GER of AINTC.*/
    IntGlobalEnable();

    /* Enable the interrupts in HIER of AINTC.*/
    IntIRQEnable();
#endif
}
#endif

/* EDMA3 Completion Handler */
#if 0
static void edma3ComplHandler(unsigned int baseAdd, unsigned int regionNum)
{
    volatile unsigned int pendingIrqs;
    volatile unsigned int isIPR = 0;

    unsigned int indexl;
    unsigned int Cnt = 0;
    indexl = 1;

#ifdef _TMS320C6X
    IntEventClear(SYS_INT_EDMA3_0_CC0_INT1);
#else
    IntSystemStatusClear(SYS_INT_CCINT0);
#endif
    isIPR = HWREG(baseAdd + EDMA3CC_S_IPR(regionNum));
    if(isIPR)
        while ((Cnt < EDMA3CC_COMPL_HANDLER_RETRY_COUNT)&& (indexl != 0u))
        {
            indexl = 0u;
            pendingIrqs = HWREG(baseAdd + EDMA3CC_S_IPR(regionNum));
            while (pendingIrqs)
            {
                if((pendingIrqs & 1u) == TRUE)
                {
                    /**
                    * If the user has not given any callback function
                    * while requesting the TCC, its TCC specific bit
                    * in the IPR register will NOT be cleared.
                    */
                    /* here write to ICR to clear the corresponding IPR bits */
                    HWREG(baseAdd + EDMA3CC_S_ICR(regionNum)) = (1u << indexl);

                    (*cb_Fxn[indexl])(indexl, EDMA3_XFER_COMPLETE);
                }
                ++indexl;
                pendingIrqs >>= 1u;
            }
            Cnt++;
        }
}
#endif

#if 0
void Edma3ComplHandlerTraceIsr(void)
{
#ifdef _TMS320C6X
	// Invoke Completion Handler ISR
	edma3ComplHandler(SOC_EDMA30CC_0_REGS, 1);
#else
    /* Invoke Completion Handler ISR */
    edma3ComplHandler(SOC_EDMA30CC_0_REGS, 0);
#endif
}
#endif



/* EDMA3 Error Handler */
#if 0
static void edma3CCErrHandler(unsigned int baseAdd)
{
    volatile unsigned int pendingIrqs = 0;
    unsigned int regionNum = 0;
    unsigned int evtqueNum = 0;
    unsigned int index = 1;
    unsigned int Cnt = 0;

#ifdef _TMS320C6X
    IntEventClear(SYS_INT_EDMA3_0_CC0_ERRINT);
#else
    IntSystemStatusClear(SYS_INT_CCERRINT);
#endif

    if((HWREG(baseAdd + EDMA3CC_EMR) != 0 ) || \
       (HWREG(baseAdd + EDMA3CC_QEMR) != 0) || \
       (HWREG(baseAdd + EDMA3CC_CCERR) != 0))
    {
        /* Loop for EDMA3CC_ERR_HANDLER_RETRY_COUNT number of time, breaks
           when no pending interrupt is found */
        while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT) && (index != 0u))
        {
            index = 0u;
            pendingIrqs = HWREG(baseAdd + EDMA3CC_EMR);
            while (pendingIrqs)
            {
                /*Process all the pending interrupts*/
                if((pendingIrqs & 1u)==TRUE)
                {
                    /* Write to EMCR to clear the corresponding EMR bits.*/
                    HWREG(baseAdd + EDMA3CC_EMCR) = (1u<<index);
                    /*Clear any SER*/
                    HWREG(baseAdd + EDMA3CC_S_SECR(regionNum)) = (1u<<index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }
            index = 0u;
            pendingIrqs = HWREG(baseAdd + EDMA3CC_QEMR);
            while (pendingIrqs)
            {
                /*Process all the pending interrupts*/
                if((pendingIrqs & 1u)==TRUE)
                {
                    /* Here write to QEMCR to clear the corresponding QEMR bits*/
                    HWREG(baseAdd + EDMA3CC_QEMCR) = (1u<<index);
                    /*Clear any QSER*/
                    HWREG(baseAdd + EDMA3CC_S_QSECR(0)) = (1u<<index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }
            index = 0u;
            pendingIrqs = HWREG(baseAdd + EDMA3CC_CCERR);
    if (pendingIrqs != 0u)
    {
        /* Process all the pending CC error interrupts. */
        /* Queue threshold error for different event queues.*/
        for (evtqueNum = 0u; evtqueNum < EDMA3_0_NUM_EVTQUE; evtqueNum++)
        {
            if((pendingIrqs & (1u << evtqueNum)) != 0u)
            {
                /* Clear the error interrupt. */
                HWREG(baseAdd + EDMA3CC_CCERRCLR) = (1u << evtqueNum);
            }
         }

         /* Transfer completion code error. */
         if ((pendingIrqs & (1 << EDMA3CC_CCERR_TCCERR_SHIFT)) != 0u)
         {
             HWREG(baseAdd + EDMA3CC_CCERRCLR) = \
                  (0x01u << EDMA3CC_CCERR_TCCERR_SHIFT);
         }
         ++index;
    }
    Cnt++;
        }
    }
}
#endif

#if 0
void Edma3CCErrHandlerTraceIsr()
{
    /* Invoke CC Error Handler ISR */
    edma3CCErrHandler(SOC_EDMA30CC_0_REGS);
}
#endif

/* Function to register EDMA3 Interuppts */
#if 0
static void ConfigureIntEDMA3(void)
{
    /* Register Interrupts Here */

#ifdef _TMS320C6X
	IntRegister(C674X_MASK_INT4, Edma3ComplHandlerIsr);
	IntRegister(C674X_MASK_INT5, Edma3CCErrHandlerIsr);

	IntEventMap(C674X_MASK_INT4, SYS_INT_EDMA3_0_CC0_INT1);
	IntEventMap(C674X_MASK_INT5, SYS_INT_EDMA3_0_CC0_ERRINT);

	IntEnable(C674X_MASK_INT4);
	IntEnable(C674X_MASK_INT5);
#else
    /* IntRegister(SYS_INT_CCINT0, Edma3ComplHandlerIsr); */

    IntChannelSet(SYS_INT_CCINT0, 2);

    IntSystemEnable(SYS_INT_CCINT0);

    /*IntRegister(SYS_INT_CCERRINT, Edma3CCErrHandlerTraceIsr);*/

    IntChannelSet(SYS_INT_CCERRINT, 2);

    IntSystemEnable(SYS_INT_CCERRINT);
#endif
}
#endif

/*
** This function is used as a callback from EDMA3 Completion Handler.
** UART in DMA Mode is Disabled over here.
*/
static void callback(unsigned int tccNum, unsigned int status)
{
    UARTDMADisable(SOC_UART_2_REGS, (UART_RX_TRIG_LEVEL_1 | UART_FIFO_MODE));
    flag = 1;

    /* Update buffer begin, mark flush finished and try restart flush */
    bufferBeg = bufferNewBeg;
    traceFlushInProgress = false;
    Swi_post(trace_flush_swi_hdl);
}

/*
 * SWI
 *  trace_flush
 *  If DMA not busy, flush data
 *  This code can only be interrupted by hwi
 */
void trace_flush_swi(){

	Uint16 length;
	Uint8 *begptr, *endptr;

	begptr = bufferBeg;
	endptr = bufferEnd;

	/* Skip this if buffer is empty or trace is busy */
	if((begptr != endptr) && (!traceFlushInProgress)) {
		/** Init a transfere **/
		if(endptr > begptr) {
			/* Transfer from beginning of the ring buffer to end of the ring buffer */
			length = endptr - begptr;
			bufferNewBeg = endptr;
		} else {
			/* Transfer from beginning of the ring buffer to the end of the physical buffer */
			length = traceBuffer + TRACE_BUFFER_LENGTH - begptr;
			bufferNewBeg = traceBuffer;
		}
		traceFlushInProgress = true;
		UartTransmitData(TRACE_EDMA3_CHA_TX, TRACE_EDMA3_CHA_TX,
					                     (char *)begptr, length);
		UARTDMAEnable(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1 |  \
					      UART_DMAMODE |          \
			              UART_FIFO_MODE );
	}
}

/*
 * trace
 * Add this to the print queue
 */
void trace(char *str) {

	char *in = str;

	/* Do not run if not init */
	if(!initDone) return;

	/* Only on at the time can fill data into the ringbuffer, wait for your turn */
	Semaphore_pend(traceSem, BIOS_WAIT_FOREVER);

#if 0
	UartTransmitData(EDMA3_CHA_UART2_TX,
			         EDMA3_CHA_UART2_TX,
	                 str,
	                 strlen(str));

	/* Enabling UART in DMA Mode*/
	UARTDMAEnable(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1 |  \
			      UART_DMAMODE |          \
	              UART_FIFO_MODE );
#else
	/* Loop until null found in in data or buffer is full */
	while((bufferEnd+1!= bufferBeg) && !((bufferBeg == (UInt8 *)&traceBuffer) && (bufferEnd ==(UInt8 *)&traceBuffer + TRACE_BUFFER_LENGTH - 1)) && *in) {

		/* copy char into trace buffer*/
		*bufferEnd = *in;

		/* Increment pointers */
		in++;
		bufferEnd++;
		if(bufferEnd >= (UInt8 *) traceBuffer + TRACE_BUFFER_LENGTH) bufferEnd = (UInt8 *) traceBuffer;
	}

	Semaphore_post(traceSem);

	/* send swi to start dma/uart */
	Swi_post(trace_flush_swi_hdl);
#endif
}
