#include "event.h"

u8 reply_buffer[PACKET_LENGTH];
u8 tx_buffer[PACKET_LENGTH];

//��zigbee�������ݰ��н������ݲ�ִ����Ӧ����
void GetBroadcastInfo()
{
	u1rx_flag=0;
	if(USART1_RX_BUF[0]==0x5E && USART1_RX_BUF[1]==0x0C		//��������ͷ
	   && USART1_RX_BUF[2]==CAR_ID && CheckData())			//id�����У����ȷ
	{
		Reply();											//���ͻظ�����
		if(USART1_RX_BUF[3]==TASK_CMD)						//��Ϊ����ָ��
		{
			stop_flag=false;
			origin.x=current_pos.x;							//��¼��ǰλ��Ϊ������
			origin.y=current_pos.y;
			target.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];//��������¼������
			target.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
			Set_TIM1_Enable();
			UpdateAttitude();
		}
		else if(USART1_RX_BUF[3]==AVOID_CMD)				//��Ϊ����ָ��
		{
			avoid.x=(USART1_RX_BUF[4]<<8)|USART1_RX_BUF[5];
			avoid.y=(USART1_RX_BUF[6]<<8)|USART1_RX_BUF[7];
			avoid_mode=true;								//����ģʽ��1
			task_finish=false;								//��ɱ�־λ��0
			stop_flag=false;								//ֹͣ��־λ��0
		}
		else if(USART1_RX_BUF[3]==STOP_CMD)					//��Ϊָֹͣ��
		{
			Stop();
		}
		else if(USART1_RX_BUF[3]==ADV_CMD)					//��Ϊǰ��ָ��
		{
			stop_flag=false;
		}
	}
}

//���ͻظ�ָ��
void Reply()
{
	u8 i;
	TxBuffer_Init();
	for(i=0;i<PACKET_LENGTH;i++)
	{
		USART1_Send(reply_buffer[i]);
	}
}

//��ʼ����������
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

//����У���
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

