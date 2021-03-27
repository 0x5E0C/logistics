#include "process.h"

process::process(QCustomPlot *w,detector *c)
{
    plot=w;
    collision=c;
    isbusy=false;
}

void process::setCacheData(QByteArray d)
{
    cachedata.append(d);
}

void process::setReplyFlag(bool *flag)
{
    reply_flag=flag;
}

void process::run()
{
    quint8 trajectory_index;
    quint16 sum;
    int index;
    isbusy=true;
    while((cachedata.size()+rec_data.size())>=PACKET_LENGTH)
    {
        rec_data.append(cachedata);
        cachedata.clear();
        index=getDataIndex(rec_data);
        if(index==-1)
        {
            rec_data.clear();
            continue;
        }
        QByteArray tempbuffer=rec_data.mid(index,PACKET_LENGTH);
        quint8 *rec_buffer=(quint8*)tempbuffer.data();
        rec_data=rec_data.mid(index+1);
        sum=0;
        for(int i=0;i<8;i++)
        {
            sum+=rec_buffer[i];
        }
        if(sum!=(rec_buffer[8]<<8|rec_buffer[9]))
        {
            qDebug()<<tempbuffer.toHex()<<sum<<(rec_buffer[8]<<8|rec_buffer[9])<<rec_buffer[8]<<rec_buffer[9];
            continue;
        }
        quint8 id=rec_buffer[2];
        qint16 x=rec_buffer[4]<<8|rec_buffer[5];
        qint16 y=rec_buffer[6]<<8|rec_buffer[7];
        if(rec_buffer[3]==REPLY_CMD)
        {
            qDebug()<<"reply";
            *reply_flag=true;
        }
        else if(rec_buffer[3]==SENDPOS_CMD)
        {
            if(id_list.indexOf(id)==-1)
            {
                addLine(id);
                QVector<QCPCurveData> trajectory_data;
                trajectory_data.append(QCPCurveData(0,x,y));
                trajectorydata_list.append(trajectory_data);
            }
            else
            {
                trajectory_index=id_list.indexOf(id);
                collision->clearPosInfo(trajectorydata_list[trajectory_index].last().key,trajectorydata_list[trajectory_index].last().value,id);
                trajectorydata_list[trajectory_index].append(QCPCurveData(trajectorydata_list[trajectory_index].size(),x,y));
                trajectory_list[trajectory_index]->data()->set(trajectorydata_list[trajectory_index],true);
            }
            collision->setCheckPoint(x,y,id);
            emit processed();
        }
        else if(rec_buffer[3]==WAIT_CMD)
        {
            collision->setWaitPoint(x,y,id);
            emit processed();
        }
    }
    isbusy=false;
}

void process::addLine(quint8 id)
{
    id_list.append(id);
    emit addGraph();
}

int process::getDataIndex(QByteArray data)
{
    int index=data.indexOf(0x5E);
    if(index!=(data.size()-1))
    {
        if(data.at(index+1)==0x0C)
        {
            return index;
        }
    }
    return -1;
}
