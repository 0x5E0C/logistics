#ifndef _ATTITUDE_H
#define _ATTITUDE_H

#include "bsp.h"

#define X_FIRST 0
#define Y_FIRST 1

typedef struct _Attitude 
{
	int speed;
	float angle;
} Attitude;

extern Attitude car_attitude;
extern bool priority;
extern bool x_finish,y_finish,task_finish;
extern bool avoid_mode;

void UpdateAttitude(void);
void Move_X(void);
void Move_Y(void);

#endif
