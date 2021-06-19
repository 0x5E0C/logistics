#include "attitude.h"

Attitude car_attitude;
bool priority=Y_FIRST;
bool x_finish=false,y_finish=false,task_finish=false;
bool avoid_mode=false;

//更新姿态函数
void UpdateAttitude()
{
	float x_bias,y_bias,angle;
	if(avoid_mode)								//如果是避让模式
	{
		x_bias=avoid.x-current_pos.x;			//计算距离避让点的X偏差
		y_bias=avoid.y-current_pos.y;			//计算距离避让点的Y偏差
		angle=atan2(y_bias,x_bias)*180.0/3.1416;//计算与避让点的角度偏差
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//误差小于10cm时重置出发点并退出避让模式
		{
			origin.x=current_pos.x;
			origin.y=current_pos.y;
			x_finish=false;
			y_finish=false;
			avoid_mode=false;
		}
		else
		{
			Move(BASE_SPEED,angle);				//设置移动的目标速度与角度
		}
	}
	else										//如果不是避让模式
	{
		x_bias=target.x-current_pos.x;			//计算距离目标点的X偏差
		y_bias=target.y-current_pos.y;			//计算距离目标点的Y偏差
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//距离小于10cm时视为已到达目标点
		{
			Stop();								//停止前行
			Set_TIM1_Disable();					//停止更新姿态
			task_finish=true;					//标志完成任务
		}
		else if(sqrt(x_bias*x_bias+y_bias*y_bias)<50)//距离小于50cm时
		{
			angle=atan2(y_bias,x_bias)*180.0/3.1416; 
			Move(BASE_SPEED,angle);				//直接向目标点前进
		}
		else									//距离大于50cm时
		{
			if(priority==X_FIRST)				//前进优先级若为优先走X
			{
				Move_X();						//沿X轴方向前进
			}
			else if(priority==Y_FIRST)			//前进优先级若为优先走Y
			{
				Move_Y();						//沿Y轴方向前进
			}
		}
	}
}

//沿X轴方向前进的控制函数
void Move_X()
{
	float x_bias,y_bias,angle;
	x_bias=target.x-current_pos.x;			//计算与目标点的X坐标的X偏差
	y_bias=origin.y-current_pos.y;			//计算与目标点的X坐标的Y偏差
	angle=atan2(y_bias,x_bias)*180.0/3.1416;//计算与目标点的X坐标的角度偏差
	Move(BASE_SPEED,angle);					//直接向目标点的X坐标前进
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//距离小于10cm时视为已到达目标点
	{
		origin.x=current_pos.x;				//重置出发点的X坐标
		x_finish=true;						//X方向已完成前进
		if(!y_finish)						//Y方向若已完成前进
		{
			priority=Y_FIRST;				//前进优先级切换为优先走Y
		}
		else
		{
			task_finish=true;
		}
	}
}

//沿Y轴方向前进的控制函数
void Move_Y()
{
	float x_bias,y_bias,angle;
	x_bias=origin.x-current_pos.x;			//计算与目标点的Y坐标的X偏差
	y_bias=target.y-current_pos.y;			//计算与目标点的Y坐标的Y偏差
	angle=atan2(y_bias,x_bias)*180.0/3.1416;//计算与目标点的Y坐标的角度偏差
	Move(BASE_SPEED,angle);					//直接向目标点的Y坐标前进
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)//距离小于10cm时视为已到达目标点
	{
		origin.y=current_pos.y;				//重置出发点的X坐标
		y_finish=true;						//Y方向已完成前进
		if(!x_finish)						//X方向若已完成前进
		{
			priority=X_FIRST;				//前进优先级切换为优先走X
		}
		else
		{
			task_finish=true;
		}
	}
}
