#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMetaType>
#include "qcustomplot.h"
#include "detector.h"
#include "process.h"
#include "public.h"
#include "colorlib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    typedef struct _task
    {
        int id; //车辆id
        int x;  //目的地x坐标
        int y;  //目的地y坐标
    }task;

private:
    Ui::MainWindow *ui;
    QTimer *timer = new QTimer(this);                   //用于定时搜索串口的定时器
    QTimer *send_timer = new QTimer(this);              //用于定时发送数据的定时器
    process *processor;                                 //数据包处理器（新线程）
    QList<QString> serialport_list;                     //可用串口列表
    bool zigbee_serial_state,uwb_serial_state;          //zigbee（uwb）所在串口打开状态
    QString  zigbee_serial_choice,uwb_serial_choice;    //zigbee（uwb）选择打开的串口的名字
    QSerialPort *zigbee_serialport = new QSerialPort(); //初始化zigbee串口
    QSerialPort *uwb_serialport = new QSerialPort();    //初始化uwb串口
    QList<quint8> task_id_list;                         //任务id列表
    quint8 max_task_id;                                 //最大的任务id
    QList<task> task_list;                              //任务列表
    detector *collision;                                //碰撞检测器
    bool reply_flag,autosend_flag;                      //是否收到下位机回复标志位，自动发送标志位
    quint8 zigbee_send_buffer[PACKET_LENGTH],uwb_send_buffer[11];   //zigbee（uwb）发送缓存区
    QList<quint8> id_list;                              //车辆id列表
    QVector<QVector<QCPCurveData>> trajectorydata_list; //轨迹数据点列表
    QVector<QCPCurve*> trajectory_list;                 //轨迹列表
    int map_width,map_lenth;                            //地图宽（长）
    void widgetInit();
    bool openZigbeeSerialport();
    bool openUWBSerialport();
    void closeZigbeeSerialport();
    void closeUWBSerialport();
    quint8 addLine(quint8 id);
    quint16 crc_chk(quint8 *data,quint8 length);

private slots:
   void changeZigbeeSerialState();
   void changeUWBSerialState();
   void searchSerialport();
   void recordZigbeeSerialChoice(int choice);
   void recordUWBSerialChoice(int choice);
   void readZigbeeSerialport();
   void readUWBSerialport();
   void emitRegSignals(QMap<quint8,QPoint> q);
   void emitStopSignal(QList<quint8> q);
   void emitAdvSignal(quint8 id);
   void addTask();
   void deleteTask();
   void changeTask(int i);
   void updateMapSize();
   void postTaskInfo();
   void writeZigbeeSerialport();
   void writeUWBSerialport();
   void recordEditedTask();
   void addGraph();
};
#endif // MAINWINDOW_H
