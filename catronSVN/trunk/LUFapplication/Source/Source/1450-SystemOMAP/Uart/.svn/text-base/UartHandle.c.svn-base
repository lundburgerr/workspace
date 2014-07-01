
#include "hw_psc_OMAPL138.h"
#include "soc_OMAPL138.h"
#include "interrupt.h"
#include "evmOMAPL138.h"
#include "hw_types.h"
#include "uart.h"
#include "psc.h"
#include "../CLI/CLI.h"
#include "../trace/trace.h"

//char txArray[] = "StarterWare UART echo application\n\r";
#define inBufferSize (100)
char inBuffer[inBufferSize];
int uartNewString = 0;

#if 1
void UARTIsr()
{
    //static unsigned int length = sizeof(txArray);
    //static unsigned int count = 0;
    char rxData = 0;
    unsigned int int_id = 0;
    static char *inBufferPtr = inBuffer;
    static char traceBuf[2] = {'\0', '\0'};

    do {
    /* This determines the cause of UART2 interrupt.*/
    int_id = UARTIntStatus(SOC_UART_2_REGS);

#ifdef _TMS320C6X
    // Clear UART2 system interrupt in DSPINTC
    IntEventClear(SYS_INT_UART2_INT);
#else
    /* Clears the system interupt status of UART2 in AINTC. */
    IntSystemStatusClear(SYS_INT_UARTINT2);
#endif

#if 0
    /* Checked if the cause is transmitter empty condition.*/
    if(UART_INTID_TX_EMPTY == int_id)
    {
        if(0 < length)
        {
            /* Write a byte into the THR if THR is free. */
            UARTCharPutNonBlocking(SOC_UART_2_REGS, txArray[count]);
            length--;
            count++;
        }
        if(0 == length)
        {
            /* Disable the Transmitter interrupt in UART.*/
            UARTIntDisable(SOC_UART_2_REGS, UART_INT_TX_EMPTY);
        }
     }
#endif

    /* Check if the cause is receiver data condition.*/
    if(UART_INTID_RX_DATA == int_id)
    {
			rxData = UARTCharGetNonBlocking(SOC_UART_2_REGS);
			if (uartNewString == 0) {
				if (rxData == '\r') {
					*inBufferPtr = '\0';
					uartNewString = 1;
					inBufferPtr = inBuffer;
					/* Disable the Receiver interrupt in UART.*/
					//UARTIntDisable(SOC_UART_2_REGS, UART_INT_RXDATA_CTI);

				} else {
					*inBufferPtr = rxData;
					inBufferPtr++;
					if (inBufferPtr >= (inBuffer + inBufferSize))
						inBufferPtr--;
					else {
						*traceBuf = rxData;
						trace(traceBuf);
					}
				}
			}
			//UARTCharPutNonBlocking(SOC_UART_2_REGS, rxData);
    }


    /* Check if the cause is receiver line error condition.*/
    if(UART_INTID_RX_LINE_STAT == int_id)
    {
        while(UARTRxErrorGet(SOC_UART_2_REGS))
        {
            /* Read a byte from the RBR if RBR has data.*/
            UARTCharGetNonBlocking(SOC_UART_2_REGS);
        }
    }

    } while (int_id);

    return;
}
#endif

#if 0
static void ConfigureIntUART(void)
{
#ifdef _TMS320C6X
	IntRegister(C674X_MASK_INT4, UARTIsr);
	IntEventMap(C674X_MASK_INT4, SYS_INT_UART2_INT);
	IntEnable(C674X_MASK_INT4);
#else
    /* Registers the UARTIsr in the Interrupt Vector Table of AINTC. */
    IntRegister(SYS_INT_UARTINT2, UARTIsr);

    /* Map the channel number 2 of AINTC to UART2 system interrupt. */
    IntChannelSet(SYS_INT_UARTINT2, 2);

    IntSystemEnable(SYS_INT_UARTINT2);
#endif
}
#endif

#if 0
static void SetupInt(void)
{
#ifdef _TMS320C6X
	// Initialize the DSP INTC
	IntDSPINTCInit();

	// Enable DSP interrupts globally
	IntGlobalEnable();
#else
    /* Initialize the ARM Interrupt Controller(AINTC). */
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

void uartStart(void) {
    unsigned int intFlags = 0;
    unsigned int config = 0;

    /* Enabling the PSC for UART2.*/
    /* roger, crash
	PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART2, PSC_POWERDOMAIN_ALWAYS_ON,
			PSC_MDCTL_NEXT_ENABLE);
			*/

    /* Setup PINMUX */
    UARTPinMuxSetup(2, FALSE);

    /* Enabling the transmitter and receiver*/
    UARTEnable(SOC_UART_2_REGS);

    /* 1 stopbit, 8-bit character, no parity */
    config = UART_WORDL_8BITS;

    /* Configuring the UART parameters*/
    UARTConfigSetExpClk(SOC_UART_2_REGS, SOC_UART_2_MODULE_FREQ,
                        BAUD_115200, config,
                        UART_OVER_SAMP_RATE_16);

    /* Enabling the FIFO and flushing the Tx and Rx FIFOs.*/
    UARTFIFOEnable(SOC_UART_2_REGS);

    /* Setting the UART Receiver Trigger Level*/
    UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1);

    /*
    ** Enable AINTC to handle interrupts. Also enable IRQ interrupt in ARM
    ** processor.
    */
    /* Do int from config, not by starterware SetupInt(); */

    /* Configure AINTC to receive and handle UART interrupts. */
    /* Do int from config, not by starterware ConfigureIntUART(); */

    /* Preparing the 'intFlags' variable to be passed as an argument.*/
    /*
    intFlags |= (UART_INT_LINE_STAT  |  \
                 UART_INT_TX_EMPTY |    \
                 UART_INT_RXDATA_CTI);
	*/
    intFlags |= (UART_INT_LINE_STAT  |  \
                 UART_INT_RXDATA_CTI);
    /* Enable the Interrupts in UART.*/
    UARTIntEnable(SOC_UART_2_REGS, intFlags);
}
