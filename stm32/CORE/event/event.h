#ifndef _EVENT_H
#define _EVENT_H

#define STOP_CMD  	0x00
#define TASK_CMD  	0x01
#define REPLY_CMD 	0x03
#define SENDPOS_CMD	0x04

#include "bsp.h"

extern u8 tx_buffer[PACKET_LENGTH];

void GetBroadcastInfo(void);
void Reply(void);
void TxBuffer_Init(void);
bool CheckData(void);
void SendCurrentPos(void);

#endif
