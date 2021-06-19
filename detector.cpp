#include "detector.h"

detector::detector()
{
    isbusy=false;
}

void detector::run()
{
    startDetector(formatXpos(pending_info.x),formatXpos(pending_info.y));
}

/*
函数名：createMap
参数：地图的x长度，地图的y长度
返回值：无
功能：在内存中开辟一片空间用于储存每辆车的位置
*/
void detector::createMap(int xsize,int ysize)
{
    map_size->setX(xsize);                              //设置地图x轴大小
    map_size->setY(ysize);                              //设置地图y轴大小
    array_size.xsize=(xsize%safe_distance==0)?(xsize/safe_distance):(xsize/safe_distance+1);    //将x轴划分为多个安全距离，有余数补多
    array_size.ysize=(ysize%safe_distance==0)?(ysize/safe_distance):(ysize/safe_distance+1);    //将y轴划分为多个安全距离，有余数补多
    array_size.xsize+=2;                                                                        //添加边缘填充
    array_size.ysize+=2;
    map=(pos_info*)malloc(array_size.xsize*array_size.ysize*sizeof(pos_info));                  //申请地图内存
    memset(map,0,array_size.xsize*array_size.ysize*sizeof(pos_info));                           //初始化内存
    for(int i=0;i<array_size.xsize*array_size.ysize;i++)
    {
        (map+i)->x=new QList<int>;                                                              //实例化每块栅格中的信息
        (map+i)->y=new QList<int>;
        (map+i)->id=new QList<quint8>;
    }
}

/*
函数名：clearPosInfo
参数：在地图中的x坐标、y坐标、车辆id
返回值：无
功能：清除指定位置中储存的id
*/
void detector::clearPosInfo(int xpos,int ypos,quint8 id)
{
    int index=getArrayFromPos(xpos,ypos)->id->indexOf(id);  //找出该id在栅格中的id列表中的位置
    if(index==-1)
    {
        return;
    }
    getArrayFromPos(xpos,ypos)->x->removeAt(index);         //清除相关信息
    getArrayFromPos(xpos,ypos)->y->removeAt(index);
    getArrayFromPos(xpos,ypos)->id->removeAll(id);
}

/*
函数名：setPosInfo
参数：在地图中的x坐标、y坐标、车辆id
返回值：无
功能：在指定位置中储存id
*/
void detector::setPosInfo(int xpos,int ypos,quint8 id)
{
    getArrayFromPos(xpos,ypos)->x->append(xpos);            //在该位置的栅格中存入相关信息
    getArrayFromPos(xpos,ypos)->y->append(ypos);
    getArrayFromPos(xpos,ypos)->id->append(id);
}

/*
函数名：setCheckPoint
参数：在地图中的x坐标、y坐标、车辆id
返回值：无
功能：设置碰撞检测点
*/
void detector::setCheckPoint(int xpos,int ypos,quint8 id)
{
    pending_info.x=xpos;                                    //将信息存入待处理变量中
    pending_info.y=ypos;
    pending_info.id=id;
    setPosInfo(xpos,ypos,id);                               //存入该点信息
}

/*
函数名：startDetector
参数：内存中储存地图的二维数组的x、y位置
返回值：无
功能：开始检测碰撞
*/
void detector::startDetector(int array_x,int array_y)
{
    isbusy=true;                                        //线程标记为忙
    queue.clear();                                      //清除调控队列
    for(int i=array_x-1;i<=array_x+1;i++)
    {
        for(int j=array_y-1;j<=array_y+1;j++)           //遍历该车辆所在栅格四周的八个栅格
        {
            if(!getArray(i,j)->id->isEmpty())           //若有栅格中存有车辆信息
            {
                for(int k=0;k<getArray(i,j)->id->size();k++)
                {
                    if(sqrt((getArray(i,j)->x->at(k)-pending_info.x)*(getArray(i,j)->x->at(k)-pending_info.x)
                           +(getArray(i,j)->y->at(k)-pending_info.y)*(getArray(i,j)->y->at(k)-pending_info.y))<safe_distance)//计算距离
                    {
                        if(getArray(i,j)->id->at(k)!=pending_info.id)
                        {
                            queue.append(getArray(i,j)->id->at(k)); //距离小于安全距离把该车辆id存入待调控队列
                        }
                    }
                }
            }
        }
    }
    if(!queue.isEmpty())                                            //队列非空
    {
        queue.append(pending_info.id);                              //将自身id存入队列
        std::sort(queue.begin(),queue.end(),std::greater<int>());   //id排序
        emit stopSignal(queue);                                     //停止相关车辆前进
    }
    else if(car_state.contains(pending_info.id))                    //队列为空且记录了车辆状态
    {
        if(car_state[pending_info.id]==STOP)                        //若车辆状态是停止前进的
        {
            emit advSignal(pending_info.id);                        //继续前进
        }
    }
    isbusy=false;                                                   //线程标记为空闲
}

/*
函数名：regCars
参数：需要调控的车辆列表
返回值：无
功能：计算避让点和调控车辆
*/
void detector::regCars(QList<quint8> q)
{
    quint8 first_id=q.last();                           //将优先级最高的车辆取出
    q.removeLast();                                     //清除队列中最高优先级的车辆
    QList<quint8> yield_id=q;                           //把其他车辆id取出
    QMap<quint8,QPoint> reg_q;                          //调控的车辆
    int xpos,ypos;
    if(!car_dir.contains(first_id))                     //最高优先级的方向信息缺失，无法调控
    {
        return;
    }
    for(int i=0;i<yield_id.size();i++)
    {
        int id=yield_id.at(i);                          //检查其他车辆的方向信息，若缺失则退出
        if(!car_dir.contains(id))
        {
            return;
        }
        if(car_dir[first_id]==X_DIR)                    //最高优先级车辆正跑x方向
        {
            xpos=car_pos[id].x();                       //保持避让x坐标与当前一致
            ypos=car_pos[first_id].y()+getYAvailableDir(car_pos[id])*avoid_coe*safe_distance;//计算避让点y坐标
        }
        else if(car_dir[first_id]==Y_DIR)
        {
            xpos=car_pos[first_id].x()+getXAvailableDir(car_pos[id])*avoid_coe*safe_distance;
            ypos=car_pos[id].y();
        }
        else
        {
            return;
        }
        QPoint pos=QPoint(xpos,ypos);                   //避让点坐标
        reg_q.insert(id,pos);                           //添加调控信息
    }
    emit regSignal(reg_q);                              //发送调控命令
}

/*
函数名：getXAvailableDir
参数：在地图中的位置
返回值：1：X轴正方向
      -1：X轴负方向
       0：异常情况
功能：查询X方向可用于避让的空间
*/
int detector::getXAvailableDir(QPoint pos)
{
    int xpos=pos.x();                                   //获取x、y坐标
    int ypos=pos.y();
    int array_x=formatXpos(xpos);                       //将位置转化为栅格位置
    int array_y=formatYpos(ypos);
    if(getArray(array_x+1,array_y)->id->isEmpty() && getArray(array_x-1,array_y)->id->isEmpty())//若x方向相邻栅格都有空闲空间
    {
        return -(xpos-map_size->x()/2)/qAbs(xpos-map_size->x()/2);//返回靠地图中心的方向
    }
    else if(getArray(array_x+1,array_y)->id->isEmpty()) //只有右栅格有空闲
    {
        return 1;                                       //返回右方向
    }
    else if(getArray(array_x-1,array_y)->id->isEmpty()) //只有左栅格有空闲
    {
        return -1;                                      //返回左方向
    }
    return 0;
}

/*
函数名：getYAvailableDir
参数：在地图中的位置
返回值：1：Y轴正方向
      -1：Y轴负方向
       0：异常情况
功能：查询Y方向可用于避让的空间
*/
int detector::getYAvailableDir(QPoint pos)//同getXAvailableDir
{
    int xpos=pos.x();
    int ypos=pos.y();
    int array_x=formatXpos(xpos);
    int array_y=formatYpos(ypos);
    if(getArray(array_x,array_y+1)->id->isEmpty() && getArray(array_x,array_y-1)->id->isEmpty())
    {
        return -(ypos-map_size->y()/2)/qAbs(ypos-map_size->y()/2);
    }
    else if(getArray(array_x,array_y+1)->id->isEmpty())
    {
        return 1;
    }
    else if(getArray(array_x,array_y+1)->id->isEmpty())
    {
        return -1;
    }
    return 0;
}
