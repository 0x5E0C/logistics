#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widgetInit();
    connect(timer,SIGNAL(timeout()),this,SLOT(searchSerialport()));
    connect(send_timer,SIGNAL(timeout()),this,SLOT(writeSerialport()));
    connect(ui->openButton,SIGNAL(clicked()),this,SLOT(changeSerialState()));
    connect(ui->serialportBox,SIGNAL(activated(int)),this,SLOT(recordSerialChoice(int)));
    connect(ui->newTaskButton,SIGNAL(clicked()),this,SLOT(addTask()));
    connect(ui->deleteTaskButton,SIGNAL(clicked()),this,SLOT(deleteTask()));
    connect(ui->postTaskButton,SIGNAL(clicked()),this,SLOT(postTaskInfo()));
    connect(ui->updateMapButton,SIGNAL(clicked()),this,SLOT(updateMapSize()));
    connect(ui->taskBox,SIGNAL(activated(int)),this,SLOT(changeTask(int)));
    connect(ui->editID,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
    connect(ui->editEndX,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
    connect(ui->editEndY,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
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
    reply_flag=false;
    autosend_flag=false;
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

void MainWindow::writeSerialport()
{
    QByteArray buffer;
    buffer.resize(PACKET_LENGTH);
    memcpy(buffer.data(),send_buffer,PACKET_LENGTH*sizeof(quint8));
    serialport->write(buffer);
}

void MainWindow::readSerialport()
{
    QByteArray src_data=serialport->readAll();
    quint8 line_id;
    quint16 sum=0;
    int index;
    rec_cache.append(src_data);
    index=getDataIndex(rec_cache);
    rec_cache=(index>0)?rec_cache.mid(index-1):rec_cache;
    if(rec_cache.size()<PACKET_LENGTH)
    {
        return;
    }
    quint8 *rec_buffer=(quint8*)rec_cache.data();
    for(int i=0;i<6;i++)
    {
        sum+=rec_buffer[i];
    }
    if(sum!=(rec_buffer[6]<<8|rec_buffer[7]))
    {
        return;
    }
    qDebug()<<rec_cache.size();
    rec_cache=rec_cache.mid(index+1);
    qDebug()<<rec_cache.size();
    qint16 x=rec_buffer[2]<<8|rec_buffer[3];
    qint16 y=rec_buffer[4]<<8|rec_buffer[5];
    if(rec_buffer[1]==REPLY_CMD)
    {
        reply_flag=true;
    }
    else if(rec_buffer[1]==SENDPOS_CMD)
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
    quint8 id=(id1>id2)?id1:id2;
    quint8 buffer[PACKET_LENGTH];
    if(!autosend_flag)
    {
        buffer[0]=id;
        buffer[1]=STOP_CMD;
        buffer[2]=0x00;
        buffer[3]=0x00;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=((STOP_CMD+id)&0xFF00)>>8;
        buffer[7]=(STOP_CMD+id)&0x00FF;
        memcpy(send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
        writeSerialport();
        send_timer->start(SENDTIME);
        autosend_flag=true;
        while(!reply_flag)
        {
            QCoreApplication::processEvents();
        }
        send_timer->stop();
        autosend_flag=false;
        reply_flag=false;
    }
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
    ui->taskBox->setCurrentText(QString::number(max_task_id));
    changeTask(ui->taskBox->currentIndex());
    if(ui->editEndX->text().isEmpty() && ui->editEndY->text().isEmpty())
    {
        QString dlgTitle="错误";
        QString strInfo="请先填写坐标信息!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return;
    }
    ui->editID->clear();
    ui->editEndX->clear();
    ui->editEndY->clear();
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

void MainWindow::updateMapSize()
{
    if(ui->editMapX->text().isEmpty() || ui->editMapY->text().isEmpty())
    {
        QString dlgTitle="错误";
        QString strInfo="请先填写地图信息!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return;
    }
    int x=ui->editMapX->text().toInt();
    int y=ui->editMapY->text().toInt();
    ui->widget->xAxis->setRange(0,x);//设置x轴长度
    ui->widget->yAxis->setRange(0,y);//设置y轴长度
    ui->widget->replot();
    collision->createMap(x,y);
}

void MainWindow::postTaskInfo()
{
    quint8 buffer[PACKET_LENGTH];
    int sum=0;
    if(!autosend_flag)
    {
        for(int i=0;i<task_list.size();i++)
        {
            buffer[0]=task_list[i].id;
            buffer[1]=TASK_CMD;
            buffer[2]=(task_list[i].x&0xFF00)>>8;
            buffer[3]=task_list[i].x&0x00FF;
            buffer[4]=(task_list[i].y&0xFF00)>>8;
            buffer[5]=task_list[i].y&0x00FF;
            for(int j=0;j<6;j++)
            {
                sum+=buffer[j];
            }
            buffer[6]=(sum&0xFF00)>>8;
            buffer[7]=sum&0x00FF;
            memcpy(send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
            writeSerialport();
            send_timer->start(SENDTIME);
            autosend_flag=true;
            while(!reply_flag)
            {
                QCoreApplication::processEvents();
            }
            send_timer->stop();
            autosend_flag=false;
            reply_flag=false;
        }
    }
}

void MainWindow::recordEditedTask()
{
    int index=task_id_list.indexOf(ui->taskBox->currentText().toInt());
    if(index==-1)
    {
        task newtask;
        newtask.id=ui->editID->text().toInt();
        newtask.x=ui->editEndX->text().toInt();
        newtask.y=ui->editEndY->text().toInt();
        task_list.append(newtask);
        task_id_list.append(max_task_id);
        max_task_id++;
    }
    else
    {
        task_list[index].id=ui->editID->text().toInt();
        task_list[index].x=ui->editEndX->text().toInt();
        task_list[index].y=ui->editEndY->text().toInt();
    }
}

int MainWindow::getDataIndex(QByteArray data)
{
    int reply_index,pos_index;
    reply_index=data.indexOf(REPLY_CMD);
    pos_index=data.indexOf(SENDPOS_CMD);
    if(reply_index==-1)
    {
        return pos_index;
    }
    else if(pos_index==-1)
    {
        return reply_index;
    }
    return (reply_index<pos_index)?reply_index:pos_index;
}

