#ifndef _MOTOR_H
#define _MOTOR_H

#include "bsp.h"

extern bool stop_flag;

void SetSpeed(int left_speed,int right_speed);
void Move(int speed,float angle);
void Stop(void);

#endif
