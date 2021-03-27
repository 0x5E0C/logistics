#include "attitude.h"

Attitude car_attitude;

void UpdateAttitude()
{
	float x_bias,y_bias,angle;
	x_bias=target.x-current_pos.x;
	y_bias=target.y-current_pos.y;
	angle=atan2(y_bias,x_bias)*180.0/3.1416;
	if(sqrt(x_bias*x_bias+y_bias*y_bias)<10)
	{
		Stop();
		Set_TIM1_Disable();
	}
	else
	{
		Move(BASE_SPEED,angle);
	}
}

