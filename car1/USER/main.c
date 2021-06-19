#include "bsp.h"

int main()
{
	Stm32_Clock_Init(9);
    delay_init(72);
    BSP_Init();					//板载外设初始化
	PID_Total_Init();			//初始化所有pid
	Stop();						//保持车辆停止
    while(1)
	{
		if(u2rx_flag)			//检测usart2的UWB是否更新数据
		{
			UpdatePosinfo();	//更新自身位置
		}
		if(u1rx_flag)			//检测usart1的zigbee是否更新数据
		{
			GetBroadcastInfo(); //接收上位机命令
		}
	}
}

 




