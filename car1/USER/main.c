#include "bsp.h"

int main()
{
	Stm32_Clock_Init(9);
    delay_init(72);
    BSP_Init();					//���������ʼ��
	PID_Total_Init();			//��ʼ������pid
	Stop();						//���ֳ���ֹͣ
    while(1)
	{
		if(u2rx_flag)			//���usart2��UWB�Ƿ��������
		{
			UpdatePosinfo();	//��������λ��
		}
		if(u1rx_flag)			//���usart1��zigbee�Ƿ��������
		{
			GetBroadcastInfo(); //������λ������
		}
	}
}

 




