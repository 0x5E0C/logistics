#include "attitude.h"

Attitude car_attitude;
bool priority=Y_FIRST;
bool x_finish=false,y_finish=false,task_finish=false;
bool avoid_mode=false;

//������̬����
void UpdateAttitude()
{
	float x_bias,y_bias,angle;
	if(avoid_mode)								//����Ǳ���ģʽ
	{
		x_bias=avoid.x-current_pos.x;			//���������õ��Xƫ��
		y_bias=avoid.y-current_pos.y;			//���������õ��Yƫ��
		angle=atan2(y_bias,x_bias)*180.0/3.1416;//��������õ�ĽǶ�ƫ��
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//���С��10cmʱ���ó����㲢�˳�����ģʽ
		{
			origin.x=current_pos.x;
			origin.y=current_pos.y;
			x_finish=false;
			y_finish=false;
			avoid_mode=false;
		}
		else
		{
			Move(BASE_SPEED,angle);				//�����ƶ���Ŀ���ٶ���Ƕ�
		}
	}
	else										//������Ǳ���ģʽ
	{
		x_bias=target.x-current_pos.x;			//�������Ŀ����Xƫ��
		y_bias=target.y-current_pos.y;			//�������Ŀ����Yƫ��
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//����С��10cmʱ��Ϊ�ѵ���Ŀ���
		{
			Stop();								//ֹͣǰ��
			Set_TIM1_Disable();					//ֹͣ������̬
			task_finish=true;					//��־�������
		}
		else if(sqrt(x_bias*x_bias+y_bias*y_bias)<50)//����С��50cmʱ
		{
			angle=atan2(y_bias,x_bias)*180.0/3.1416; 
			Move(BASE_SPEED,angle);				//ֱ����Ŀ���ǰ��
		}
		else									//�������50cmʱ
		{
			if(priority==X_FIRST)				//ǰ�����ȼ���Ϊ������X
			{
				Move_X();						//��X�᷽��ǰ��
			}
			else if(priority==Y_FIRST)			//ǰ�����ȼ���Ϊ������Y
			{
				Move_Y();						//��Y�᷽��ǰ��
			}
		}
	}
}

//��X�᷽��ǰ���Ŀ��ƺ���
void Move_X()
{
	float x_bias,y_bias,angle;
	x_bias=target.x-current_pos.x;			//������Ŀ����X�����Xƫ��
	y_bias=origin.y-current_pos.y;			//������Ŀ����X�����Yƫ��
	angle=atan2(y_bias,x_bias)*180.0/3.1416;//������Ŀ����X����ĽǶ�ƫ��
	Move(BASE_SPEED,angle);					//ֱ����Ŀ����X����ǰ��
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//����С��10cmʱ��Ϊ�ѵ���Ŀ���
	{
		origin.x=current_pos.x;				//���ó������X����
		x_finish=true;						//X���������ǰ��
		if(!y_finish)						//Y�����������ǰ��
		{
			priority=Y_FIRST;				//ǰ�����ȼ��л�Ϊ������Y
		}
		else
		{
			task_finish=true;
		}
	}
}

//��Y�᷽��ǰ���Ŀ��ƺ���
void Move_Y()
{
	float x_bias,y_bias,angle;
	x_bias=origin.x-current_pos.x;			//������Ŀ����Y�����Xƫ��
	y_bias=target.y-current_pos.y;			//������Ŀ����Y�����Yƫ��
	angle=atan2(y_bias,x_bias)*180.0/3.1416;//������Ŀ����Y����ĽǶ�ƫ��
	Move(BASE_SPEED,angle);					//ֱ����Ŀ����Y����ǰ��
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//����С��10cmʱ��Ϊ�ѵ���Ŀ���
	{
		origin.y=current_pos.y;				//���ó������X����
		y_finish=true;						//Y���������ǰ��
		if(!x_finish)						//X�����������ǰ��
		{
			priority=X_FIRST;				//ǰ�����ȼ��л�Ϊ������X
		}
		else
		{
			task_finish=true;
		}
	}
}
