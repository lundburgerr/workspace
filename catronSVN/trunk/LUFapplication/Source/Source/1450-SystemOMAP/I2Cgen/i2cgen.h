/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : i2cgen.h
* Author            : Roger Vilmunen ÅF
* Description       : General I2C-interface. Should only be used by other drivers.
*                   :
********************************************************************************
* Revision History
* 2013-06-07        : First Issue
*******************************************************************************/

#include <xdc/std.h>

#define I2C_OK (0)
#define I2C_BUSY (-1)

/******************************************************************************
* Function Name    : i2cgen_init
* Function Purpose : Initialize the i2c function
*                  : This must be called before any other i2c functions
* Input parameters : none
* Return value     : 0 = ok
******************************************************************************/
Int8 i2cgen_init(void);

/******************************************************************************
* Function Name    : i2cgen_com
* Function Purpose : Communication over I2C
*                  : Sends and/or receives (receive not implemented)
* Input parameters : adr: The i2c slave address
* 				   : dataOut: Data to be sent.
* 				              Buffer must be valid until i2c finished.
* 				   : lenOut : Size (bytes) of outgoing data
* 				   : dataIn : Pointer to allocated space for incomming data.
* 				              Buffer must be valid until i2c finished.
* 				   : lenIn  : Size (bytes) of incomming data
* 				   : blockIfBusy : 0=return busy if so, 1=wait until bus is free
* 				   : cbDone : Funtion to be called when I2C finnished. (can be null).
* Return value     : I2C_OK, I2C_BUSY
******************************************************************************/
Int8 i2cgen_com(Uint8 adr, Uint8 *dataOut, Uint16 lenOut, Uint8 *dataIn, Uint16 lenIn,
				Uint8 blockIfBusy, void (*cbDone)(Int8 returnCode));
