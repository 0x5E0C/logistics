#ifndef _GYRO_H
#define _GYRO_H

#include "bsp.h"

typedef struct _GYRO
{
	float ax;	//X轴加速度
	float ay;	//Y轴加速度
	float az;	//Z轴加速度
	float wx;	//X轴角速度
	float wy;	//Y轴角速度
	float wz;	//Z轴角速度
	float roll;	//翻滚角
	float pitch;//俯仰角
	float yaw;	//偏航角
} Gyro;

extern Gyro gyro;

float GetAngleBias(float target);

#endif
