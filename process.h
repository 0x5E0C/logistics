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
signals:
    void processed();
    void addGraph();

public:
    process(QCustomPlot *w,detector *c);
    bool isbusy;
    QVector<QVector<QCPCurveData>> trajectorydata_list;
    QVector<QCPCurve*> trajectory_list;
    void setCacheData(QByteArray d);
    void setReplyFlag(bool *flag);

private:
    QByteArray cachedata;
    QByteArray rec_data;
    QCustomPlot *plot;
    detector *collision;
    QList<quint8> id_list;
    bool *reply_flag;
    void addLine(quint8 id);
    int getDataIndex(QByteArray data);

protected:
    void run();
};


#endif // PROCESS_H
