#include "detector.h"

detector::detector()
{

}

void detector::run()
{
    startDetector(formatXpos(pending_info.x),formatXpos(pending_info.y));
}

void detector::createMap(int xsize,int ysize)
{
    array_size.xsize=(xsize%safe_distance==0)?(xsize/safe_distance):(xsize/safe_distance+1);
    array_size.ysize=(ysize%safe_distance==0)?(ysize/safe_distance):(ysize/safe_distance+1);
    array_size.xsize+=2;
    array_size.ysize+=2;
    map=(pos_info*)malloc(array_size.xsize*array_size.ysize*sizeof(pos_info));
    memset(map,-1,array_size.xsize*array_size.ysize*sizeof(pos_info));
}

void detector::clearPosInfo(int xpos,int ypos,quint8 id)
{
    getArrayFromPos(xpos,ypos)->x=0;
    getArrayFromPos(xpos,ypos)->y=0;
    getArrayFromPos(xpos,ypos)->exist=false;
    if(queue[getArrayFromPos(xpos,ypos)->index].size()==1)
    {
        queue.removeAt(getArrayFromPos(xpos,ypos)->index);
        qDebug()<<"removeAll";
    }
    else
    {
        queue[getArrayFromPos(xpos,ypos)->index].removeOne(id);
        qDebug()<<"removeAt";
    }
    getArrayFromPos(xpos,ypos)->index=-1;
}

void detector::setPosInfo(int xpos,int ypos,quint8 id)
{
    getArrayFromPos(xpos,ypos)->x=xpos;
    getArrayFromPos(xpos,ypos)->y=ypos;
    getArrayFromPos(xpos,ypos)->exist=true;
    if(getArrayFromPos(xpos,ypos)->index==-1)
    {
        QList<quint8> id_list;
        id_list.append(id);
        getArrayFromPos(xpos,ypos)->index=queue.size();
        queue.append(id_list);
        qDebug()<<"set";
    }
    else
    {
        queue[getArrayFromPos(xpos,ypos)->index].append(id);
        qDebug()<<"new";
    }
}

void detector::setCheckpoint(int xpos,int ypos,quint8 id)
{
    pending_info.x=xpos;
    pending_info.y=ypos;
    pending_info.id=id;
    if(getArrayFromPos(pending_info.x,pending_info.y)->exist==true)
    {
        qDebug()<<"crash";
        //emit stopSignal(pending_info.id,getArrayFromPos(pending_info.x,pending_info.y)->lineid);
    }
    else
    {
        setPosInfo(xpos,ypos,id);
    }
}

void detector::startDetector(int array_x,int array_y)
{
    for(int i=array_x-1;i<array_x+2;i++)
    {
        for(int j=array_y-1;j<array_y+2;j++)
        {
            if(i==array_x && j==array_y)
            {
                continue;
            }
            if(getArray(i,j)->exist==true)
            {
                if(sqrt((getArray(i,j)->x-pending_info.x)*(getArray(i,j)->x-pending_info.x)+
                        (getArray(i,j)->y-pending_info.y)*(getArray(i,j)->y-pending_info.y))<safe_distance)
                {
                    qDebug()<<"emit";
                    return;
                    //emit stopSignal(pending_info.id,getArrayFromPos(pending_info.x,pending_info.y)->lineid);
                }
            }
        }
    }
}
