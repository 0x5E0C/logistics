#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>
#include "public.h"

#define formatXpos(x) (x/safe_distance+1)
#define formatYpos(y) (y/safe_distance+1)

#define getArray(x,y)        (map+y*array_size.xsize+x)
#define getArrayFromPos(x,y) getArray(formatXpos(x),formatYpos(y))

#define WAIT_FLAG  0
#define CHECK_FLAG 1

struct pos_info
{
    QList<int> *x;
    QList<int> *y;
    QList<quint8> *id;
};

class detector : public QThread
{
    Q_OBJECT
signals:
    void stopSignal(QList<quint8> q);
    void waitSignal(QList<quint8> q);
    void advSignal(quint8 id);


public:
    detector();
    QList<quint8> queue,wait_queue,stop_queue;
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos,quint8 id);
    void setPosInfo(int xpos,int ypos,quint8 id);
    void setCheckPoint(int xpos,int ypos,quint8 id);
    void setWaitPoint(int xpos,int ypos,quint8 id);
    void setTaskMode(int mode);

private:
    const int safe_distance=30;
    pos_info *map;
    bool task_flag;
    bool jam_flag;
    struct array_size
    {
        int xsize;
        int ysize;
    }array_size;
    struct untreated_info
    {
        int x;
        int y;
        quint8 id;
    }pending_info;
    void startDetector(int array_x,int array_y);
    void emitRegSignals();
    void emitAdvSignals();

protected:
    void run();
};

#endif // DETECTOR_H
