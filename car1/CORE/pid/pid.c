#include "pid.h"

PID PID_Gyro;

//将所有pid初始化参数
void PID_Total_Init()
{
	PID_Gyro.P = 100;			//比例环节系数
    PID_Gyro.I = 0.19;			//积分环节系数
    PID_Gyro.D = 0; 			//微分环节系数
    PID_Gyro.OutputMax = 10000;	//最大输出限制为10000
    PID_Gyro.OutputMin = -10000;//反向最大输出限制为-10000
    PID_Init(&PID_Gyro);
}

//初始化pid的各状态量
void PID_Init(PID* pid)
{
    pid->ActualValue = 0;
    pid->Err = 0;
    pid->Last_Err = 0;
    pid->Last_Err2 = 0;
}

//增量式pid
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

