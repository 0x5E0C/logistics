#ifndef _ATTITUDE_H
#define _ATTITUDE_H

#include "bsp.h"

typedef struct _Attitude 
{
	int speed;
	float angle;
} Attitude;

extern Attitude car_attitude;

void UpdateAttitude(void);

#endif
