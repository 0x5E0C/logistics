#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>
#include <QtMath>
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
    void regSignal(QMap<quint8,QPoint> q);

public:
    detector();
    QList<quint8> queue;
    QMap<int,bool> car_state;
    QMap<int,int> car_dir;
    QMap<int,QPoint> car_pos;
    bool isbusy;
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos,quint8 id);
    void setPosInfo(int xpos,int ypos,quint8 id);
    void setCheckPoint(int xpos,int ypos,quint8 id);
    void regCars(QList<quint8> q,bool *flag);

private:
    pos_info *map;
    QPoint *map_size=new QPoint();
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
    enum DIR
    {
        UNDEF,
        LEFT,
        TOP,
        RIGHT,
        BOTTOM
    } dir;
    void startDetector(int array_x,int array_y);
    int getXAvailableDir(QPoint pos);
    int getYAvailableDir(QPoint pos);

protected:
    void run();
};

#endif // DETECTOR_H
