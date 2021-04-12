#include "attitude.h"

Attitude car_attitude;
bool priority=Y_FIRST;
bool x_finish=false,y_finish=false,task_finish=false;
bool avoid_mode=false;

void UpdateAttitude()
{
	float x_bias,y_bias,angle;
	if(avoid_mode)
	{
		x_bias=avoid.x-current_pos.x;
		y_bias=avoid.y-current_pos.y;
		angle=atan2(y_bias,x_bias)*180.0/3.1416;
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)
		{
			origin.x=current_pos.x;
			origin.y=current_pos.y;
			x_finish=false;
			y_finish=false;
			avoid_mode=false;
		}
		else
		{
			Move(BASE_SPEED,angle);
		}
	}
	else
	{
		x_bias=target.x-current_pos.x;
		y_bias=target.y-current_pos.y;
		if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)
		{
			Stop();
			Set_TIM1_Disable();
			task_finish=true;
		}
		else if(sqrt(x_bias*x_bias+y_bias*y_bias)<50)
		{
			angle=atan2(y_bias,x_bias)*180.0/3.1416;
			Move(BASE_SPEED,angle);
		}
		else
		{
			if(priority==X_FIRST)
			{
				Move_X();
			}
			else if(priority==Y_FIRST)
			{
				Move_Y();
			}
		}
	}
	
}

void Move_X()
{
	float x_bias,y_bias,angle;
	x_bias=target.x-current_pos.x;
	y_bias=origin.y-current_pos.y;
	angle=atan2(y_bias,x_bias)*180.0/3.1416;
	Move(BASE_SPEED,angle);
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)
	{
		origin.x=current_pos.x;
		x_finish=true;
		if(!y_finish)
		{
			priority=Y_FIRST;
		}
		else
		{
			task_finish=true;
		}
	}
}

void Move_Y()
{
	float x_bias,y_bias,angle;
	x_bias=origin.x-current_pos.x;
	y_bias=target.y-current_pos.y;
	angle=atan2(y_bias,x_bias)*180.0/3.1416;
	Move(BASE_SPEED,angle);
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)
	{
		origin.y=current_pos.y;
		y_finish=true;
		if(!x_finish)
		{
			priority=X_FIRST;
		}
		else
		{
			task_finish=true;
		}
	}
}
