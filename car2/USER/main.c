#include "bsp.h"

int main()
{
	Stm32_Clock_Init(9);
    delay_init(72);
    BSP_Init();
	PID_Total_Init();
	Stop();
    while(1)
	{
		if(u2rx_flag)
		{
			UpdatePosinfo();
		}
		if(u1rx_flag)
		{
			GetBroadcastInfo();
		}
	}
}

 




