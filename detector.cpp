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
    map_size->setX(xsize);
    map_size->setY(ysize);
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
    setPosInfo(xpos,ypos,id);
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
//                            qDebug()<<"check:"<<pending_info.id<<getArray(i,j)->id->at(k);
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
        std::sort(queue.begin(),queue.end(),std::greater<int>());
        emit stopSignal(queue);
    }
    else if(car_state.contains(pending_info.id))
    {
        if(car_state[pending_info.id]==STOP)
        {
            emit advSignal(pending_info.id);
        }
    }
    isbusy=false;
    //qDebug()<<"after queue:"<<queue;
}

void detector::regCars(QList<quint8> q,bool *flag)
{
    quint8 first_id=q.last();
    q.removeLast();
    QList<quint8> yield_id=q;
    QMap<quint8,QPoint> reg_q;
    int xpos,ypos;
    if(!car_dir.contains(first_id))
    {
        return;
    }
    for(int i=0;i<yield_id.size();i++)
    {
        int id=yield_id.at(i);
        if(!car_dir.contains(id))
        {
            return;
        }
        qDebug()<<__LINE__<<car_dir[first_id]<<car_dir[id];
        if(car_dir[first_id]==X_DIR)
        {
            qDebug()<<"im in xdir"<<car_dir[first_id]<<car_dir[id];
            qDebug()<<car_pos[id].x();
            qDebug()<<car_pos[first_id].y();
            qDebug()<<getYAvailableDir(car_pos[id]);
            xpos=car_pos[id].x();
            ypos=car_pos[first_id].y()+getYAvailableDir(car_pos[id])*avoid_coe*safe_distance;
        }
        else if(car_dir[first_id]==Y_DIR)
        {
            qDebug()<<"im in ydir"<<car_dir[first_id]<<car_dir[id];
            xpos=car_pos[first_id].x()+getXAvailableDir(car_pos[id])*avoid_coe*safe_distance;
            ypos=car_pos[id].y();
        }
        else
        {
            return;
        }
        QPoint pos=QPoint(xpos,ypos);
        qDebug()<<id<<pos;
        reg_q.insert(id,pos);
    }
    emit regSignal(reg_q);
}

int detector::getXAvailableDir(QPoint pos)
{
    int xpos=pos.x();
    int ypos=pos.y();
    int array_x=formatXpos(xpos);
    int array_y=formatYpos(ypos);
    if(getArray(array_x+1,array_y)->id->isEmpty() && getArray(array_x-1,array_y)->id->isEmpty())
    {
        return -(xpos-map_size->x()/2)/qAbs(xpos-map_size->x()/2);
    }
    else if(getArray(array_x+1,array_y)->id->isEmpty())
    {
        return 1;
    }
    else if(getArray(array_x-1,array_y)->id->isEmpty())
    {
        return -1;
    }
    return 0;
}

int detector::getYAvailableDir(QPoint pos)
{
    int xpos=pos.x();
    int ypos=pos.y();
    int array_x=formatXpos(xpos);
    int array_y=formatYpos(ypos);
    return -1;
    //qDebug()<<array_x<<array_y<<array_size.xsize<<array_size.ysize;
    //qDebug()<<map<<map+array_size.xsize*array_size.ysize*sizeof(pos_info);
    //qDebug()<<getArray(array_x,array_y+1)<<getArray(array_x,array_y-1);
    if(getArray(array_x,array_y+1)->id->isEmpty() && getArray(array_x,array_y-1)->id->isEmpty())
    {
        qDebug()<<(ypos-map_size->y()/2)/qAbs(ypos-map_size->y()/2);
//        qDebug()<<*getArray(array_x,array_y+1)->id<<*getArray(array_x,array_y-1)->id;
        return -(ypos-map_size->y()/2)/qAbs(ypos-map_size->y()/2);
    }
    else if(getArray(array_x,array_y+1)->id->isEmpty())
    {
//        qDebug()<<*getArray(array_x,array_y+1)->id<<*getArray(array_x,array_y-1)->id;
        return 1;
    }
    else if(getArray(array_x,array_y+1)->id->isEmpty())
    {
        return -1;
    }
    return 0;
}
