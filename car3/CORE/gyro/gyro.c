#include "gyro.h"

Gyro gyro;

//获取当前与目标角度的差值，并格式化输出
float GetAngleBias(float target)
{
	if(gyro.yaw>=target)
	{
		if((gyro.yaw-target)>180) 
		{
			return (gyro.yaw-target)-360;
		}
		else	
		{
			return (gyro.yaw-target); 
		}
	}
	else
	{
		if((gyro.yaw+360-target)>180)
		{
			return (gyro.yaw+360-target)-360;
		}
		else
		{				
			return gyro.yaw+360-target;
		}
	}
}
