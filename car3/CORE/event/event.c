#include "event.h"

u8 reply_buffer[PACKET_LENGTH];
u8 tx_buffer[PACKET_LENGTH];

//从zigbee返回数据包中解析数据并执行相应程序
void GetBroadcastInfo()
{
	u1rx_flag=0;
	if(USART1_RX_BUF[0]==0x5E && USART1_RX_BUF[1]==0x0C		//解析出包头
	   && USART1_RX_BUF[2]==CAR_ID && CheckData())			//id相符且校验正确
	{
		Reply();											//发送回复命令
		if(USART1_RX_BUF[3]==TASK_CMD)						//若为任务指令
		{
			stop_flag=false;
			origin.x=current_pos.x;							//记录当前位置为出发点
			origin.y=current_pos.y;
			target.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];//解析并记录出发点
			target.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
			Set_TIM1_Enable();
			UpdateAttitude();
		}
		else if(USART1_RX_BUF[3]==AVOID_CMD)				//若为避让指令
		{
			avoid.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];
			avoid.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
			avoid_mode=true;								//避让模式置1
			task_finish=false;								//完成标志位置0
			stop_flag=false;								//停止标志位置0
		}
		else if(USART1_RX_BUF[3]==STOP_CMD)					//若为停止指令
		{
			Stop();
		}
		else if(USART1_RX_BUF[3]==ADV_CMD)					//若为前进指令
		{
			stop_flag=false;
		}
	}
}

//发送回复指令
void Reply()
{
	u8 i;
	TxBuffer_Init();
	for(i=0;i<PACKET_LENGTH;i++)
	{
		USART1_Send(reply_buffer[i]);
	}
}

//初始化发送数组
void TxBuffer_Init()
{
	u16 sum=0;
	u8 i;
	reply_buffer[0]=0x5E;
	reply_buffer[1]=0x0C;
	reply_buffer[2]=CAR_ID;
	reply_buffer[3]=REPLY_CMD;
	reply_buffer[4]=priority;
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

//检验校验和
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

