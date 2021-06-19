#ifndef _PID_H
#define _PID_H

#include "bsp.h"

#define No_Max	FLT_MAX	
#define No_Min  -FLT_MAX

typedef struct _PID 
{
	float SetValue;		//目标值
	float ActualValue;	//当前值
	float Err;			//当前值与上一个输入值的差值
	float Last_Err;		//上一个输入值与上上个输入值的差值
	float Last_Err2;	//上上个输入值与上上上个输入值的差值
	float P,I,D;		//比例环节系数，积分环节系数，微分环节系数
	float OutputMax;	//最大输出限制
	float OutputMin;	//最小输出限制
	float OutputPoint;	//输出值
} PID;

extern PID PID_Gyro;	//陀螺仪相关的pid

void PID_Total_Init(void);
void PID_Init(PID *pid);
float Incremental_PID(PID *pid);

#endif
