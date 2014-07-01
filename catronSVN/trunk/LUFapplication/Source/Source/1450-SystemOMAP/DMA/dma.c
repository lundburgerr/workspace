/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : dma.c
* Author            : Roger Vilmunen ÅF
* Description       : Handle DMA
*                   :
********************************************************************************
* Revision History
* 2013-04-09        : First Issue
*******************************************************************************/

#include "dma.h"
#include "hw_types.h"
#include "interrupt.h"
#include "soc_OMAPL138.h"
#include "edma.h"

/******************************************************************/
/*                   GLOBAL VARIABLES                             */
/******************************************************************/

static volatile unsigned int flagTx = 0;
static volatile unsigned int flagRx = 0;

static unsigned int evtQ = 0;

/* Callback Function Declaration*/
void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);

static void ConfigureIntEDMA3(void);

/*
 * Init dma
 */
void EDMA3Initialize(void)
{

	static int initDone = 0;

#if 0
    /* Enabling the PSC for EDMA3CC_0).*/
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON,
		     PSC_MDCTL_NEXT_ENABLE);

    /* Enabling the PSC for EDMA3TC_0.*/
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON,
		     PSC_MDCTL_NEXT_ENABLE);
#endif

    /* Do only init once */
    if(initDone) return;
    else initDone = 1;

    /* Initialization of EDMA3 */
    EDMA3Init(SOC_EDMA30CC_0_REGS, evtQ);

    /*
    ** Enable AINTC to handle interuppts. Also enable IRQ interuppt in
    ** ARM processor.
    */
#if 0
    SetupInt();
#endif

    /* Register EDMA3 Interrupts */
    ConfigureIntEDMA3();
}

void ConfigureIntEDMA3(void)
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

void Edma3ComplHandler0Isr(void)
{
#ifdef _TMS320C6X
	// Invoke Completion Handler ISR
	edma3ComplHandler(SOC_EDMA30CC_0_REGS, 1);
#else
    /* Invoke Completion Handler ISR */
    edma3ComplHandler(SOC_EDMA30CC_0_REGS, 0);
#endif
}

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

void Edma3CCErrHandler0Isr()
{
    /* Invoke CC Error Handler ISR */
    edma3CCErrHandler(SOC_EDMA30CC_0_REGS);
}
