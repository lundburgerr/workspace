/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : i2cgen.h
* Author            : Roger Vilmunen ÅF
* Description       : General I2C-interface. Should only be used by other drivers.
*                   :
********************************************************************************
* Revision History
* 2013-06-07        : First Issue
*******************************************************************************/

#include "i2c.h"
#include "i2cgen.h"
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <interrupt.h>
#include "soc_OMAPL138.h"
#include <xdc/cfg/global.h>
#include "trace/trace.h"

#include "evmOMAPL138.h"
#include "hw_syscfg0_OMAPL138.h"

UInt8 *dataToSlave;
UInt8 *dataFromSlave;
UInt16 dataLenFromSlave = 0;
void (*cbDoneFx)(Int8 returnCode);

/*****************************************************************************
**                       MACRO DEFINITIONS
*****************************************************************************/
/* Pin Multiplexing bit masks to select I2C0 pins. */
#define PINMUX4_I2C0_SDA_ENABLE    (SYSCFG_PINMUX4_PINMUX4_15_12_I2C0_SDA << \
                                    SYSCFG_PINMUX4_PINMUX4_15_12_SHIFT)

#define PINMUX4_I2C0_SCL_ENABLE    (SYSCFG_PINMUX4_PINMUX4_11_8_I2C0_SCL << \
                                    SYSCFG_PINMUX4_PINMUX4_11_8_SHIFT)


/* Pin Multiplexing bit masks to select I2C1 pins. */
#define PINMUX4_I2C1_SDA_ENABLE    (SYSCFG_PINMUX4_PINMUX4_23_20_I2C1_SDA << \
                                    SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT)

#define PINMUX4_I2C1_SCL_ENABLE    (SYSCFG_PINMUX4_PINMUX4_19_16_I2C1_SCL << \
                                    SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT)


void I2CPinMuxSetup(unsigned int instanceNum);

/*
** Init i2c
**
** setup pin
*/
Int8 i2cgen_init(void) {

	UInt8 initDone = 0;

	Semaphore_pend(i2cSem, BIOS_WAIT_FOREVER);

	if(initDone) {
		Semaphore_post(i2cSem);
		return I2C_OK;
	}

	/* Configure pin setup */
	I2CPinMuxSetup(0);

    /* Put i2c in reset/disabled state */
    I2CMasterDisable(SOC_I2C_0_REGS);

    /* Configure i2c bus speed to 100khz */
    I2CMasterInitExpClk(SOC_I2C_0_REGS, 24000000, 8000000, 100000);

    initDone = 1;
    Semaphore_post(i2cSem);

    return I2C_OK;

}

void i2cgen_comRead() {
    I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_DATA_READY|I2C_INT_STOP_CONDITION);

    /* Set i2c data count register to number of data to be transmit */
    I2CSetDataCount(SOC_I2C_0_REGS, dataLenFromSlave);
    dataLenFromSlave = 0; /* No more data after this */

    /*
    ** Configure i2c has master-transmitter and to generate stop condition
    ** when value in internal data count register count down to zero
    */
    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX | I2C_CFG_STOP);

    /* Bring i2c out of reset */
    I2CMasterEnable(SOC_I2C_0_REGS);

    /* Generate start condition on i2c bus */
    I2CMasterStart(SOC_I2C_0_REGS);

}

Int8 i2cgen_com(Uint8 adr, Uint8 *dataOut, Uint16 lenOut, Uint8 *dataIn, Uint16 lenIn,
				Uint8 blockIfBusy, void (*cbDone)(Int8 returnCode)) {

	/* Check if busy */
	if(blockIfBusy) {
		Semaphore_pend(i2cSem, BIOS_WAIT_FOREVER);
	} else {
		if(!Semaphore_pend(i2cSem, BIOS_NO_WAIT)) {
			return I2C_BUSY;
		}
	}

	if(lenOut == 0 && lenIn == 0) {
		Semaphore_post(i2cSem);
		return I2C_OK;
	}

	/* Remember some params */
	dataToSlave = dataOut;
	dataFromSlave = dataIn;
	dataLenFromSlave = lenIn;
	cbDoneFx = cbDone;

	/* Read not impl, do we need it? */
	if(lenIn) TRACE("Indata not implemented");

    /* Set i2c slave address */
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, adr);

    if(lenOut) {
    /* Enable transmit data ready and stop condition interrupt */
    I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY|I2C_INT_STOP_CONDITION);

    /* Set i2c data count register to number of data to be transmit */
    I2CSetDataCount(SOC_I2C_0_REGS, lenOut);

    /*
    ** Configure i2c has master-transmitter and to generate stop condition
    ** when value in internal data count register count down to zero
    */
    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX | I2C_CFG_STOP);

    /* Bring i2c out of reset */
    I2CMasterEnable(SOC_I2C_0_REGS);

    /* Generate start condition on i2c bus */
    I2CMasterStart(SOC_I2C_0_REGS);
    } else {
    	i2cgen_comRead();
    }
    return I2C_OK;

}

void I2CIsr(void)
{
    unsigned int intCode = 0;

    /* Get interrupt vector code */
    intCode = I2CInterruptVectorGet(SOC_I2C_0_REGS);

    /* Clear status of interrupt */
#ifdef _TMS320C6X
    IntEventClear(SYS_INT_I2C0_INT);
#else
    IntSystemStatusClear(SYS_INT_I2CINT0);
#endif

    if (intCode == I2C_INTCODE_TX_READY)
    {
         /* Put data to data transmit register of i2c */
         I2CMasterDataPut(SOC_I2C_0_REGS, *dataToSlave++);
    }

    if (intCode == I2C_INTCODE_STOP)
    {
    	/* Disable transmit data ready register */
        I2CMasterIntDisableEx(SOC_I2C_0_REGS,I2C_INT_TRANSMIT_READY);

        /* Shall we read */
        if(dataLenFromSlave) {
        	i2cgen_comRead();
        } else {
        	if(cbDoneFx) cbDoneFx(0);
            Semaphore_post(i2cSem);
        }
    }

    if (intCode == I2C_INTCODE_RX_READY)
    {
         /* Put data to data transmit register of i2c */
    	*dataFromSlave++ = I2CMasterDataGet(SOC_I2C_0_REGS);
    }
}

/**
 * \brief   This function selects the I2C pins for use. The I2C pins are
 *          multiplexed with pins of other peripherals in the System on
 *          Chip(SoC).
 *
 * \param   instanceNum   The instance number of the I2C instance to be used.
 *
 * \return  None.
 *
 */
void I2CPinMuxSetup(unsigned int instanceNum) {
	unsigned int savePinMux = 0;

	if (0 == instanceNum) {

		/*
		 ** Clearing the bits in context and retaining the other bit values
		 ** of PINMUX4 register.
		 */
		savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4))
				& ~(SYSCFG_PINMUX4_PINMUX4_15_12 | SYSCFG_PINMUX4_PINMUX4_11_8);

		/* Actual selection of I2C0 peripheral's pins for use. */
		HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) =
				(PINMUX4_I2C0_SDA_ENABLE | PINMUX4_I2C0_SCL_ENABLE | savePinMux);

	}

	else if (1 == instanceNum) {

		/*
		 ** Clearing the bits in context and retaining the other bit values
		 ** of PINMUX4 register.
		 */
		savePinMux =
				HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4))
						& ~(SYSCFG_PINMUX4_PINMUX4_23_20
								| SYSCFG_PINMUX4_PINMUX4_19_16);

		/* Actual selection of I2C1 peripheral's pins for use. */
		HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) =
				(PINMUX4_I2C1_SDA_ENABLE | PINMUX4_I2C1_SCL_ENABLE | savePinMux);

	}
}

void testCb(Int8 x) {
	return;
}

i2cgen_test(Int8 led) {
	UInt8 datac[2] = {0x06, 0x3f}; // cfg
	UInt8 datas[2] = {0x02, 0x00}; // cfg
	datas[1] = led<<6;
	i2cgen_com(0x21, datac, 2, 0, 0, 1, testCb);
	i2cgen_com(0x21, datas, 2, 0, 0, 1, 0);
	i2cgen_com(0x21, datas, 0, 0, 0, 1, 0);
	datac[0] = 0xAB;
	datas[0] = 0x01;
	i2cgen_com(0x21, datas, 1, datac, 1, 1, 0);
	i2cgen_com(0x21, datas, 0, 0, 0, 1, 0);
	TRACE1("I2C-Read:0x%x\r\n",datac[0]);
}
