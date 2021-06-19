#ifndef _GYRO_H
#define _GYRO_H

#include "bsp.h"

typedef struct _GYRO
{
	float ax;	//X����ٶ�
	float ay;	//Y����ٶ�
	float az;	//Z����ٶ�
	float wx;	//X����ٶ�
	float wy;	//Y����ٶ�
	float wz;	//Z����ٶ�
	float roll;	//������
	float pitch;//������
	float yaw;	//ƫ����
} Gyro;

extern Gyro gyro;

float GetAngleBias(float target);

#endif
