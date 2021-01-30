#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qcustomplot.h"
#include "detector.h"

#define OPENSTATE   0
#define CLOSESTATE  1

#define BASETIME    500

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    typedef struct line
    {
        QVector<double> x;
        QVector<double> y;
    }line;
    typedef struct task
    {
        int id;
        int x;
        int y;
    }task;

private:
    Ui::MainWindow *ui;
    QTimer *timer = new QTimer(this);
    QList<QString> serialport_list;
    bool serial_state;
    QString  serial_choice;
    QSerialPort *serialport = new QSerialPort();
    QList<quint8> id_list,task_id_list;
    quint8 max_line_id,max_task_id;
    QList<line> line_list;
    QList<task> task_list;
    detector *collision = new detector();
    void widgetInit();
    bool openSerialport();
    void closeSerialport();
    quint8 addLine(quint8 id);
    void writeSerialport(quint8 *data,int count);

private slots:
   void changeSerialState();
   void searchSerialport();
   void recordSerialChoice(int choice);
   void readSerialport();
   void emitStopSignal(quint8 line_id1,quint8 line_id2);
   void addTask();
   void deleteTask();
   void changeTask(int i);
   void syncCoordinateAxis();
   void postTaskInfo();
};
#endif // MAINWINDOW_H
