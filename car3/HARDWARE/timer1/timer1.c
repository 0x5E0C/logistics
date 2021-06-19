#include "timer1.h"

long int mills=0;	//用于全局计时

//tim1用于定时更新姿态
//定时设置为20ms一次更新中断
//中断分组为2,2
void Trans_TIM1_Init(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 11;
    TIM1->ARR = arr-1;
    TIM1->PSC = psc-1;
    TIM1->DIER |= 1 << 0;
    MY_NVIC_Init(2, 2, TIM1_UP_IRQn, 2);    
}

//使能定时器1
void Set_TIM1_Enable()
{
	TIM1->CR1 |= 0x01;
}

//关闭定时器1
void Set_TIM1_Disable()
{
	TIM1->CR1 &= ~(1 << 0);
}

//中断中更新姿态
void TIM1_UP_IRQHandler(void)
{
    if(TIM1->SR & 0X0001)
    {
		mills+=20;
		if(!stop_flag && !task_finish)	//车为前进状态且任务未完成
		{
			UpdateAttitude();			//更新姿态
		}
    }
    TIM1->SR &= ~(1 << 0);
}
