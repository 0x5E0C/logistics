#include "process.h"

process::process(QCustomPlot *w,detector *c)
{
    plot=w;
    collision=c;
    max_line_id=0;
    isbusy=false;
}

void process::setCacheData(QByteArray d)
{
    cachedata.append(d);
    qDebug()<<cachedata.size();
}

void process::setReplyFlag(bool *flag)
{
    reply_flag=flag;
}

quint8 process::getCarIdInIdList(quint8 id)
{
    return id_list[id];
}

void process::run()
{
    quint8 line_id;
    quint16 sum=0;
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
        for(int i=0;i<6;i++)
        {
            sum+=rec_buffer[i];
        }
        if(sum!=(rec_buffer[6]<<8|rec_buffer[7]))
        {
            continue;
        }
        qint16 x=rec_buffer[2]<<8|rec_buffer[3];
        qint16 y=rec_buffer[4]<<8|rec_buffer[5];
        qDebug()<<x<<y;
        if(rec_buffer[1]==REPLY_CMD)
        {
            *reply_flag=true;
        }
        else if(rec_buffer[1]==SENDPOS_CMD)
        {
            if(id_list.indexOf(rec_buffer[0])==-1)
            {
                line_id=addLine(rec_buffer[0]);
                line templine;
                templine.x.append(x);
                templine.y.append(y);
                line_list.append(templine);
            }
            else
            {
                line_id=id_list.indexOf(rec_buffer[0]);
                collision->clearPosInfo(line_list[line_id].x.last(),line_list[line_id].y.last());
                line_list[line_id].x.append(x);
                line_list[line_id].y.append(y);
                plot->graph(line_id)->setData(line_list[line_id].x,line_list[line_id].y,true);
            }
            collision->setCheckpoint(x,y,line_id);
            emit processed();
        }
    }
    isbusy=false;
}


quint8 process::addLine(quint8 id)
{
    quint8 temp_id=max_line_id;
    id_list.append(id);
    max_line_id++;
    int b=temp_id%6;
    int g=temp_id/6;
    int r=temp_id/36;
    emit addGraph(temp_id,QColor(r*51,g*51,b*51));
    return temp_id;
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
