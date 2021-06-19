#include "pid.h"

PID PID_Gyro;

//������pid��ʼ������
void PID_Total_Init()
{
	PID_Gyro.P = 100;			//��������ϵ��
    PID_Gyro.I = 0.19;			//���ֻ���ϵ��
    PID_Gyro.D = 0; 			//΢�ֻ���ϵ��
    PID_Gyro.OutputMax = 10000;	//����������Ϊ10000
    PID_Gyro.OutputMin = -10000;//��������������Ϊ-10000
    PID_Init(&PID_Gyro);
}

//��ʼ��pid�ĸ�״̬��
void PID_Init(PID* pid)
{
    pid->ActualValue = 0;
    pid->Err = 0;
    pid->Last_Err = 0;
    pid->Last_Err2 = 0;
}

//����ʽpid
//u=Kp*[u(k)-e(k-1)]+Ki*e(k)+Kd*[e(k)-2e(k-1)+e(k-2)]
float Incremental_PID(PID* pid)
{
    pid->Err = pid->ActualValue - pid->SetValue;
    pid->OutputPoint += pid->P * (pid->Err - pid->Last_Err) +
                        pid->I * pid->Err +
                        pid->D * (pid->Err - 2 * pid->Last_Err + pid->Last_Err2);
    pid->Last_Err2 = pid->Last_Err;
    pid->Last_Err = pid->Err;
    if(pid->OutputPoint > pid->OutputMax)
    {
        pid->OutputPoint = pid->OutputMax;
    }
    else if(pid->OutputPoint < pid->OutputMin)
    {
        pid->OutputPoint = pid->OutputMin;
    }
    return pid->OutputPoint;
}

