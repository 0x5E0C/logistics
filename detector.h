#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>
#include "public.h"

#define formatXpos(x) (x/safe_distance+1)
#define formatYpos(y) (y/safe_distance+1)

#define getArray(x,y)        (map+y*array_size.xsize+x)
#define getArrayFromPos(x,y) getArray(formatXpos(x),formatYpos(y))

const int safe_distance=65;
typedef struct pos_info
{
    int x;
    int y;
    bool exist;
    int index;
} pos_info;

class detector : public QThread
{
    Q_OBJECT
signals:
    void stopSignal(quint8 line_id1,quint8 line_id2);

public:
    detector();
    QList<QList<quint8>> queue;
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos,quint8 id);
    void setPosInfo(int xpos,int ypos,quint8 id);
    void setCheckpoint(int xpos,int ypos,quint8 id);

private:
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

protected:
    void run();
};

#endif // DETECTOR_H
