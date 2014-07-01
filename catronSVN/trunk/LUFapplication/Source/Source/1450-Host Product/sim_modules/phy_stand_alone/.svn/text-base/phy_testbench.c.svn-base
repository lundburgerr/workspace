
#include <xdc/std.h>
#include <stdlib.h>
#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Timer.h>

#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/IHeap.h>

/************************IPC *****************/
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>

#include "rtos_stub.h"
#include "spi_driver.h"
#include "adf7242.h"
#include "rn_phy.h"
#include "slot_timing.h"
#include "spicom.h"
#include "trace.h"
#include "CLI.h"
#include "gpio.h"
#include "soc_OMAPL138.h"
#include "hw_syscfg0_OMAPL138.h"
#include "rn_log.h"

//#include "gpio_v2.h"
#include "hw_types.h"


//extern Swi_Handle swi0;
extern void slotMainThread();

extern void	rx_interupt();
//extern Mailbox_Handle mailbox0;


//String CORE0_MESSAGEQNAME="qEtt";

//extern uint8_t TestLoadCRC(void);



//**********CLI interface*//
extern uint8_t TestGetStatus();
extern uint8_t TestSetRadioReady();
extern uint8_t TestSetRadioRX();
extern uint8_t TestClrIRQ();
extern uint8_t TestWriteReg();
extern uint8_t SetTXBuf();
extern uint8_t Send();
extern uint8_t TestGetRXbuf();
extern uint8_t waitRXIRQ();
extern uint8_t TestSetIdle(void);
extern uint8_t TestGetIRQ(void);
extern uint8_t getRegister(uint16_t reg);
extern uint8_t SPI_SetRegister(uint16_t  addr,uint8_t  data);
extern uint8_t TestContiniousTX(void);



void PHY_radio_test();




static portBASE_TYPE prvDelCommand(int8_t *pcWriteBuffer,
                                   size_t xWriteBufferLen,
                                   const int8_t *pcCommandString );

static portBASE_TYPE prvRegCommand(int8_t *pcWriteBuffer,
                                   size_t xWriteBufferLen,
                                    const int8_t *pcCommandString );

static portBASE_TYPE prvReadCommand(int8_t *pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const int8_t *pcCommandString );
static portBASE_TYPE prvQuitCommand(int8_t *pcWriteBuffer,
                            size_t xWriteBufferLen,
                            const int8_t *pcCommandString );

static portBASE_TYPE prvCRCTXCommand(int8_t *pcWriteBuffer,
                            size_t xWriteBufferLen,
                            const int8_t *pcCommandString );

static portBASE_TYPE prvGetIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );


static portBASE_TYPE prvSetIdle(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );


static portBASE_TYPE prvGetStatus(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );





static portBASE_TYPE prvSetRadioReady(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvSetRadioRX(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );


static portBASE_TYPE prvGetRXbuf(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvClrIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvTXBuf(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );


static portBASE_TYPE prvSend(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvWaitRXIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvContiniousTX(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE prvSlotTiming(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );
static portBASE_TYPE phyInfo(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );

static portBASE_TYPE slotInfo(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString );


static void initcmd();

uint8_t reciveflag;
uint8_t TXdone;




void startInit();

#define HEAP_NAME1   "myHeapBuf1"
#define MSGSIZE 256
#define HEAPID1 1
char textStrBuf[15];

RTOS_QUEUE_ID_T test_tccr_queue_id;

RTOS_QUEUE_ID_T test_uct_rec_queue_id;

RTOS_QUEUE_ID_T test_uct_send_queue_id;

void genToggle(uint8_t channel);
void genMark(uint8_t channel);

void toggleMarkBit(uint8_t port,uint8_t bit,uint8_t n );
void genMarkIO(uint8_t port,uint8_t bit,uint8_t n );

PHY_test_bench()
{

	startInit();
}

void PHY_radio_test()
{
	 setUpAD();
	 initcmd();
}



void startInit() //todo move to stub
{
   /* test code move to stub*/
//	uartStart();
	uint8_t testVal;
/*	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) |= (SYSCFG_PINMUX5_PINMUX5_23_20_SPI1_SIMO0<< SYSCFG_PINMUX5_PINMUX5_23_20_SHIFT);
	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) |= (SYSCFG_PINMUX5_PINMUX5_19_16_SPI1_SOMI0<< SYSCFG_PINMUX5_PINMUX5_19_16_SHIFT);
	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) |= (SYSCFG_PINMUX5_PINMUX5_15_12_NSPI1_ENA<< SYSCFG_PINMUX5_PINMUX5_15_12_SHIFT);
	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) |= (SYSCFG_PINMUX5_PINMUX5_11_8_SPI1_CLK<< SYSCFG_PINMUX5_PINMUX5_11_8_SHIFT);*/

	testVal=getStatus();
	 TRACE1("SPI status %x",testVal);
	 setUpAD();
	 testVal=getStatus();
	 TRACE1("SPI status %x",testVal);
//	rn_phy_master_conf(3);
	  rn_phy_slave_conf(3);
	  rn_phy_module_init();


	/*uartStart();*/
	//	trace_init();
	  testVal= SPI_GetRegister(REG_CCA1);
	  TRACE1("SPI test %x",testVal);
	  SPI_SetRegister(REG_CCA1,0xa5);
	  testVal= SPI_GetRegister(REG_CCA1);
	  TRACE1("SPI test %x",testVal);


/////Slave
	slotMainThread();

	rx_interupt();

	slotMainThread();
///end slave
	 MessageQ_open("test_tccr", &test_tccr_queue_id);
	 MessageQ_open("test_uct_rec", &test_uct_rec_queue_id);
	 MessageQ_open("test_uct_send_queue_id", &test_uct_rec_queue_id);
	rn_phy_register_tccr_message_receiver(test_tccr_queue_id); /*length fixed to buff*/
	rn_phy_register_uct_message_receiver(test_uct_rec_queue_id); /*length fixed to buff*/
	rn_phy_register_uct_message_sender(test_uct_send_queue_id); /*length fixed to buff*/
/*	rn_phy_activate_slot(0, PHY_SLOT_DCC_SEND);
	rn_phy_activate_slot(1, PHY_SLOT_UC_RECEIVE);
	rn_phy_activate_slot(4, PHY_SLOT_UC_RECEIVE);
	rn_phy_activate_slot(5, PHY_SLOT_UC_RECEIVE);
	rn_phy_module_start(0);
/*/
	rn_phy_activate_slot(0,PHY_SLOT_DCC_RECEIVE);
	rn_phy_activate_slot(1, PHY_SLOT_UC_RECEIVE);

	rn_phy_activate_slot(4, PHY_SLOT_UC_SEND);
	rn_phy_activate_slot(5, PHY_SLOT_UC_RECEIVE);

//	rn_phy_activate_slot(5, PHY_SLOT_UC_RECEIVE);

//	rn_phy_activate_slot(7, PHY_SLOT_UC_SEND);

//    Mytimer=Timer_create(0, myIsr, &timerParams,NULL);

}


void genToggle(uint8_t channel)
{
	uint8_t port[8]={4,0,0,0,0,0,0,0};
	uint8_t bit[8]= {14,0,0,0,0,0,0,0};
	toggleMarkBit(port[channel],bit[channel],channel);


}

void genMark(uint8_t channel)
{
	uint8_t port[8]={4,0,0,0,0,0,0,0};
	uint8_t bit[8]= {15,0,0,0,0,0,0,0};
	genMarkIO(port[channel],bit[channel],channel);


}

static uint8_t first=0;

void toggleMarkBit(uint8_t port,uint8_t bit,uint8_t n )
{
static uint8_t value[10];

uint32_t baseAdd=SOC_GPIO_0_REGS;
if(first==0)
	{
	initcmd();
	first++;
	}
if(value[n]==0)
	{
	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) |= (SYSCFG_PINMUX9_PINMUX9_7_4_GPIO4_14 << SYSCFG_PINMUX9_PINMUX9_7_4_SHIFT);

	GPIODirModeSet(SOC_GPIO_0_REGS,79,GPIO_DIR_OUTPUT);// (32*port)+bit+1, GPIO_DIR_OUTPUT);
	//HWREG(baseAdd + 0x0c) = 0x1f;
	//HWREG(baseAdd + 0x60) &= ~(1 << bit);
	}
else
	{

	GPIOPinWrite(SOC_GPIO_0_REGS, 79, value[n] & 0x01);//(32*port)+bit+1, value[n] & 0x01);
/*	if((value[n] & 0x01)==1)
	{
	    HWREG(baseAdd + 0x68) |= (1 << bit);
	}
	else
	{
	    HWREG(baseAdd + 0x6C) |= (1 << bit);
	}*/

	if(value[n]>=0xff)
		{
		value[n]=0;
		}
	}
value[n]++;
}

void genMarkIO(uint8_t port,uint8_t bit,uint8_t n )
{
static bool  isInit[10];
volatile uint16_t delayTime=0x1f;


if(isInit[n]==0)
	{
	 HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) |=(SYSCFG_PINMUX9_PINMUX9_3_0_GPIO4_15 << SYSCFG_PINMUX9_PINMUX9_3_0_SHIFT);


	GPIODirModeSet(SOC_GPIO_0_REGS,80,GPIO_DIR_OUTPUT);//(32*port)+bit+1, GPIO_DIR_OUTPUT);
	isInit[n]++;
	}

GPIOPinWrite(SOC_GPIO_0_REGS,80,0x01); //(32*port)+bit+1, 0x01);

do
{
	delayTime--;
}
while(delayTime>0);
GPIOPinWrite(SOC_GPIO_0_REGS,80,0x00);// (32*port)+bit+1, 0x00);
}



void initcmd()
{
    static const xCommandLineInput Command[] = {
        {(  int8_t *  )"reg",(  int8_t *  )"reg <addr> <data> write data to addr \n\r",prvRegCommand,2},
        {(  int8_t *  )"read",(  int8_t *  )"read <addr> read data from addr \n\r",prvReadCommand,1},
        {(  int8_t *  )"setcrc",(  int8_t *  )"load the tx buffer with a known value \n\r",prvCRCTXCommand,0},
        {(  int8_t *  )"slottime",(  int8_t *  )"enable slot timing printing 0|1 0=disabled \n\r ",prvSlotTiming,1},
        {(  int8_t *  )"getirq",( int8_t * )"get the current IRQ vector \n\r",prvGetIRQ,0},
        {(  int8_t *  )"status",(  int8_t *)"Get the current radio status \n\r",prvGetStatus,0},
        {(  int8_t *  )"idle",(  int8_t *)"Sets the radio to idle state \n\r",prvSetIdle,0},
        {(  int8_t *  )"rr",(  int8_t *)"Set the radio to the radio ready state \n\r",prvSetRadioReady,0},
        {(  int8_t *  )"rrx",(  int8_t *  )"Set the radio to radio RX state \n\r",prvSetRadioRX,0},
        {(  int8_t *  )"getrx",(  int8_t *  )"prints the RX buffer data \n\r",prvGetRXbuf,0},
        {(  int8_t *  )"clrirq",(  int8_t *  )"clears the irq vector \n\r",prvClrIRQ,0,},
        {(  int8_t *  )"txbuf",(  int8_t *  )"Puts data to the TX buffer \n\r",prvTXBuf,0},
        {(  int8_t *  )"send",(  int8_t *  )"Generate the send cmd \n\r",prvSend,0},
        {(  int8_t *  )"waitirq",(  int8_t *  )"wait for receive IRQ \n\r",prvWaitRXIRQ,0},
        {(  int8_t *  )"contitx",(  int8_t *  )"starts continues transmitting \n\r",prvContiniousTX,0},
        {(  int8_t *  )"phyinfo",(  int8_t *  )"writes phy states  \n\r",phyInfo,0},
        {(  int8_t *  )"slotinfo",(  int8_t *  )"writes active slot info  \n\r",slotInfo,0},
        {(  int8_t *  )"0",(  int8_t *  )"0",0,0}
    };
    uint32_t i=0;

    while(Command[i].pxCommandInterpreter!=0) {
        FreeRTOS_CLIRegisterCommand( &Command[i]);
        i++;


    }



}


static portBASE_TYPE prvCRCTXCommand(int8_t *pcWriteBuffer,
                            size_t xWriteBufferLen,
                            const int8_t *pcCommandString )
{
    portBASE_TYPE retVal=0;

//    (void)TestLoadCRC();


    TRACE("\r\n******CRC******* \r\n");
    return retVal;
}




static portBASE_TYPE prvRegCommand(int8_t *pcWriteBuffer,
                            size_t xWriteBufferLen,
                             const int8_t *pcCommandString )
{
    portBASE_TYPE retVal=0;
    int8_t  *pcParameter1, *pcParameter2;
    uint16_t addr, data;
    portBASE_TYPE xParameter1StringLength, xParameter2StringLength;
    (void)xWriteBufferLen;
    /* Obtain the name of the source file, and the length of its name, from
    the command string. The name of the source file is the first parameter. */
    pcParameter1 =(int8_t*)FreeRTOS_CLIGetParameter
                   (
                       /* The command string itself. */
                       pcCommandString,
                       /* Return the first parameter. */
                       1,
                       /* Store the parameter string length. */
                       &xParameter1StringLength
                   );

    /* Obtain the name of the destination file, and the length of its name. */
    pcParameter2 =(int8_t*)FreeRTOS_CLIGetParameter( pcCommandString,
                   2,
                   &xParameter2StringLength );

    /* Terminate both file names. */
    pcParameter1[ xParameter1StringLength ] = 0x00;
    pcParameter2[ xParameter2StringLength ] = 0x00;
    addr =  strtol((char *)pcParameter1,NULL,16); //string to int conversion
    data =  strtol((char *)pcParameter2,NULL,16); //string to int conversion
    TRACE2("Addr %x, Data %x \n\r",addr,data);

    SPI_SetRegister(addr,data);
pcWriteBuffer[0]=0;
    pcWriteBuffer[1]=data;
    pcWriteBuffer[2]=0;
    xWriteBufferLen=3;


    return retVal;

}


static portBASE_TYPE prvReadCommand(int8_t *pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const int8_t *pcCommandString )
{  portBASE_TYPE retVal=0;
    int8_t  *pcParameter1;
    uint16_t addr;
    portBASE_TYPE xParameter1StringLength;

    /* Obtain the name of the source file, and the length of its name, from
    (void)xWriteBufferLen;
    the command string. The name of the source file is the first parameter. */
    pcParameter1 =(int8_t*)FreeRTOS_CLIGetParameter
                   (
                       /* The command string itself. */
                       pcCommandString,
                       /* Return the first parameter. */
                       1,
                       /* Store the parameter string length. */
                       &xParameter1StringLength
                   );


    /* Terminate both file names. */
    pcParameter1[ xParameter1StringLength ] = 0x00;

    addr =  strtol((char *)pcParameter1,NULL,16); //string to int conversion



   uint8_t data=SPI_GetRegister(addr);
   TRACE2("Addr %x, Data %x \n\r",addr,data);
    pcWriteBuffer[0]=0;
    pcWriteBuffer[1]=data;
    pcWriteBuffer[2]=0;
    xWriteBufferLen=3;

    return retVal;
}

static portBASE_TYPE prvQuitCommand(int8_t *pcWriteBuffer,
                            size_t xWriteBufferLen,
                            const int8_t *pcCommandString )
{
    pcWriteBuffer[0]='q';
    pcWriteBuffer[1]=0;
    xWriteBufferLen=2;

    return 0;
}




void checkIRQ()
{
  uint8_t RRC;
TRACE("line IRQ \n\r");

   TestGetIRQ();



    RRC=SPI_GetRegister(REG_IRQ1_SRC1);
    if(RRC|0x02) {
        reciveflag=1;
    }
    else if(RRC|0x04)
    {
       TXdone=1;
    }

}

extern bool slotTiming;

static portBASE_TYPE prvSlotTiming(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	 int8_t  *pcParameter1;
	    uint16_t YorN;
	    portBASE_TYPE xParameter1StringLength;

	    /* Obtain the name of the source file, and the length of its name, from
	    (void)xWriteBufferLen;
	    the command string. The name of the source file is the first parameter. */
	    pcParameter1 =(int8_t*)FreeRTOS_CLIGetParameter
	                   (
	                       /* The command string itself. */
	                       pcCommandString,
	                       /* Return the first parameter. */
	                       1,
	                       /* Store the parameter string length. */
	                       &xParameter1StringLength
	                   );


	    /* Terminate both file names. */
	    pcParameter1[ xParameter1StringLength ] = 0x00;

	    YorN =  strtol((char *)pcParameter1,NULL,16); //string to int conversion
if(YorN==0)
{
	slotTiming=false;
}
else
{
	slotTiming=true;
}

	return 0;
}


static portBASE_TYPE prvGetIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;
	TestGetIRQ();
	return 0;

}

static portBASE_TYPE prvSetIdle(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;
	TestSetIdle();
	return 0;

}

static portBASE_TYPE prvGetStatus(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;
	TestGetStatus();
	return 0;

}

static portBASE_TYPE prvSetRadioReady(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;

	TestSetRadioReady();
	return 0;

}


static portBASE_TYPE prvSetRadioRX(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;

	TestSetRadioRX();
	return 0;

}

static portBASE_TYPE prvGetRXbuf(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{

	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;
	TestGetRXbuf();
	return 0;

}

static portBASE_TYPE prvClrIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{

	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;

	TestClrIRQ();
	return 0;

}
static portBASE_TYPE prvTXBuf(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;

	SetTXBuf();
	return 0;

}

static portBASE_TYPE prvSend(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;

	Send();
	return 0;

}
static portBASE_TYPE prvWaitRXIRQ(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;




	waitRXIRQ();
	return 0;

}
static portBASE_TYPE prvContiniousTX(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	(void)pcWriteBuffer;
	(void)xWriteBufferLen;
	(void)pcCommandString;


	TestContiniousTX();
	return 0;

}
extern bool isMaster;
extern powerSaveStates_t PHY_radistate;
extern states_t PHY_state;
extern uint32_t  recivePktCount;
extern uint32_t  sendPktCount;
extern uint32_t  loadPktCount;
extern uint32_t  frameCount;
extern uint32_t	 reciveSlotCount;
extern uint32_t	 spiUnavalible;


static portBASE_TYPE phyInfo(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
char *Phystate[]={"RN_UNDIFINED","MASTER_NO_REG", "MASTER_RUNNIG","SLAVE_UNREGISTER", "SLAVE_SEARCH"," SLAVE_RNFOUND", "SLAVE_REGISTERED"};
char *PowerState[]={
	"PS_SLEEP",
	"PS_IDLE",
	"PS_RX_READOUT",
	"PS_TX_DELAY",
	"PS_TX_FROMSLEEP",
	"PS_RX_FROMSLEEP",
	"PS_TX_FROMIDLE",
	"PS_RX_FROMIDLE",
	"PS_TX",
	"PS_RX",
	"PS_NEWFRAME",
	"PS_NEWFRAME_PREP",
	"PS_FRAME_DETECT_WIN"};
(void)pcWriteBuffer;
(void)xWriteBufferLen;
(void)pcCommandString;




if(isMaster)
	{
	TRACE("PHY configured as : Master \n\r");
	}
else
	{
	TRACE("PHY configured as : Slave \n\r")
	}
TRACE1("PHY state: %s \n\r", Phystate[PHY_state]);

TRACE1("PHY state: %s \n\r", PowerState[PHY_radistate]);

TRACE2("Receive pkt count %d of possible %d\n\r", recivePktCount,reciveSlotCount);
TRACE1("Send pkt count %d \n\r",sendPktCount);

TRACE1("load pkt count %d \n\r", loadPktCount);
TRACE1("frame count %d \n\r", frameCount);
TRACE1("SPI busy count %d \n\r",spiUnavalible);

return 0;

}


extern slot_t slot_table[];

static portBASE_TYPE slotInfo(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString )
{
	int8_t i;
	char fstr[5];
	char sendstr[100];
	TRACE("Active slot\n\r");

	for(i=0;i<MAXNUMBEROFSLOTS;i++)
	{
		sprintf(fstr," %2d|",i);
		console_write(fstr);
	}
	//trace("\n\r");*/

	for(i=0;i<MAXNUMBEROFSLOTS;i++)
	{
		if(slot_table[i].listenTo)
		{

			if(slot_table[i].SendSlot)
			{
				strcat(sendstr," S |");
			}
			else
			{
				strcat(sendstr," R |");
			}
		}
		else
		{
			strcat(sendstr,"---|");
		}

	}
	strcat(sendstr,"\n\r");
	console_write(sendstr);
	return 0;
}


