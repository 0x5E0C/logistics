#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qcustomplot.h"
#include "detector.h"
#include "process.h"
#include "public.h"

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
    bool serial_state;
    QString  serial_choice;
    QSerialPort *serialport = new QSerialPort();
    QList<quint8> task_id_list;
    quint8 max_task_id;
    QList<task> task_list;
    detector *collision;
    bool reply_flag,autosend_flag;
    quint8 send_buffer[PACKET_LENGTH];
    void widgetInit();
    bool openSerialport();
    void closeSerialport();
    quint8 addLine(quint8 id);

private slots:
   void changeSerialState();
   void searchSerialport();
   void recordSerialChoice(int choice);
   void readSerialport();
   void emitStopSignal(quint8 line_id1,quint8 line_id2);
   void addTask();
   void deleteTask();
   void changeTask(int i);
   void updateMapSize();
   void postTaskInfo();
   void writeSerialport();
   void recordEditedTask();
   void startThread();
   void addGraph(int id,QColor color);
};
#endif // MAINWINDOW_H
