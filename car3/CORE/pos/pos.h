#ifndef _POS_H
#define _POS_H

#include "bsp.h"

typedef struct _POSINFO
{
	int x;	//X坐标
	int y;	//Y坐标
} Posinfo;

extern Posinfo current_pos;			//当前位置
extern Posinfo origin,target,avoid;	//出发点，目标点，避让点
	
void UpdatePosinfo(void);

#endif
