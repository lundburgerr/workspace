/******************** Copyright (c) 2013 Catron Elektronik AB ******************
* Filename          : slot_timimg.h
* Author            : Björn Lindberg ÅF
* Description       : Declaration of rn_phy module internal interface
********************************************************************************
* Revision History
* 2013-04-09        : First Issue
*******************************************************************************/


#ifndef SLOT_API_H_
#define  SLOT_API__H_

#define radio
#define rnirecive

#include <stdint.h>
//#include "rn_phy_api.h"
#include "rn_phy.h"
#include <ti/sysbios/knl/Mailbox.h>

typedef struct slot {
	uint8_t slotPos;
	bool registred;
	bool listenTo;
	bool SendSlot;
	phy_slot_conf_t slotType;
	uint8_t lastRSSI;
	MessageQ_QueueId destbuf;
} slot_t;

typedef struct data_item {
	uint8_t frame;
	uint8_t channel;
	uint16_t length;
	uint8_t dataBuf[];
} data_item_t;


typedef struct
{
	 msg_buf_t		header;
	 uint8_t frame;
	 uint8_t channel;
	 uint16_t length;
	 uint8_t dataBuf[50];
}msg_audio_t;


#define MAXNUMBEROFSLOTS 15


typedef enum {
	RN_UNDIFINED, MASTER_NO_REG, MASTER_RUNNIG,

	SLAVE_UNREGISTER, SLAVE_SEARCH, SLAVE_RNFOUND, SLAVE_REGISTERED

} states_t;

typedef enum {
	PS_SLEEP = 0,
	PS_IDLE,
	PS_RX_READOUT,
	PS_TX_DELAY,
	PS_TX_FROMSLEEP,
	PS_RX_FROMSLEEP,
	PS_TX_FROMIDLE,
	PS_RX_FROMIDLE,
	PS_TX,
	PS_RX,
	PS_NEWFRAME,
	PS_NEWFRAME_PREP,
	PS_FRAME_DETECT_WIN
} powerSaveStates_t;


/*
 ** brief\ Function for upper layer to add new channel to be listening on.
 * param\ pos, slot position same as channel
 * param\ active enable disable function for keep data but not receive.
 * param\ sendSlot is true if PT supposed to send on that slot
 * param\ queID id to higher layer queue or receive buffer.
 *
 */

void PHY_registerSlotlistener(uint8_t pos, bool active, bool sendSlot,
		MessageQ_QueueId queID);

void PHY_unRegisterSlotlistener(uint8_t pos);

/*
 ** brief\ Activate a sending on RAC on the next possible frame
 * param\ *buf contains data to send
 *
 */

void PHY_SendRAC(uint8_t *buf);

/*
 ** brief\ Activate a sending UCC on the next possible frame
 * param\ *buf contains data to send
 *
 */
void PHY_SendUCC(uint8_t *buf);
/*
 ** brief\ Activate a sending UC Voice on the next possible frame
 * param\ *buf contains data to send
 *
 */

void PHY_SendUCVoice(uint8_t *buf);

void PHY_Init_Phy(bool isMaster, uint8_t rnId);

void PHY_SetRNID(uint8_t rnId);

void PHY_setSendSlot(uint8_t slotNum);

void slotMainThread(void);

#endif /*  SLOT_API_ */
