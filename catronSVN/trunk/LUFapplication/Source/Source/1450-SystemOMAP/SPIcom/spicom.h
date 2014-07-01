/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : spicom.h
* Author            : Roger Vilmunen ÅF
* Description       : SPI interface
*                   :
********************************************************************************
* Revision History
* 2013-04-08        : First Issue
*******************************************************************************/

#include <xdc/std.h>
#include <ti/ipc/MessageQ.h>

/* Units */
#define SPICOM_RADIO_UNIT     1
#define SPICOM_PT_HSET_UNIT   2

/* SPICOM return codes */
#define SPICOM_OK                  (0)
#define SPICOM_BUSY               (-1)
#define SPICOM_NOT_INIT           (-2)
#define SPICOM_UNIT_NOT_SUPPORTED (-3)

/******************************************************************************
* Function Name    : spicom_init
* Function Purpose : Initialize the spi function
*                  : This must be called before any other spi functions/macros
* Input parameters : none
* Return value     : 0 = ok
******************************************************************************/
Int8 spicom_init(void);

/******************************************************************************
* Function Name    : spi_exchangeData
* Function Purpose : Exchange data with a spi unit.
*                  : The function is blocked until a busy SPI is released.
* Input parameters : spiUnit: Predefined Unit
 *                   length:  Number of bytes to write and read
 *                   outData: Pointer to data to send,
 *              		      Can be a NULL pointer.
 *                   inData:  Pointer to memory space to store read data,
 *                            Can be a NULL pointer.
 *                   cbDone:  Callback function. Called when transfer is done.
 *                            Can be a NULL pointer.
* Return value     : return code
******************************************************************************/
Int8 spi_exchangeData(Uint8  spiUnit,
		              Uint16 length,
                      Uint8 *outData,
                      volatile UInt8 *inData,
		              void (*cbDone)(Int8 returnCode));



/* TEST */
Int8 radio_exchangeData(Uint16 length, Uint8 *outData, volatile UInt8 *inData,
		void (*cbDone)(Int8 returnCode));
