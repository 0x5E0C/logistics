#include "timer1.h"

long int mills=0;	//����ȫ�ּ�ʱ

//tim1���ڶ�ʱ������̬
//��ʱ����Ϊ20msһ�θ����ж�
//�жϷ���Ϊ2,2
void Trans_TIM1_Init(u16 arr, u16 psc)
{
    RCC->APB2ENR |= 1 << 11;
    TIM1->ARR = arr-1;
    TIM1->PSC = psc-1;
    TIM1->DIER |= 1 << 0;
    MY_NVIC_Init(2, 2, TIM1_UP_IRQn, 2);    
}

//ʹ�ܶ�ʱ��1
void Set_TIM1_Enable()
{
	TIM1->CR1 |= 0x01;
}

//�رն�ʱ��1
void Set_TIM1_Disable()
{
	TIM1->CR1 &= ~(1 << 0);
}

//�ж��и�����̬
void TIM1_UP_IRQHandler(void)
{
    if(TIM1->SR & 0X0001)
    {
		mills+=20;
		if(!stop_flag && !task_finish)	//��Ϊǰ��״̬������δ���
		{
			UpdateAttitude();			//������̬
		}
    }
    TIM1->SR &= ~(1 << 0);
}
