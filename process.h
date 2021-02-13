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
    void addGraph(int id,QColor color);

public:
    process(QCustomPlot *w,detector *c);
    typedef struct _line
    {
        QVector<double> x;
        QVector<double> y;
    }line;
    bool isbusy;
    void setCacheData(QByteArray d);
    void setReplyFlag(bool *flag);
    quint8 getCarIdInIdList(quint8 id);

private:
    QByteArray cachedata;
    QByteArray rec_data;
    QCustomPlot *plot;
    detector *collision;
    QList<quint8> id_list;
    quint8 max_line_id;
    QList<line> line_list;
    bool *reply_flag;
    quint8 addLine(quint8 id);
    int getDataIndex(QByteArray data);

protected:
    void run();
};


#endif // PROCESS_H
