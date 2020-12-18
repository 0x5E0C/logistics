#ifndef DETECTOR_H
#define DETECTOR_H

#include <QThread>
#include <QDebug>

#define formatXpos(x) (x/safe_distance+1)
#define formatYpos(y) (y/safe_distance+1)

#define getArray(x,y)        (map+y*array_size.xsize+x)
#define getArrayFromPos(x,y) getArray(formatXpos(x),formatYpos(y))

const int safe_distance=5;
typedef struct pos_info
{
    int x;
    int y;
    bool exist;
    quint8 lineid;
} pos_info;

class detector : public QThread
{
    Q_OBJECT
signals:
    void stopSignal(quint8 line_id1,quint8 line_id2);

public:
    detector();
    void createMap(int xsize,int ysize);
    void clearPosInfo(int xpos,int ypos);
    void setPosInfo(int xpos,int ypos,quint8 lineid);
    void setCheckpoint(int xpos,int ypos,quint8 lineid);

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

protected:
    void run();
};

#endif // DETECTOR_H
