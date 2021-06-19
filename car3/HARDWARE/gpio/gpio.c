#include "gpio.h"

//初始化PB0和PB1，分别用于控制电机的1个端口
void GPIO_Init()
{
	RCC->APB2ENR|=1<<3;
	
	GPIOB->CRL&=0XFFFFFF00;
	GPIOB->CRL|=0X00000033;
}
