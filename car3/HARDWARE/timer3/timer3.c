#include "timer3.h"

//tim3用于计算pid
//设置为20ms一次更新中断
//优先级1,1
void Timing_TIM3_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 1;
    TIM3->ARR = arr-1;
    TIM3->PSC = psc-1;
    TIM3->DIER |= 1 << 0;
    MY_NVIC_Init(1, 1, TIM3_IRQn, 2);
    TIM3->CR1 |= 0x01;
}

//中断中，若车不为停止状态则进行pid计算
//计算后将结果反馈回电机作为输出
void TIM3_IRQHandler(void)
{
	int bias;
    if(TIM3->SR & 0X0001)
    {
		if(!stop_flag && !task_finish)
		{
			PID_Gyro.ActualValue=GetAngleBias(car_attitude.angle);//获得角度偏差
			bias=Incremental_PID(&PID_Gyro);	//获得pid计算结果
			if(abs(bias)<500)				    //小于500的偏差看做无误差					
			{
				PID_Init(&PID_Gyro);			//将误差清零
			}
			SetSpeed(car_attitude.speed+bias,car_attitude.speed-bias);//赋值电机的pwm
		}
    }
    TIM3->SR &= ~(1 << 0);
}
