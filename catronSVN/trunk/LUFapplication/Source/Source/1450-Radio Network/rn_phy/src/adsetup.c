/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : adsetup.c
* Author            : Björn Lindberg ÅF
* Description       : initialization of radio cicuit.
********************************************************************************
* Revision History
* 2013-04-09        : First Issue
*******************************************************************************/

/*-----------------------------------------------------------
 Standard includes
 -------------------------------------------------------------
 */


#include "adf7242.h"
#include "stdint.h"

extern uint8_t      SPI_SetRegister(uint16_t  addr,uint8_t  data);
extern void spi_init();

/*-----------------------------------------------------------
 Command declarations
 -------------------------------------------------------------*/



void   setUpAD(void)
{
  //  spi_init();
    SPI_SetRegister(REG_RXFE_CFG,0x16);     // change to 0x06; when using eternal LNA and svitch to non combined antenna

    SPI_SetRegister(REG_RC_CFG,0x04); //not in Lab config
    SPI_SetRegister(REG_GP_CFG,0x00); //not in Lab config
    SPI_SetRegister(REG_SYNC_WORD0,0x31);
    SPI_SetRegister(REG_SYNC_WORD1,0x7F);
    SPI_SetRegister(REG_SYNC_WORD2,0xAA);
//   SPI_SetRegister(REG_SFD_15_4,0xF0);  //ad test prog
    SPI_SetRegister(REG_SYNC_CONFIG,0x10);
    SPI_SetRegister(REG_NUMBER_PREAMBL,0x08);   //same as ADf7242 lab curcuit conside change to 0x07
    SPI_SetRegister(REG_PREAMBL_VALID,0x01);


    SPI_SetRegister(REG_DR0,0x27);   //1000kbs
    SPI_SetRegister(REG_DR1,0x10);
    SPI_SetRegister(REG_TX_FD,25);

    SPI_SetRegister(REG_DM_CFG0,13);
    SPI_SetRegister(REG_DM_CFG1,0x6e);
    SPI_SetRegister(REG_IIRF_CFG,0x05);

    SPI_SetRegister(REG_TX_M,3);

    SPI_SetRegister(REG_RXFE_CFG,0x06);
    SPI_SetRegister(REG_SYNT,0x28); //todo investigate



    SPI_SetRegister(REG_AGC_CFG1,0x34);    //ad test prog
    SPI_SetRegister(REG_AGC_MAX,0x80);   /* RW Slew rate  */
    SPI_SetRegister(REG_AGC_CFG2,0x37);    /* RW RSSI Parameters */
    SPI_SetRegister(REG_AGC_CFG3,42);   /* RW RSSI Parameters */
    SPI_SetRegister(REG_AGC_CFG4,29);    /* RW RSSI Parameters */

    SPI_SetRegister(REG_AGC_CFG6,0x24);    /* RW NDEC Parameters */
    SPI_SetRegister(REG_AGC_CFG7,0x7B);

    SPI_SetRegister(REG_OCL_CFG0,0);
    SPI_SetRegister(REG_OCL_CFG1,3);

    SPI_SetRegister(REG_OCL_BW0,26); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_BW1,25); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_BW2,30); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_BW3,30); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_BW4,30); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_BWS,0); /* RW OCL System Parameters */
    SPI_SetRegister(REG_OCL_CFG13,0xF0);  /* RW OCL System Parameters */



//SPI_SetRegister(REG_AFC_CFG,??); /* RW AFC mode and polarity */
    SPI_SetRegister(REG_AFC_KI_KP,0x99); /* RW AFC ki and kp */
//SPI_SetRegister(REG_AFC_RANGE,??); /* RW AFC range */


// memmory pkt config
    SPI_SetRegister(REG_PKT_CFG,4); // 5 for skip crc validation //not in Lab config
    SPI_SetRegister(REG_TXPB,128);
    SPI_SetRegister(REG_RXPB,0);
    SPI_SetRegister(REG_GP_DRV,0x01);
     SPI_SetRegister(REG_CH_FREQ2,0x03);  //frq to 2450khz
    SPI_SetRegister(REG_CH_FREQ1,0xBD);   //not in Lab config done with window ctrl
    SPI_SetRegister(REG_CH_FREQ0,0x08);

}




