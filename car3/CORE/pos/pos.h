#ifndef _POS_H
#define _POS_H

#include "bsp.h"

typedef struct _POSINFO
{
	int x;	//X����
	int y;	//Y����
} Posinfo;

extern Posinfo current_pos;			//��ǰλ��
extern Posinfo origin,target,avoid;	//�����㣬Ŀ��㣬���õ�
	
void UpdatePosinfo(void);

#endif
