#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widgetInit();
    connect(timer,SIGNAL(timeout()),this,SLOT(searchSerialport()));
    connect(ui->openButton,SIGNAL(clicked()),this,SLOT(changeSerialState()));
    connect(ui->serialportBox,SIGNAL(activated(int)),this,SLOT(recordSerialChoice(int)));
    connect(ui->newTaskButton,SIGNAL(clicked()),this,SLOT(addTask()));
    connect(ui->deleteTaskButton,SIGNAL(clicked()),this,SLOT(deleteTask()));
    connect(ui->postTaskButton,SIGNAL(clicked()),this,SLOT(postTaskInfo()));
    connect(ui->syncButton,SIGNAL(clicked()),this,SLOT(syncCoordinateAxis()));
    connect(ui->taskBox,SIGNAL(activated(int)),this,SLOT(changeTask(int)));
    connect(serialport,SIGNAL(readyRead()),this,SLOT(readSerialport()));
    connect(collision,SIGNAL(stopSignal(quint8,quint8)),this,SLOT(emitStopSignal(quint8,quint8)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::widgetInit()
{
    serial_state=CLOSESTATE;
    serial_choice="";
    max_line_id=0;
    max_task_id=0;
    ui->openButton->setIcon(QPixmap(":/state/close.png"));
    ui->openButton->setText("打开串口");
    ui->widget->setInteractions(QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    timer->start(BASETIME);
}

void MainWindow::changeSerialState()
{
    if(serial_state==CLOSESTATE && openSerialport())
    {
        ui->openButton->setIcon(QPixmap(":/state/open.png"));
        ui->openButton->setText("关闭串口");
        serial_state=OPENSTATE;
    }
    else if(serial_state==OPENSTATE)
    {
        ui->openButton->setIcon(QPixmap(":/state/close.png"));
        ui->openButton->setText("打开串口");
        serial_state=CLOSESTATE;
        closeSerialport();
    }
}

void MainWindow::recordSerialChoice(int choice)
{
    serial_choice=ui->serialportBox->itemText(choice);
    if(serial_state==OPENSTATE)
    {
        closeSerialport();
        openSerialport();
    }
}

void MainWindow::searchSerialport()
{
    QList<QString> temp_list;
    if(QSerialPortInfo::availablePorts().size()!=0)
    {
        if(ui->serialportBox->itemText(0)=="")
        {
            ui->serialportBox->removeItem(0);
        }
        if(serialport_list.isEmpty())
        {
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
            {
                serialport_list.append(info.portName());
            }
            for(int i=0;i<serialport_list.size();i++)
            {
                ui->serialportBox->addItem(serialport_list[i]);
            }
        }
        else
        {
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
            {
                temp_list.append(info.portName());
            }
            if(temp_list!=serialport_list)
            {
                ui->serialportBox->clear();
                for(int i=0;i<temp_list.size();i++)
                {
                    ui->serialportBox->addItem(temp_list[i]);
                }
                serialport_list=temp_list;
            }
        }
        ui->serialportBox->setCurrentText(serial_choice);
    }
    else
    {
        ui->serialportBox->clear();
        ui->serialportBox->addItem("");
        serialport_list.clear();
    }
}

bool MainWindow::openSerialport()
{
    serialport->setPortName(ui->serialportBox->currentText());
    serialport->setBaudRate(ui->baudrateBox->currentText().toInt());
    serialport->setStopBits(QSerialPort::OneStop);
    serialport->setFlowControl(QSerialPort::NoFlowControl);
    serialport->setDataBits(QSerialPort::Data8);
    serialport->setParity(QSerialPort::NoParity);
    QSerialPortInfo serial_info(ui->serialportBox->currentText());
    if(serial_info.isBusy()==true)
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!串口已被占用!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        serial_state=OPENSTATE;
        changeSerialState();
        return false;
    }
    if(!serialport->open(QIODevice::ReadWrite))
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return false;
    }
    return true;
}

void MainWindow::closeSerialport()
{
    serialport->close();
}

void MainWindow::writeSerialport(quint8 *data,int count)
{
    QByteArray buffer;
    buffer.resize(count);
    memcpy(buffer.data(),data,count);
    serialport->write(buffer);
}

void MainWindow::readSerialport()
{
    quint8 *rec_buffer=(quint8*)serialport->readAll().data();
    int sum=0;
    quint8 line_id;
    for(int i=0;i<=6;i++)
    {
        sum+=rec_buffer[i];
    }
    if(sum!=(rec_buffer[7]<<8|rec_buffer[8]))
    {
        return;
    }
    int x=(rec_buffer[1]==0x00?rec_buffer[2]<<8|rec_buffer[3]:-(rec_buffer[2]<<8|rec_buffer[3]));
    int y=(rec_buffer[4]==0x00?rec_buffer[5]<<8|rec_buffer[6]:-(rec_buffer[5]<<8|rec_buffer[6]));
    if(rec_buffer[0]==0x00)
    {
        ui->widget->xAxis->setRange(0,x);//设置x轴长度
        ui->widget->yAxis->setRange(0,y);//设置y轴长度
        ui->widget->replot();
        collision->createMap(x,y);
    }
    else
    {
        if(id_list.indexOf(rec_buffer[0])==-1)
        {
            line_id=addLine(rec_buffer[0]);
            line templine;
            templine.x.append(x);
            templine.y.append(y);
            line_list.append(templine);
        }
        else
        {
            line_id=id_list.indexOf(rec_buffer[0]);
            collision->clearPosInfo(line_list[line_id].x.last(),line_list[line_id].y.last());
            line_list[line_id].x.append(x);
            line_list[line_id].y.append(y);
            ui->widget->graph(line_id)->setData(line_list[line_id].x,line_list[line_id].y,true);
        }
        collision->setCheckpoint(x,y,line_id);
        collision->start();
        ui->widget->replot();
    }
}

void MainWindow::emitStopSignal(quint8 line_id1,quint8 line_id2)
{
    quint8 id1=id_list[line_id1];
    quint8 id2=id_list[line_id2];
    quint8 sumh=(id1+id2)/255;
    quint8 suml=(id1+id2)%255;
    quint8 buffer[4]={id1,id2,sumh,suml};
    writeSerialport(buffer,sizeof(buffer)/sizeof(quint8));
}

quint8 MainWindow::addLine(quint8 id)
{
    quint8 temp_id=max_line_id;
    id_list.append(id);
    max_line_id++;
    ui->widget->addGraph();
    int b=temp_id%6;
    int g=temp_id/6;
    int r=temp_id/36;
    ui->widget->graph(temp_id)->setPen(QPen(QColor(r*51,g*51,b*51)));
    return temp_id;
}

void MainWindow::addTask()
{
    if(ui->editEndX->text().isEmpty() && ui->editEndY->text().isEmpty())
    {
        QString dlgTitle="错误";
        QString strInfo="请先填写坐标信息!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return;
    }
    task newtask;
    newtask.id=ui->editID->text().toInt();
    newtask.x=ui->editEndX->text().toInt();
    newtask.y=ui->editEndY->text().toInt();
    ui->editID->clear();
    ui->editEndX->clear();
    ui->editEndY->clear();
    task_list.append(newtask);
    task_id_list.append(max_task_id);
    max_task_id++;
    ui->taskBox->addItem(QString::number(max_task_id));
    ui->taskBox->setCurrentText(QString::number(max_task_id));
    ui->deleteTaskButton->setEnabled(true);
}

void MainWindow::deleteTask()
{
    int index=ui->taskBox->currentText().toInt();
    ui->taskBox->removeItem(ui->taskBox->currentIndex());
    task_list.removeAt(task_id_list.indexOf(index));
    task_id_list.removeAt(task_id_list.indexOf(index));
    if(task_id_list.size()==1)
    {
        ui->deleteTaskButton->setEnabled(false);
    }
    ui->taskBox->setCurrentText(QString::number(task_id_list[0]));
    changeTask(0);
}

void MainWindow::changeTask(int i)
{
    Q_UNUSED(i);
    int index=task_id_list.indexOf(ui->taskBox->currentText().toInt());
    if(index==-1)
    {
        ui->editID->clear();
        ui->editEndX->clear();
        ui->editEndY->clear();
        return;
    }
    ui->editID->setText(QString::number(task_list[index].id));
    ui->editEndX->setText(QString::number(task_list[index].x));
    ui->editEndY->setText(QString::number(task_list[index].y));
}

void MainWindow::syncCoordinateAxis()
{
    quint8 buffer[9]={0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x05,0xFA};
    writeSerialport(buffer,sizeof(buffer)/sizeof(quint8));
}

void MainWindow::postTaskInfo()
{
    quint8 buffer[9];
    int sum=0;
    for(int i=0;i<task_list.size();i++)
    {
        buffer[0]=task_list[i].id;
        buffer[1]=(task_list[i].x>=0)?(0x00):(0x01);
        buffer[2]=task_list[i].x/256;
        buffer[3]=task_list[i].x%256;
        buffer[4]=(task_list[i].y>=0)?(0x00):(0x01);
        buffer[5]=task_list[i].y/256;
        buffer[6]=task_list[i].y%256;
        for(int j=0;j<9;j++)
        {
            sum+=buffer[j];
        }
        buffer[7]=sum/255;
        buffer[8]=sum%256;
    }
    writeSerialport(buffer,sizeof(buffer)/sizeof(quint8));
}
