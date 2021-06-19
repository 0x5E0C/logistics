#ifndef _PID_H
#define _PID_H

#include "bsp.h"

#define No_Max	FLT_MAX	
#define No_Min  -FLT_MAX

typedef struct _PID 
{
	float SetValue;		//Ŀ��ֵ
	float ActualValue;	//��ǰֵ
	float Err;			//��ǰֵ����һ������ֵ�Ĳ�ֵ
	float Last_Err;		//��һ������ֵ�����ϸ�����ֵ�Ĳ�ֵ
	float Last_Err2;	//���ϸ�����ֵ�������ϸ�����ֵ�Ĳ�ֵ
	float P,I,D;		//��������ϵ�������ֻ���ϵ����΢�ֻ���ϵ��
	float OutputMax;	//����������
	float OutputMin;	//��С�������
	float OutputPoint;	//���ֵ
} PID;

extern PID PID_Gyro;	//��������ص�pid

void PID_Total_Init(void);
void PID_Init(PID *pid);
float Incremental_PID(PID *pid);

#endif
