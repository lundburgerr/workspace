#include "spicom.h"
#include "trace.h"
#include "CLI.h"
#include "gpio.h"
#include "adf7242.h"
#include "spi_driver.h"

extern  uint8_t getStatus();
extern  uint8_t SPI_GetRegister(uint16_t reg);
extern  uint8_t SPI_SetRegister(uint16_t  addr,uint8_t  data);
extern  uint8_t sendCmd(uint8_t cmd);
extern  void fluschReciveRing();
extern uint8_t SPI_TXwrite(uint8_t *dataBuf,uint8_t length);
extern uint8_t SPI_RXread(uint8_t readBuf[],uint8_t readLength);

extern uint8_t reciveflag;
extern uint8_t TXdone;

uint8_t SetTXBuf();

uint8_t TestSetIdle(void);
uint8_t TestGetIRQ(void);


uint8_t TestGetStatus()
{
    uint8_t statusByte;

    statusByte=getStatus();

    TRACE1("Test Status: %x \n\r", statusByte );

    return 0;
}


uint8_t TestSetRadioReady()
{
    uint8_t RRCmd=0xB3;
    sendCmd(RRCmd);
    return 0;
}
uint8_t TestSetRadioRX()
{
    uint8_t RRCmd=0xB4;
    sendCmd(RRCmd);
    return 0;
}



uint8_t SetTXBuf()
{
    uint8_t buf[]= {0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa};

    SPI_TXwrite(buf,10);
    return 0;
}

uint8_t Send()
{
    uint8_t TXCmd=0xB5;
    sendCmd(TXCmd);
    return 0;
}

uint8_t TestClrIRQ()
{
    uint8_t RRC;
    fluschReciveRing();
    RRC=SPI_GetRegister(REG_IRQ1_SRC0);

    TRACE1("\n\r IRQ: %x \n\r", RRC );
    RRC=SPI_GetRegister(REG_IRQ1_SRC1);

    TRACE1("\n\r IRQ: %x \n\r", RRC );
    SPI_SetRegister(REG_IRQ1_SRC0,0xff);
    SPI_SetRegister(REG_IRQ1_SRC1,0xff);

    RRC=SPI_GetRegister(REG_IRQ1_SRC0);
    TRACE1("\n\r IRQ: %x \n\r", RRC );
    RRC=SPI_GetRegister(REG_IRQ1_SRC1);
    TRACE1("\n\r IRQ: %x \n\r", RRC );

    return 0;
}


uint8_t TestWriteReg()
{
    uint8_t RRC;
    fluschReciveRing();
    RRC=SPI_GetRegister(REG_CCA1);

    TRACE1("Timer: %x \n\r", RRC );

    SPI_SetRegister(REG_CCA1,RRC+5);

    RRC=SPI_GetRegister(REG_CCA1);
    TRACE1("Timer: %x \n\r", RRC );


    return 0;
}



uint8_t TestGetRXbuf()
{
    uint8_t readLength=20;
    uint8_t i;
    uint8_t readBuf[20];
    SPI_RXread(readBuf,readLength);

    for(i=0; i<readLength; i++) {
    	  TRACE1("%x \n\r", readBuf[i] );
    }


    return 0;
}


uint8_t waitRXIRQ()
{
//clr int
    uint8_t enabled1;

    uint8_t readBuf[11];
    uint8_t readLength=10;
    uint8_t IRQmask=0x02;

    uint16_t i;
    uint32_t timeout=300000;

    SPI_SetRegister(REG_IRQ1_SRC0,0xff);
    SPI_SetRegister(REG_IRQ1_SRC1,0xff);
//enable RX int
    enabled1=SPI_GetRegister(REG_IRQ1_EN1);
    reciveflag=0;
    SPI_SetRegister(REG_IRQ1_EN1,(enabled1|IRQmask));
//read status

    while((reciveflag==0)&&(timeout>0)) {
     //   wait(0.0001);
        timeout--;


    }
    if(timeout>0) {
        SPI_RXread(readBuf,readLength);
        for(i=0; i<readLength; i++) {
        	  TRACE1("%x \n\r", readBuf[i] );
        }
    } else {
    	  TRACE("Time out \n\r" );
    }


//wait
//decrease timeout counter
//read status
//if IRQ
//readout RX
//else print time out
//disable IRQ
//clear IRQ
/*    SPI_SetRegister(REG_IRQ1_EN1,(enabled1&(~IRQmask)));
    SPI_SetRegister(REG_IRQ1_SRC0,0xff);
    SPI_SetRegister(REG_IRQ1_SRC1,0xff);*/


    return 0;
}




uint8_t TestGetIRQ(void)
{
    uint8_t RRC;
    RRC=SPI_GetRegister(REG_IRQ1_SRC0);

    TRACE1("\n\r IRQ0: %x \n\r", RRC );
    RRC=SPI_GetRegister(REG_IRQ1_SRC1);
    TRACE1("\n\r IRQ1: %x \n\r", RRC );
 return 0;
}

uint8_t TestSetIdle(void)
{
 uint8_t TXCmd=0xB2;
    sendCmd(TXCmd);
    return 0;
}


uint8_t TestLoadCRC(void)
{

    uint8_t buf[]= {0x30,0x31,0x32,0x33};

    SPI_TXwrite(buf,4);
    return 0;
}


uint8_t TestContiniousTX(void)
{
uint32_t sendInt=1000;
do
    {
    TestSetRadioRX();
    Send();
    while(getStatus()!=CMD_RC_TX);
    //waitTXIRQ();
    sendInt--;
    }
 while(sendInt>0);
return 0;
}



uint8_t waitTXIRQ()
{
//clr int
    uint8_t enabled1;



    uint8_t IRQmask=0x02; //todo check IRQ for TXsend


    uint32_t timeout=300000;

    SPI_SetRegister(REG_IRQ1_SRC0,0xff);
    SPI_SetRegister(REG_IRQ1_SRC1,0xff);
//enable RX int
    enabled1=SPI_GetRegister(REG_IRQ1_EN1);
    TXdone=0;
    SPI_SetRegister(REG_IRQ1_EN1,(enabled1|IRQmask));
//read status

    while((TXdone==0)&&(timeout>0)) {
    //    wait(0.0001);
        timeout--;


    }

    SPI_SetRegister(REG_IRQ1_EN1,(enabled1&(~IRQmask)));
    SPI_SetRegister(REG_IRQ1_SRC0,0xff);
    SPI_SetRegister(REG_IRQ1_SRC1,0xff);


    return 0;
}




