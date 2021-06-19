#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>
#include <QtMath>
#include "public.h"

#define formatXpos(x) (x/safe_distance+1)                           //将x坐标转化为栅格x坐标
#define formatYpos(y) (y/safe_distance+1)                           //将y坐标转化为栅格y坐标

#define getArray(x,y)        (map+y*array_size.xsize+x)             //输入栅格的x、y坐标返回该栅格指针
#define getArrayFromPos(x,y) getArray(formatXpos(x),formatYpos(y))  //输入实际x、y坐标返回该坐标相应的栅格指针

struct pos_info
{
    QList<int> *x;
    QList<int> *y;
    QList<quint8> *id;
};//栅格中的车辆x、y、id信息

class detector : public QThread
{
    Q_OBJECT
signals:
    void stopSignal(QList<quint8> q);
    void advSignal(quint8 id);
    void regSignal(QMap<quint8,QPoint> q);

public:
    detector();
    QList<quint8> queue;            //待调控车辆队列
    QMap<int,bool> car_state;       //各车辆状态
    QMap<int,int> car_dir;          //各车辆方向
    QMap<int,QPoint> car_pos;       //各车辆位置
    bool isbusy;                    //线程状态
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos,quint8 id);
    void setPosInfo(int xpos,int ypos,quint8 id);
    void setCheckPoint(int xpos,int ypos,quint8 id);
    void regCars(QList<quint8> q);

private:
    pos_info *map;                  //储存地图的内存
    QPoint *map_size=new QPoint();  //地图尺寸
    struct array_size
    {
        int xsize;
        int ysize;
    }array_size;                    //栅格尺寸
    struct untreated_info
    {
        int x;
        int y;
        quint8 id;
    }pending_info;                  //待处理信息
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
