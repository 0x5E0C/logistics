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
        int id;
        int x;
        int y;
    }task;

private:
    Ui::MainWindow *ui;
    QTimer *timer = new QTimer(this);
    QTimer *send_timer = new QTimer(this);
    process *processor;
    QList<QString> serialport_list;
    bool zigbee_serial_state,uwb_serial_state;
    QString  zigbee_serial_choice,uwb_serial_choice;
    QSerialPort *zigbee_serialport = new QSerialPort();
    QSerialPort *uwb_serialport = new QSerialPort();
    QList<quint8> task_id_list;
    quint8 max_task_id;
    QList<task> task_list;
    detector *collision;
    bool reply_flag,autosend_flag;
    quint8 zigbee_send_buffer[PACKET_LENGTH],uwb_send_buffer[11];
    QList<quint8> id_list;
    QVector<QVector<QCPCurveData>> trajectorydata_list;
    QVector<QCPCurve*> trajectory_list;
    int map_width,map_lenth;
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
