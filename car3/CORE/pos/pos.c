#include "pos.h"

Posinfo current_pos;
Posinfo origin,target,avoid;

//����UWB��ǩ���ص����ݰ��е�X��Y����
void UpdatePosinfo()
{
	int xresult=0,yresult=0;
	int pos,flag=1;
	u2rx_flag=0;
	pos=0;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')	//���������з����ֲ���
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')//�������ֲ�ת��Ϊ��Ӧ����
	{
		xresult=xresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	xresult*=flag;			//������ȡ����
	flag=1;
	while(USART2_RX_BUF[pos]<'0' || USART2_RX_BUF[pos]>'9')
	{
		if(USART2_RX_BUF[pos]=='-')
		{
			flag=-1;
		}
		pos++;
	}
	while(USART2_RX_BUF[pos]>='0' && USART2_RX_BUF[pos]<='9')//�������ֲ�ת��Ϊ��Ӧ����
	{
		yresult=yresult*10+USART2_RX_BUF[pos]-'0';
		pos++;
	}
	yresult*=flag;			//������ȡ����
	current_pos.x=xresult;
	current_pos.y=yresult;
}
