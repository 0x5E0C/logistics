#include "event.h"

u8 reply_buffer[PACKET_LENGTH];
u8 tx_buffer[PACKET_LENGTH];

void GetBroadcastInfo()
{
	u1rx_flag=0;
	if(USART1_RX_BUF[0]==CAR_ID && CheckData())
	{
		Reply();
		if(USART1_RX_BUF[1]==STOP_CMD)
		{
			Stop();
		}
		else if(USART1_RX_BUF[1]==TASK_CMD)
		{
			target.x=(USART1_RX_BUF[2]<<8)|USART1_RX_BUF[3];
			target.y=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];
			Set_TIM1_Enable();
			UpdateAttitude();
		}
	}
}

void Reply()
{
	u8 i;
	for(i=0;i<PACKET_LENGTH;i++)
	{
		USART1_Send(reply_buffer[i]);
	}
}

void TxBuffer_Init()
{
	u16 sum=0;
	u8 i;
	reply_buffer[0]=CAR_ID;
	reply_buffer[1]=REPLY_CMD;
	reply_buffer[2]=0x65;
	reply_buffer[3]=0x36;
	reply_buffer[4]=0x52;
	reply_buffer[5]=0x30;
	for(i=0;i<=5;i++)
	{
		sum+=reply_buffer[i];
	}
	reply_buffer[6]=sum/256;
	reply_buffer[7]=sum%256;
}

bool CheckData()
{
	u16 check,sum=0;
	u8 i;
	for(i=0;i<6;i++)
	{
		sum+=USART1_RX_BUF[i];
	}
	check=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
	return (check==sum);
}

void SendCurrentPos()
{
	int sum=0;
	u8 i;
	tx_buffer[0]=CAR_ID;
	tx_buffer[1]=SENDPOS_CMD;
	tx_buffer[2]=(current_pos.x&0xFF00)>>8;
	tx_buffer[3]=current_pos.x&0x00FF;
	tx_buffer[4]=(current_pos.y&0xFF00)>>8;
	tx_buffer[5]=current_pos.y&0x00FF;
	for(i=0;i<6;i++)
	{
		sum+=tx_buffer[i];
		USART1_Send(tx_buffer[i]);
	}
	USART1_Send((sum&0xFF00)>>8);
	USART1_Send(sum&0x00FF);
}
