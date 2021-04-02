#include "event.h"

u8 reply_buffer[PACKET_LENGTH];
u8 tx_buffer[PACKET_LENGTH];
bool finish_task_flag;

void GetBroadcastInfo()
{
	u1rx_flag=0;
	if(USART1_RX_BUF[0]==0x5E && USART1_RX_BUF[1]==0x0C
	   && USART1_RX_BUF[2]==CAR_ID && CheckData())
	{
		Reply();
		if(USART1_RX_BUF[3]==TASK_CMD)
		{
			finish_task_flag=false;
			target.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];
			target.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
			Set_TIM1_Enable();
			UpdateAttitude();
		}
		else if(USART1_RX_BUF[3]==STOP_CMD)
		{
			Stop();
			printf("\r\n3stop\r\n");
		}
		else if(USART1_RX_BUF[3]==ADV_CMD)
		{
			stop_flag=false;
			printf("\r\n3adv\r\n");
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
	reply_buffer[0]=0x5E;
	reply_buffer[1]=0x0C;
	reply_buffer[2]=CAR_ID;
	reply_buffer[3]=REPLY_CMD;
	reply_buffer[4]=0x00;
	reply_buffer[5]=0x00;
	reply_buffer[6]=0x00;
	reply_buffer[7]=0x00;
	for(i=0;i<=7;i++)
	{
		sum+=reply_buffer[i];
	}
	reply_buffer[8]=sum/256;
	reply_buffer[9]=sum%256;
}

bool CheckData()
{
	u16 check,sum=0;
	u8 i;
	for(i=0;i<PACKET_LENGTH-2;i++)
	{
		sum+=USART1_RX_BUF[i];
	}
	check=(USART1_RX_BUF[PACKET_LENGTH-2]<<8)|USART1_RX_BUF[PACKET_LENGTH-1];
	return (check==sum);
}
