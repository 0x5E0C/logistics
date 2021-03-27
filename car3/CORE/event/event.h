#ifndef _EVENT_H
#define _EVENT_H

#define WAIT_CMD  	0x01
#define ADV_CMD     0x02
#define TASK_CMD  	0x03
#define REPLY_CMD 	0x04
#define SENDPOS_CMD	0x05
#define STOP_CMD    0x06

#define WAIT_STATE	WAIT_CMD
#define STOP_STATE  STOP_CMD

#include "bsp.h"

extern u8 tx_buffer[PACKET_LENGTH];
extern u8 car_state;


void GetBroadcastInfo(void);
void Reply(void);
void TxBuffer_Init(void);
bool CheckData(void);
void SendCurrentPos(void);

#endif
