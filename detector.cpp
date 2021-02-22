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
    memset(map,0,array_size.xsize*array_size.ysize*sizeof(pos_info));
    for(int i=0;i<array_size.xsize*array_size.ysize;i++)
    {
        (map+i)->x=new QList<int>;
        (map+i)->y=new QList<int>;
        (map+i)->id=new QList<quint8>;
    }
}

void detector::clearPosInfo(int xpos,int ypos,quint8 id)
{
    int index=getArrayFromPos(xpos,ypos)->id->indexOf(id);
    getArrayFromPos(xpos,ypos)->x->removeAt(index);
    getArrayFromPos(xpos,ypos)->y->removeAt(index);
    getArrayFromPos(xpos,ypos)->id->removeAt(index);
}

void detector::setPosInfo(int xpos,int ypos,quint8 id)
{
    getArrayFromPos(xpos,ypos)->x->append(xpos);
    getArrayFromPos(xpos,ypos)->y->append(ypos);
    getArrayFromPos(xpos,ypos)->id->append(id);
}

void detector::setCheckpoint(int xpos,int ypos,quint8 id)
{
    pending_info.x=xpos;
    pending_info.y=ypos;
    pending_info.id=id;
    if(!getArrayFromPos(xpos,ypos)->id->isEmpty())
    {
        queue.append(*getArrayFromPos(xpos,ypos)->id);
        queue.append(id);
        emitStopSignals();
    }
    setPosInfo(xpos,ypos,id);
}

void detector::startDetector(int array_x,int array_y)
{
    int index_x=formatXpos(pending_info.x);
    int index_y=formatYpos(pending_info.y);
    queue.clear();
    for(int i=array_x-1;i<=array_x+1;i++)
    {
        for(int j=array_y-1;j<=array_y+1;j++)
        {
            if(!getArray(i,j)->id->isEmpty() && i!=index_x && j!=index_y)
            {
                for(int k=0;k<getArray(i,j)->id->size();k++)
                {
                    qDebug()<<pending_info.x<<pending_info.y<<getArray(i,j)->y->at(k)<<getArray(i,j)->y->at(k);
                    if(sqrt((getArray(i,j)->x->at(k)-pending_info.x)*(getArray(i,j)->x->at(k)-pending_info.x)
                           +(getArray(i,j)->y->at(k)-pending_info.y)*(getArray(i,j)->y->at(k)-pending_info.y))<safe_distance)
                    {
                        queue.append(getArray(i,j)->id->at(i));
                    }
                }
            }
        }
    }
    if(!queue.isEmpty())
    {
        queue.append(pending_info.id);
        emitStopSignals();
    }
}

void detector::emitStopSignals()
{
    std::sort(queue.begin(),queue.end());
    queue.removeAt(0);
    emit stopSignal();
}
