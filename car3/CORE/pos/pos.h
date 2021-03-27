#ifndef _POS_H
#define _POS_H

#include "bsp.h"

typedef struct _POSINFO
{
	int x;
	int y;
} Posinfo;

extern Posinfo current_pos;
extern Posinfo target;
	
void UpdatePosinfo(void);

#endif
