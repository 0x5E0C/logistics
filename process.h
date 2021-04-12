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
    bool isbusy;
    void setCacheData(QByteArray d);
    void setReplyFlag(bool *flag);

private:
    QByteArray cachedata;
    QByteArray rec_data;
    bool *reply_flag;
    detector *collision;
    void processData();
    int getDataIndex(QByteArray data);

protected:
    void run();
};


#endif // PROCESS_H
