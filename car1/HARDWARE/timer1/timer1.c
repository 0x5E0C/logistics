#include "timer1.h"

long int mills=0;

void Trans_TIM1_Init(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 11;
    TIM1->ARR = arr-1;
    TIM1->PSC = psc-1;
    TIM1->DIER |= 1 << 0;
    MY_NVIC_Init(2, 2, TIM1_UP_IRQn, 2);    
}

void Set_TIM1_Enable()
{
	TIM1->CR1 |= 0x01;
}

void Set_TIM1_Disable()
{
	TIM1->CR1 &= ~(1 << 0);
}

void TIM1_UP_IRQHandler(void)
{
    if(TIM1->SR & 0X0001)
    {
		mills+=20;
		if(!stop_flag && !finish_task_flag)
		{
			UpdateAttitude();
		}
    }
    TIM1->SR &= ~(1 << 0);
}
