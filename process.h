#ifndef PROCESS_H
#define PROCESS_H

#include <QThread>
#include <QDebug>
#include "public.h"
#include "qcustomplot.h"
#include "detector.h"

class process : public QThread
{
    Q_OBJECT

public:
    process(detector *c);
    bool isbusy;                        //线程状态
    void setCacheData(QByteArray d);
    void setReplyFlag(bool *flag);

private:
    QByteArray cachedata;               //数据缓冲区
    QByteArray rec_data;                //数据待处理区
    bool *reply_flag;                   //从mainwindows传入的回复标记位的指针
    detector *collision;                //碰撞检测器
    void processData();
    int getDataIndex(QByteArray data);

protected:
    void run();
};


#endif // PROCESS_H
