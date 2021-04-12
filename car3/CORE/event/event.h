#ifndef _EVENT_H
#define _EVENT_H

#define AVOID_CMD  	0x01
#define ADV_CMD     0x02
#define TASK_CMD  	0x03
#define REPLY_CMD 	0x04
#define SENDPOS_CMD	0x05
#define STOP_CMD    0x06

#include "bsp.h"

extern u8 tx_buffer[PACKET_LENGTH];

void GetBroadcastInfo(void);
void Reply(void);
void TxBuffer_Init(void);
bool CheckData(void);

#endif
