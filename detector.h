#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>
#include "public.h"

#define formatXpos(x) (x/safe_distance+1)
#define formatYpos(y) (y/safe_distance+1)

#define getArray(x,y)        (map+y*array_size.xsize+x)
#define getArrayFromPos(x,y) getArray(formatXpos(x),formatYpos(y))

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
    void advSignal(quint8 id);
    void regSignal();

public:
    detector();
    QList<quint8> queue;
    QList<quint8> car_list;
    QList<bool> car_state;
    bool isbusy;
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos,quint8 id);
    void setPosInfo(int xpos,int ypos,quint8 id);
    void setCheckPoint(int xpos,int ypos,quint8 id);

private:
    const int safe_distance=100;
    pos_info *map;
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

protected:
    void run();
};

#endif // DETECTOR_H
