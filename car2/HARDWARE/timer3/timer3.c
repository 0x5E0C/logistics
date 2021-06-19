#include "timer3.h"

//tim3���ڼ���pid
//����Ϊ20msһ�θ����ж�
//���ȼ�1,1
void Timing_TIM3_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 1;
    TIM3->ARR = arr-1;
    TIM3->PSC = psc-1;
    TIM3->DIER |= 1 << 0;
    MY_NVIC_Init(1, 1, TIM3_IRQn, 2);
    TIM3->CR1 |= 0x01;
}

//�ж��У�������Ϊֹͣ״̬�����pid����
//����󽫽�������ص����Ϊ���
void TIM3_IRQHandler(void)
{
	int bias;
    if(TIM3->SR & 0X0001)
    {
		if(!stop_flag && !task_finish)
		{
			PID_Gyro.ActualValue=GetAngleBias(car_attitude.angle);//��ýǶ�ƫ��
			bias=Incremental_PID(&PID_Gyro);	//���pid������
			if(abs(bias)<500)				    //С��500��ƫ��������					
			{
				PID_Init(&PID_Gyro);			//���������
			}
			SetSpeed(car_attitude.speed+bias,car_attitude.speed-bias);//��ֵ�����pwm
		}
    }
    TIM3->SR &= ~(1 << 0);
}
