#include "event.h"

u8 reply_buffer[PACKET_LENGTH];
u8 tx_buffer[PACKET_LENGTH];
u8 car_state;

void GetBroadcastInfo()
{
	u1rx_flag=0;
	if(USART1_RX_BUF[0]==0x5E && USART1_RX_BUF[1]==0x0C
	   && USART1_RX_BUF[2]==CAR_ID && CheckData())
	{
		Reply();
		if(USART1_RX_BUF[3]==WAIT_CMD)
		{
			car_state=WAIT_STATE;
			Stop();
		}
		else if(USART1_RX_BUF[3]==STOP_CMD)
		{
			car_state=STOP_STATE;
			Stop();
		}
		else if(USART1_RX_BUF[3]==TASK_CMD)
		{
			target.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];
			target.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
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

void SendCurrentPos()
{
	int sum=0;
	u8 i;
	tx_buffer[0]=0x5E;
	tx_buffer[1]=0x0C;
	tx_buffer[2]=CAR_ID;
	tx_buffer[3]=(car_state==WAIT_STATE)?WAIT_CMD:SENDPOS_CMD;
	tx_buffer[4]=(current_pos.x&0xFF00)>>8;
	tx_buffer[5]=current_pos.x&0x00FF;
	tx_buffer[6]=(current_pos.y&0xFF00)>>8;
	tx_buffer[7]=current_pos.y&0x00FF;
	for(i=0;i<=7;i++)
	{
		sum+=tx_buffer[i];
		USART1_Send(tx_buffer[i]);
	}
	USART1_Send((sum&0xFF00)>>8);
	USART1_Send(sum&0x00FF);
}
