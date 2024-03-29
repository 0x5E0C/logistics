#ifndef _BSP_H
#define _BSP_H

#define PACKET_LENGTH  10
#define LABEL_ID       0x01
#define CAR_ID 		   LABEL_ID
#define BASE_SPEED	   4000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>
#include "sys.h"
#include "stm32f10x.h"
#include "delay.h"
#include "gpio.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "timer1.h"
#include "timer2.h"
#include "timer3.h"
#include "pid.h"
#include "motor.h"
#include "gyro.h"
#include "pos.h"
#include "event.h"
#include "attitude.h"

void BSP_Init(void);
void PID_Total_Init(void);

#endif
