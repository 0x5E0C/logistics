#include "pos.h"

Posinfo current_pos;
Posinfo origin,target,avoid;

//解析UWB标签返回的数据包中的X和Y坐标
void UpdatePosinfo()
{
	int xresult=0,yresult=0;
	int pos,flag=1;
	u2rx_flag=0;
	pos=0;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')	//跳过数据中非数字部分
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')//解析数字并转化为相应坐标
	{
		xresult=xresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	xresult*=flag;			//按符号取正负
	flag=1;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')//解析数字并转化为相应坐标
	{
		yresult=yresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	yresult*=flag;			//按符号取正负
	current_pos.x=xresult;
	current_pos.y=yresult;
}
