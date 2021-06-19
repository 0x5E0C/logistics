#include "usart1.h"

u8 USART1_RX_BUF[USART1_REC_LEN];
u8 USART1_byteNum;
u8 u1rx_flag=0;
u8 last_byteNum=0;
u8 RX_CACHE[USART1_REC_LEN];

void USART1_Send(u8 data)
{
	while((USART1->SR & 0X40) == 0);
    USART1->DR = data;
}

//usart1连接zigbee无线通讯模块
//设置usart1为dma接收+空闲中断
//中断分组为1,0
void Wireless_USART1_Init(u32 bound)
{	
	float temp;
    temp = 72000000 / bound;
    RCC->APB2ENR |= 1 << 2;
    RCC->APB2ENR |= 1 << 14;
    RCC->AHBENR |= 1 << 0;
    RCC->APB2RSTR |= 1 << 14;
    RCC->APB2RSTR &= ~(1 << 14);
    GPIOA->CRH &= 0xFFFFF00F;
    GPIOA->CRH |= 0x000008B0;
    USART1->BRR = (u16)temp;
    DMA1_Channel5->CNDTR = USART1_REC_LEN;
    DMA1_Channel5->CPAR = (u32)(&USART1->DR);
    DMA1_Channel5->CMAR = (u32)USART1_RX_BUF;
    DMA1_Channel5->CCR |= 0X3081;
    USART1->CR3 |= 1 << 6;
    MY_NVIC_Init(1, 0, USART1_IRQn, 2);
    USART1->CR1 = 0x201C;
}


void USART1_IRQHandler(void)
{
	u8 i;
    if(USART1->SR & (1 << 3))
    {
        USART1->DR;
        USART1->SR &= ~(1 << 3);
    }
    if(USART1->SR & (1 << 4))
    {
		USART1->CR1 &= ~(1 << 5);
        USART1_byteNum = USART1->SR;
        USART1_byteNum = USART1->DR;
        USART1_byteNum = USART1_REC_LEN - DMA1_Channel5->CNDTR;
        DMA1_Channel5->CCR &= ~(1 << 0);
        DMA1_Channel5->CNDTR = USART1_REC_LEN;
		if(USART1_byteNum == PACKET_LENGTH)	//若接收的数据包长度等于定义的包长
		{
			u1rx_flag = 1;					//zigbee数据更新标志位置1
		}
		else if((USART1_byteNum + last_byteNum) == PACKET_LENGTH)//若本次收到的数据与上次接收到的数据总长度等于定义包长
		{
			for(i = 0;i < USART1_byteNum;i++)
			{
				RX_CACHE[i + last_byteNum]=USART1_RX_BUF[i];	//拼接数据包
			}
			for(i = 0;i < USART1_byteNum;i++)
			{
				USART1_RX_BUF[i] = RX_CACHE[i];					//将RX_CACHE内容转存到USART1_RX_BUF中等待处理
			}
			u1rx_flag = 1;						//zigbee数据更新标志位置1
		}
		else									//只接受到前半部分数据
		{
			last_byteNum = USART1_byteNum;		//记录本次接收的数据长度
			for(i = 0;i < USART1_byteNum;i++)
			{
				RX_CACHE[i] = USART1_RX_BUF[i];	//将内容存入RX_CACHE
			}
		}			
        DMA1_Channel5->CCR |= 1 << 0;
		USART1->CR1 |= 1<<5;
    }
}
