#include "pos.h"

Posinfo current_pos;
Posinfo origin,target,avoid;

void UpdatePosinfo()
{
	int xresult=0,yresult=0;
	int pos,flag=1;
	u2rx_flag=0;
	pos=0;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')
	{
		xresult=xresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	xresult*=flag;
	flag=1;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')
	{
		yresult=yresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	yresult*=flag;
	current_pos.x=xresult;
	current_pos.y=yresult;
}
