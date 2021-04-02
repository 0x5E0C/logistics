#include "detector.h"

detector::detector()
{
    isbusy=false;
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
    if(index==-1)
    {
        return;
    }
    //qDebug()<<"before clear:"<<*getArrayFromPos(xpos,ypos)->id;
    getArrayFromPos(xpos,ypos)->x->removeAt(index);
    getArrayFromPos(xpos,ypos)->y->removeAt(index);
    getArrayFromPos(xpos,ypos)->id->removeAll(id);
    //qDebug()<<"after clear:"<<*getArrayFromPos(xpos,ypos)->id;
}

void detector::setPosInfo(int xpos,int ypos,quint8 id)
{
    getArrayFromPos(xpos,ypos)->x->append(xpos);
    getArrayFromPos(xpos,ypos)->y->append(ypos);
    getArrayFromPos(xpos,ypos)->id->append(id);
}

void detector::setCheckPoint(int xpos,int ypos,quint8 id)
{
    pending_info.x=xpos;
    pending_info.y=ypos;
    pending_info.id=id;
//    if(!getArrayFromPos(xpos,ypos)->id->isEmpty())
//    {
//        queue.clear();
//        queue.append(*getArrayFromPos(xpos,ypos)->id);
//        queue.append(id);
//        qDebug()<<"set:"<<queue;
//        emitRegSignals();
//    }
    //qDebug()<<"before set:"<<*getArrayFromPos(xpos,ypos)->id;
    setPosInfo(xpos,ypos,id);
    //qDebug()<<"after set:"<<*getArrayFromPos(xpos,ypos)->id;
}

void detector::startDetector(int array_x,int array_y)
{
    isbusy=true;
    queue.clear();
    for(int i=array_x-1;i<=array_x+1;i++)
    {
        for(int j=array_y-1;j<=array_y+1;j++)
        {
            if(!getArray(i,j)->id->isEmpty())
            {
                for(int k=0;k<getArray(i,j)->id->size();k++)
                {
                    //qDebug()<<sqrt((getArray(i,j)->x->at(k)-pending_info.x)*(getArray(i,j)->x->at(k)-pending_info.x)
                    //               +(getArray(i,j)->y->at(k)-pending_info.y)*(getArray(i,j)->y->at(k)-pending_info.y));
                    if(sqrt((getArray(i,j)->x->at(k)-pending_info.x)*(getArray(i,j)->x->at(k)-pending_info.x)
                           +(getArray(i,j)->y->at(k)-pending_info.y)*(getArray(i,j)->y->at(k)-pending_info.y))<safe_distance)
                    {
                        if(getArray(i,j)->id->at(k)!=pending_info.id)
                        {
                            queue.append(getArray(i,j)->id->at(k));
                            //qDebug()<<"check:"<<pending_info.id<<getArray(i,j)->id->at(k);
                        }
                    }
                }
            }
        }
    }
    //qDebug()<<"before queue:"<<queue;
    if(!queue.isEmpty())
    {
        queue.append(pending_info.id);
        emitRegSignals();
    }
    else if(car_list.indexOf(pending_info.id)!=-1)
    {
        if(car_state[car_list.indexOf(pending_info.id)]==STOP)
        {
            emit advSignal(pending_info.id);
        }
    }
    isbusy=false;
    //qDebug()<<"after queue:"<<queue;
}

void detector::emitRegSignals()
{
    std::sort(queue.begin(),queue.end());
    emit regSignal();
}
