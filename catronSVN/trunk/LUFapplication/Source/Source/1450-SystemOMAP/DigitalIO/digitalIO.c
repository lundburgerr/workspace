
//#include "gpio_v2.h"
#include "digitalIO.h"
#include "gpio.h"
#include "soc_OMAPL138.h"
#include <xdc/std.h>
#include "interrupt.h"

#define DIO_RADIO_IRQ2_IO (80)
#define DIO_RADIO_IRQ1_IO (79)

void (*subcribeLst[DIO_RADIO_IRQ2 + 1]) (Uint8 io);

/*
 * dio_init
 * Define GPIO direction
 * Define GPIO IRQ sources
 *
 */
void dio_init(void) {

	Uint8 i;

	/* Init data */
	for(i=0; i < DIO_RADIO_IRQ2; i++);  subcribeLst[i] = 0;

	/* Init GPIO */

	/* Set GPIO interrupt source */
	GPIODirModeSet(SOC_GPIO_0_REGS, DIO_RADIO_IRQ1_IO, GPIO_DIR_INPUT); /*GPIP4[14] */
	GPIODirModeSet(SOC_GPIO_0_REGS, DIO_RADIO_IRQ2_IO, GPIO_DIR_INPUT); /*GPIP4[15] */

	/*
	 ** Configure rising edge and falling edge triggers on pin to generate
	 ** an interrupt
	 */
	GPIOIntTypeSet(SOC_GPIO_0_REGS, DIO_RADIO_IRQ1_IO, GPIO_INT_TYPE_RISEDGE);
	GPIOIntTypeSet(SOC_GPIO_0_REGS, DIO_RADIO_IRQ2_IO, GPIO_INT_TYPE_RISEDGE);

	/* Enable interrupts for Bank 4.*/
	GPIOBankIntEnable(SOC_GPIO_0_REGS, 4);

	/* Interrupt subroutine is defined static */
}

/*
 * dio_IRSB4
 * Interrupt subroutine for Digial IO bank 4
 * Static routine
 */
void dio_IRSB4(void) {

#ifdef _TMS320C6X
    // Clear the system interrupt status in the DSPINTC
    IntEventClear(SYS_INT_GPIO_B4INT);
#else
    /* Clears the system interrupt status of GPIO in AINTC.*/
    IntSystemStatusClear(SYS_INT_GPIOB4);
#endif

    /* Find out what pin, clear status and call callback */
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, DIO_RADIO_IRQ1_IO)) {
    	GPIOPinIntClear(SOC_GPIO_0_REGS, DIO_RADIO_IRQ1_IO);
    	if(subcribeLst[DIO_RADIO_IRQ1]) (subcribeLst[DIO_RADIO_IRQ1])(DIO_RADIO_IRQ1);
    } else if(GPIOPinIntStatus(SOC_GPIO_0_REGS, DIO_RADIO_IRQ2_IO)) {
    	GPIOPinIntClear(SOC_GPIO_0_REGS, DIO_RADIO_IRQ2_IO);
    	if(subcribeLst[DIO_RADIO_IRQ2]) (subcribeLst[DIO_RADIO_IRQ2])(DIO_RADIO_IRQ2);
    }
}

/*
 * dio_IRS
 * Interrupt subroutine for Digial IO
 * Static routine
 */

Uint8 dio_subscribeIO(Uint8 io, void (*callback)(Uint8 io)) {

	/* Check input param */
	if(io < 1 || io > DIO_RADIO_IRQ2) return DIO_BAD_ARG;

	/* Check if already subscirbed */
	if(subcribeLst[io] != 0) return DIO_BUSY;

	/* Store callback */
	subcribeLst[io] = callback;

	/* Still here - ok */
	return DIO_OK;
}
