/*
 * spi_driver.h
 *
 *  Created on: 31 jan 2013
 *      Author: ehiblg
 */

#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include "stdint.h"
#include <ti/sysbios/knl/Queue.h>

/*
 ** brief\ Function to write to RF rf chip Tx buffer
 * param\ dataBuf, pointer to vector that contain the tx values
 * param\ length, length of tx data vector.
 *
 */


uint8_t SPI_TXwrite(uint8_t *dataBuf, uint8_t length);

/*
 ** brief\ Function to read from RF rf chip RX buffer
 * param\ dataBuf, pointer to a empty vector RX values will be copied to.
 * param\ length, length of RX data vector.
 *
 */


uint8_t SPI_RXread(uint8_t readBuf[],uint8_t readLength);

uint8_t SPI_readMem(uint8_t* readBuf, uint8_t length);

/*
 ** brief\ Function to initiate RF transmitting
 *
 */

uint8_t SPI_startTX();

uint8_t SPI_startMeas();

/*
 ** brief\ Function to set register content in RF chip
 *
 */

uint8_t SPI_SetRegister(uint16_t reg, uint8_t data);

/*
 ** brief\ Function to get register content in RF chip
 *
 */

uint8_t SPI_GetRegister(uint16_t reg);

/*
 ** brief\ Function to start comand and actions on RF chip
 *
 */
uint8_t sendCmd(uint8_t cmd);


uint8_t getStatus();

#endif /* SPI_DRIVER_H_ */
