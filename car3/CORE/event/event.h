#ifndef _EVENT_H
#define _EVENT_H

#define AVOID_CMD  	0x01	//避让指令
#define ADV_CMD     0x02	//前进指令
#define TASK_CMD  	0x03	//任务指令
#define REPLY_CMD 	0x04	//回复指令
#define SENDPOS_CMD	0x05	//发送位置指令
#define STOP_CMD    0x06	//停止指令

#include "bsp.h"

extern u8 tx_buffer[PACKET_LENGTH];

void GetBroadcastInfo(void);
void Reply(void);
void TxBuffer_Init(void);
bool CheckData(void);

#endif
