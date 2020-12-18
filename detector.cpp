#include "detector.h"

detector::detector()
{

}

void detector::run()
{
    int index_x=formatXpos(pending_info.x);
    int index_y=formatYpos(pending_info.y);
    for(int i=index_x-1;i<index_x+2;i++)
    {
        for(int j=index_y-1;j<index_y+2;j++)
        {
            if(i==index_x && j==index_y)
            {
                continue;
            }
            if(getArray(i,j)->exist)
            {
                if(sqrt((getArray(i,j)->x-pending_info.x)*(getArray(i,j)->x-pending_info.x)+
                        (getArray(i,j)->y-pending_info.y)*(getArray(i,j)->y-pending_info.y))<safe_distance)
                {
                    emit stopSignal(pending_info.id,getArrayFromPos(pending_info.x,pending_info.y)->lineid);
                }
            }
        }
    }
}

void detector::createMap(int xsize,int ysize)
{
    array_size.xsize=(xsize%safe_distance==0)?(xsize/safe_distance):(xsize/safe_distance+1);
    array_size.ysize=(ysize%safe_distance==0)?(ysize/safe_distance):(ysize/safe_distance+1);
    array_size.xsize+=2;
    array_size.ysize+=2;
    map=(pos_info*)malloc(array_size.xsize*array_size.ysize*sizeof(pos_info));
    memset(map,0,array_size.xsize*array_size.ysize*sizeof(pos_info));
}

void detector::clearPosInfo(int xpos,int ypos)
{
    getArrayFromPos(xpos,ypos)->x=0;
    getArrayFromPos(xpos,ypos)->y=0;
    getArrayFromPos(xpos,ypos)->exist=false;
    getArrayFromPos(xpos,ypos)->lineid=0;
}

void detector::setPosInfo(int xpos,int ypos,quint8 lineid)
{
    getArrayFromPos(xpos,ypos)->x=xpos;
    getArrayFromPos(xpos,ypos)->y=ypos;
    getArrayFromPos(xpos,ypos)->exist=true;
    getArrayFromPos(xpos,ypos)->lineid=lineid;
}

void detector::setCheckpoint(int xpos,int ypos,quint8 lineid)
{
    pending_info.x=xpos;
    pending_info.y=ypos;
    pending_info.id=lineid;
    if(getArrayFromPos(pending_info.x,pending_info.y)->exist)
    {
        emit stopSignal(pending_info.id,getArrayFromPos(pending_info.x,pending_info.y)->lineid);
    }
    else
    {
        setPosInfo(xpos,ypos,lineid);
    }
}
