/******************** Copyright (c) 2013 Catron Elektronik AB ******************
 * Filename          : spi_driver.c
 * Author            : Björn Lindberg
 * Description       : Driver for RADIO over spi
 ********************************************************************************
 * Revision History
 * 2013-02-01        : First Issue
 *******************************************************************************/

/*-----------------------------------------------------------
 Standard includes
 -------------------------------------------------------------
 */

#include "adf7242.h"
#include "spi_driver.h"
#include "spicom.h"

#include <xdc/runtime/knl/Cache.h>
#include <ti/sysbios/knl/Task.h>

/*
 -----------------------------------------------------------
 Macros
 -------------------------------------------------------------
 */


#define RXRINGBUFLEN 256

#define TXRINGBUFLEN 256
#define radio
/*
 -----------------------------------------------------------
 Static function declarations
 -------------------------------------------------------------
 */
void addTailTX(uint8_t *buf,uint16_t len );
uint8_t recive_nonStatus(uint8_t number);
uint16_t sync_spi();
uint8_t recive(uint8_t recBuf[], uint8_t number);
void fluschReciveRing();

Int8 SPIradioDone =0;
/* Spi test function */
void phySetRadioDone(Int8 status) {
	SPIradioDone = 1;
}

/*
 -----------------------------------------------------------
 Static variables
 -------------------------------------------------------------
 */
uint8_t send_bu[TXRINGBUFLEN]={0};
uint8_t recive_bu[RXRINGBUFLEN]={0};
uint8_t readSwap=0;
uint8_t sendSwap=0;

uint16_t sendRingIn=0;
uint16_t sendRingRead=0;

uint16_t reciveRingIn=0;
uint16_t reciveRingRead=0;

uint8_t radioCommandBuf[5];
uint8_t radioCommandInPtr=0;
uint8_t radioCommandSendPtr=0;


/*-----------------------------------------------------------
 Command declarations
 -------------------------------------------------------------*/


uint8_t getStatus()
{
    uint8_t res=0;
	xdc_runtime_Error_Block eb;
    SPIradioDone=0;
    send_bu[0]=0xff;
    radio_exchangeData(1,(UInt8 *)send_bu,(UInt8 *)recive_bu,&phySetRadioDone);
  	while(!SPIradioDone);
  	if(!Cache_inv((xdc_Ptr)recive_bu, 20, TRUE, &eb)) while(1);
 /*   cs=0;
    res= device.write(0xff);
    cs=1;*/
  	res=recive_bu[0];
    return res;
}


uint8_t SPI_GetRegister(uint16_t reg)
{
    uint8_t   buf_tx[4];

    buf_tx[0] =CMD_SPI_MEMR_RD(reg);
    buf_tx[1] =reg;
    buf_tx[2] =CMD_SPI_NOP;
    buf_tx[3] =CMD_SPI_NOP;

    addTailTX(buf_tx,4);
    sync_spi();
    return  recive_nonStatus(1);


}

uint8_t      SPI_SetRegister(uint16_t  addr,uint8_t  data)
{
    uint8_t   buf_tx[4];



    buf_tx[0] = CMD_SPI_MEMR_WR(addr);
    buf_tx[1] = addr;
    buf_tx[2] = data;
    addTailTX(buf_tx,3);
    sync_spi();
    recive_nonStatus(1);



    return 0;

}



uint8_t SPI_TXwrite(uint8_t *dataBuf,uint8_t length)
{

    uint16_t len=length;//= RXBUFLENGTH;
    uint8_t buf[3];
    buf[0] = CMD_SPI_PKT_WR;
    buf[1] = 0;                                //first position in tx buf need to be zero
    buf[2] = len + 2;
    addTailTX(buf,3);
    addTailTX(dataBuf,len);
    sync_spi();
    recive_nonStatus(1);

    return 0;
}



uint8_t     SPI_RXread(uint8_t readBuf[],uint8_t readLength)
{
    uint32_t i;
    uint8_t buf[4];
    fluschReciveRing();
    buf[0] = CMD_SPI_PKT_RD;
    buf[1] = CMD_SPI_NOP;
    buf[2] = 0;             /* PHR */

    addTailTX(buf,1);
    buf[0] = CMD_SPI_NOP;


    for(i=0; i<=readLength; i++) {
        addTailTX(buf,1);

    }

    sync_spi();


    recive(readBuf,readLength);
    return 0;
}





uint8_t sendCmd(uint8_t cmd)
{
    uint8_t   buf_tx[1];



    buf_tx[0] = cmd;

    addTailTX(buf_tx,1);
    sync_spi();
    recive_nonStatus(1);


    return 0;
}






uint8_t 	SPI_startTX()
{
    sendCmd(CMD_RC_RX);

	return 0;
}
uint8_t 	SPI_startMeas()
{
	return 0;
}





/*----------------------------------------private functions---------------------------------------------------------------------------------------*/


void addTailTX(uint8_t *buf,uint16_t len )
{
    uint32_t i=0;
//todo add mutex



    do {
        send_bu[sendRingIn]=buf[i];
        i++;
        sendRingIn++;
        if(sendRingIn>=TXRINGBUFLEN) {
            sendRingIn=0;
            sendSwap=1;
        }
    } while(i<len);

}


//start and shall be run on spi interrupt context.


uint16_t sync_spi()
{
#ifdef radio
	xdc_runtime_Error_Block eb;
	uint32_t length=sendRingIn-sendRingRead;
//check spi mutex
//    cs = 1;
 //   while((sendRingRead<sendRingIn)||sendSwap==1) {

 //       cs = 0;
    	//todo change length to allow for complete message send
	reciveRingIn=0;
	SPIradioDone=0;
  radio_exchangeData(length,(UInt8 *)send_bu,(UInt8 *)recive_bu,&phySetRadioDone);
	while(!SPIradioDone);
	if(!Cache_inv((xdc_Ptr)recive_bu, 20, TRUE, &eb)) while(1);
  sendRingIn=0;
  sendRingRead=0;
  reciveRingIn=+length;
 //       recive_bu[reciveRingIn]=device.write(send_bu[sendRingRead]);
 //       pc2.printf(" SPI write %x ; read %x ",send_bu[sendRingRead],recive_bu[reciveRingIn]);
 /*       sendRingRead++;

        if(sendRingRead>=TXRINGBUFLEN) {
            sendRingRead=0;
            sendSwap=0;
        }
        if(reciveRingIn>=TXRINGBUFLEN) {
            reciveRingIn=0;
            readSwap=1;
        }*/

//
 //  }
 //   cs = 1;
 //   pc2.printf(" \r\n");
#endif
    return 0;

}

uint8_t recive_nonStatus( uint8_t number)
{

    uint8_t readdata=recive_bu[reciveRingIn-1];

    reciveRingRead=reciveRingIn;


    return  readdata;

}
uint8_t recive(uint8_t recBuf[], uint8_t number)
{
    uint8_t i=0;
//    pc2.printf("\n\r Read %d In %d \n",reciveRingRead,reciveRingIn);
    reciveRingRead=(reciveRingIn -number); //skip old get latest
    while((reciveRingRead<reciveRingIn)||(readSwap==1)) {
        recBuf[i]=recive_bu[reciveRingRead];
        i++;
        reciveRingRead++;
        if(reciveRingRead>=TXRINGBUFLEN) {
            reciveRingRead=0;
            readSwap=0;
        }
    }

    return  reciveRingIn-reciveRingRead;

}

void fluschReciveRing()
{
    reciveRingRead=0;
    reciveRingIn=0;
    readSwap=0;
}


/* non mbed implemnted and proven function

void addRadioCmd(uint8_t cmd )
{

	radioCommandBuf[radioCommandInPtr]=cmd;
	radioCommandInPtr++;
}





uint16_t radiocommand()
{
uint8_t spi_data;
	//check spi mutex
	//set interupt
	SPI_SetRegister(REG_IRQ1_SRC0,0x08);  //clear int
	SPI_SetRegister(REG_IRQ1_EN0,0x08);   //todo investigat if inverser data i.e enable interrupt or enable mask?


	//take spi mutex
	////tx_spi=radioCommandbuf[0]; send first command
	radioCommandSendPtr=1;




	return 0;
}

void nextRadioCommand()
{

	////tx_spi=radioCommandbuf[radioCommandSendPtr]; send  command

	radioCommandSendPtr++;
	if(radioCommandSendPtr>=radioCommandInPtr)
	{
		//maskinterupt
		radioCommandInPtr=0;

	}



}

uint8_t 	SPI_readMem(uint8_t* readBuf, uint8_t length)
{
	uint32_t i;
	for(i=0;i<length;i++)
	{
	readBuf[i]=0xaa;
	}

	return 0;
}
*/



