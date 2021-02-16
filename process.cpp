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
    isbusy=true;
    while((cachedata.size()+rec_data.size())>=PACKET_LENGTH)
    {
        rec_data.append(cachedata);
        cachedata.clear();
        int index=getDataIndex(rec_data);
        if(index==-1)
        {
            rec_data.clear();
            continue;
        }
        rec_data=(index>0)?rec_data.mid(index-1):rec_data;
        quint8 *rec_buffer=(quint8*)rec_data.data();
        rec_data=rec_data.mid(index+1);
        sum=0;
        for(int i=0;i<6;i++)
        {
            sum+=rec_buffer[i];
        }
        if(sum!=(rec_buffer[6]<<8|rec_buffer[7]))
        {
            for(int i=0;i<6;i++)
            {
                qDebug()<<rec_buffer[i];
            }
            qDebug()<<rec_buffer[6]<<rec_buffer[7]<<sum<<(rec_buffer[6]<<8|rec_buffer[7]);
            continue;
        }
        qint16 x=rec_buffer[2]<<8|rec_buffer[3];
        qint16 y=rec_buffer[4]<<8|rec_buffer[5];
        if(rec_buffer[1]==REPLY_CMD)
        {
            *reply_flag=true;
        }
        else if(rec_buffer[1]==SENDPOS_CMD)
        {
            if(id_list.indexOf(rec_buffer[0])==-1)
            {
                addLine(rec_buffer[0]);
                QVector<QCPCurveData> trajectory_data;
                trajectory_data.append(QCPCurveData(0,x,y));
                trajectorydata_list.append(trajectory_data);
            }
            else
            {
                trajectory_index=id_list.indexOf(rec_buffer[0]);
                collision->clearPosInfo(trajectorydata_list[trajectory_index].last().key,trajectorydata_list[trajectory_index].last().value);
                trajectorydata_list[trajectory_index].append(QCPCurveData(trajectorydata_list[trajectory_index].size(),x,y));
                trajectory_list[trajectory_index]->data()->set(trajectorydata_list[trajectory_index],true);
            }
            collision->setCheckpoint(x,y,rec_buffer[0]);
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
    int reply_index,pos_index;
    reply_index=data.indexOf(REPLY_CMD);
    pos_index=data.indexOf(SENDPOS_CMD);
    if(reply_index==-1)
    {
        return pos_index;
    }
    else if(pos_index==-1)
    {
        return reply_index;
    }
    return (reply_index<pos_index)?reply_index:pos_index;
}
