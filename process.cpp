#include "process.h"

process::process(detector *c)
{
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
        int id=rec_buffer[2];
        int dir=rec_buffer[4];
        if(rec_buffer[3]==REPLY_CMD)
        {
            qDebug()<<"reply";
            qDebug()<<id<<dir;
            *reply_flag=true;
            if(collision->car_dir.contains(id))
            {
                collision->car_dir[id]=dir;
            }
            else
            {
                collision->car_dir.insert(id,dir);
            }
        }
    }
    isbusy=false;
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
