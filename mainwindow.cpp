#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widgetInit();
    connect(timer,SIGNAL(timeout()),this,SLOT(searchSerialport()));
    connect(send_timer,SIGNAL(timeout()),this,SLOT(writeZigbeeSerialport()));
    connect(ui->zigbee_openButton,SIGNAL(clicked()),this,SLOT(changeZigbeeSerialState()));
    connect(ui->zigbee_serialportBox,SIGNAL(activated(int)),this,SLOT(recordZigbeeSerialChoice(int)));
    connect(ui->uwb_openButton,SIGNAL(clicked()),this,SLOT(changeUWBSerialState()));
    connect(ui->uwb_serialportBox,SIGNAL(activated(int)),this,SLOT(recordUWBSerialChoice(int)));
    connect(ui->newTaskButton,SIGNAL(clicked()),this,SLOT(addTask()));
    connect(ui->deleteTaskButton,SIGNAL(clicked()),this,SLOT(deleteTask()));
    connect(ui->postTaskButton,SIGNAL(clicked()),this,SLOT(postTaskInfo()));
    connect(ui->updateMapButton,SIGNAL(clicked()),this,SLOT(updateMapSize()));
    connect(ui->taskBox,SIGNAL(activated(int)),this,SLOT(changeTask(int)));
    connect(ui->editID,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
    connect(ui->editEndX,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
    connect(ui->editEndY,SIGNAL(textEdited(const QString)),this,SLOT(recordEditedTask()));
    connect(zigbee_serialport,SIGNAL(readyRead()),this,SLOT(readZigbeeSerialport()));
    connect(uwb_serialport,SIGNAL(readyRead()),this,SLOT(readUWBSerialport()));
    connect(collision,SIGNAL(stopSignal(QList<quint8>)),this,SLOT(emitStopSignal(QList<quint8>)));
    connect(collision,SIGNAL(advSignal(quint8)),this,SLOT(emitAdvSignal(quint8)));
    connect(collision,SIGNAL(regSignal(QMap<quint8,QPoint>)),this,SLOT(emitRegSignals(QMap<quint8,QPoint>)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::widgetInit()
{
    zigbee_serial_state=CLOSESTATE;
    zigbee_serial_choice="";
    uwb_serial_state=CLOSESTATE;
    uwb_serial_choice="";
    max_task_id=0;
    reply_flag=false;
    autosend_flag=false;
    collision = new detector();
    processor = new process(collision);
    processor->setReplyFlag(&reply_flag);
    ui->zigbee_openButton->setIcon(QPixmap(":/state/close.png"));
    ui->zigbee_openButton->setText("打开串口");
    ui->uwb_openButton->setIcon(QPixmap(":/state/close.png"));
    ui->uwb_openButton->setText("打开定位");
    ui->widget->setInteractions(QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    timer->start(BASETIME);
    qRegisterMetaType<QList<quint8>>("QList<quint8>");
}

void MainWindow::changeZigbeeSerialState()
{
    if(zigbee_serial_state==CLOSESTATE && openZigbeeSerialport())
    {
        ui->zigbee_openButton->setIcon(QPixmap(":/state/open.png"));
        ui->zigbee_openButton->setText("关闭串口");
        zigbee_serial_state=OPENSTATE;
    }
    else if(zigbee_serial_state==OPENSTATE)
    {
        ui->zigbee_openButton->setIcon(QPixmap(":/state/close.png"));
        ui->zigbee_openButton->setText("打开串口");
        zigbee_serial_state=CLOSESTATE;
        closeZigbeeSerialport();
    }
}

void MainWindow::changeUWBSerialState()
{
    if(uwb_serial_state==CLOSESTATE && openUWBSerialport())
    {
        ui->uwb_openButton->setIcon(QPixmap(":/state/open.png"));
        ui->uwb_openButton->setText("取消定位");
        uwb_serial_state=OPENSTATE;
    }
    else if(uwb_serial_state==OPENSTATE)
    {
        ui->uwb_openButton->setIcon(QPixmap(":/state/close.png"));
        ui->uwb_openButton->setText("打开定位");
        uwb_serial_state=CLOSESTATE;
        closeUWBSerialport();
    }
}

void MainWindow::recordZigbeeSerialChoice(int choice)
{
    zigbee_serial_choice=ui->zigbee_serialportBox->itemText(choice);
    if(zigbee_serial_state==OPENSTATE)
    {
        closeZigbeeSerialport();
        openZigbeeSerialport();
    }
}

void MainWindow::recordUWBSerialChoice(int choice)
{
    uwb_serial_choice=ui->uwb_serialportBox->itemText(choice);
    if(uwb_serial_state==OPENSTATE)
    {
        closeUWBSerialport();
        openUWBSerialport();
    }
}

void MainWindow::searchSerialport()
{
    QList<QString> temp_list;
    if(QSerialPortInfo::availablePorts().size()!=0)
    {
        if(ui->zigbee_serialportBox->itemText(0)=="")
        {
            ui->zigbee_serialportBox->removeItem(0);
        }
        if(ui->uwb_serialportBox->itemText(0)=="")
        {
            ui->uwb_serialportBox->removeItem(0);
        }
        if(serialport_list.isEmpty())
        {
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
            {
                serialport_list.append(info.portName());
            }
            for(int i=0;i<serialport_list.size();i++)
            {
                ui->zigbee_serialportBox->addItem(serialport_list[i]);
                ui->uwb_serialportBox->addItem(serialport_list[i]);
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
                ui->zigbee_serialportBox->clear();
                ui->uwb_serialportBox->clear();
                for(int i=0;i<temp_list.size();i++)
                {
                    ui->zigbee_serialportBox->addItem(temp_list[i]);
                    ui->uwb_serialportBox->addItem(temp_list[i]);
                }
                serialport_list=temp_list;
            }
        }
        ui->zigbee_serialportBox->setCurrentText(zigbee_serial_choice);
        ui->uwb_serialportBox->setCurrentText(uwb_serial_choice);
    }
    else
    {
        ui->zigbee_serialportBox->clear();
        ui->zigbee_serialportBox->addItem("");
        ui->uwb_serialportBox->clear();
        ui->uwb_serialportBox->addItem("");
        serialport_list.clear();
    }
}

bool MainWindow::openZigbeeSerialport()
{
    zigbee_serialport->setPortName(ui->zigbee_serialportBox->currentText());
    zigbee_serialport->setBaudRate(ui->zigbee_baudrateBox->currentText().toInt());
    zigbee_serialport->setStopBits(QSerialPort::OneStop);
    zigbee_serialport->setFlowControl(QSerialPort::NoFlowControl);
    zigbee_serialport->setDataBits(QSerialPort::Data8);
    zigbee_serialport->setParity(QSerialPort::NoParity);
    QSerialPortInfo serial_info(ui->zigbee_serialportBox->currentText());
    if(serial_info.isBusy()==true)
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!串口已被占用!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        zigbee_serial_state=OPENSTATE;
        changeZigbeeSerialState();
        return false;
    }
    if(!zigbee_serialport->open(QIODevice::ReadWrite))
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return false;
    }
    return true;
}

bool MainWindow::openUWBSerialport()
{
    uwb_serialport->setPortName(ui->uwb_serialportBox->currentText());
    uwb_serialport->setBaudRate(ui->uwb_baudrateBox->currentText().toInt());
    uwb_serialport->setStopBits(QSerialPort::OneStop);
    uwb_serialport->setFlowControl(QSerialPort::NoFlowControl);
    uwb_serialport->setDataBits(QSerialPort::Data8);
    uwb_serialport->setParity(QSerialPort::NoParity);
    QSerialPortInfo serial_info(ui->uwb_serialportBox->currentText());
    if(serial_info.isBusy()==true)
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!串口已被占用!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        uwb_serial_state=OPENSTATE;
        changeUWBSerialState();
        return false;
    }
    if(!uwb_serialport->open(QIODevice::ReadWrite))
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return false;
    }
    quint8 buffer[11]={0x01,0x10,0x00,0x28,0x00,0x01,0x02,0x00,0x04,0xA1,0xBB};
    memcpy(uwb_send_buffer,buffer,11*sizeof(quint8));
    writeUWBSerialport();
    return true;
}

void MainWindow::closeZigbeeSerialport()
{
    zigbee_serialport->close();
}

void MainWindow::closeUWBSerialport()
{
    quint8 buffer[11]={0x01,0x10,0x00,0x28,0x00,0x01,0x02,0x00,0x00,0xA0,0x78};
    memcpy(uwb_send_buffer,buffer,11*sizeof(quint8));
    writeUWBSerialport();
    uwb_serialport->close();
}

void MainWindow::writeZigbeeSerialport()
{
    QByteArray buffer;
    buffer.resize(PACKET_LENGTH);
    memcpy(buffer.data(),zigbee_send_buffer,PACKET_LENGTH*sizeof(quint8));
    zigbee_serialport->write(buffer);
}

void MainWindow::writeUWBSerialport()
{
    QByteArray buffer;
    buffer.resize(11);
    memcpy(buffer.data(),uwb_send_buffer,11*sizeof(quint8));
    uwb_serialport->write(buffer);
}

void MainWindow::readZigbeeSerialport()
{
    QByteArray src_data=zigbee_serialport->readAll();
    qDebug()<<src_data;
    processor->setCacheData(src_data);
    if(!processor->isbusy)
    {
        processor->start();
    }
}

void MainWindow::readUWBSerialport()
{
    QByteArray src_data=uwb_serialport->readAll();
    if(src_data.size()==31)
    {
        quint8 *buffer=(quint8*)src_data.data();
        if(crc_chk(buffer,29)!=(buffer[30]<<8|buffer[29]))
        {
            return;
        }
        qint16 id=buffer[3]<<8|buffer[4];
        qint16 x=buffer[7]<<8|buffer[8];
        qint16 y=buffer[9]<<8|buffer[10];
        x=(x<0)?0:x;
        x=(x>map_lenth)?map_lenth:x;
        y=(y<0)?0:y;
        y=(y>map_width)?map_width:y;
        quint8 trajectory_index;
        if(!collision->car_state.contains(id))
        {
            collision->car_state.insert(id,ADV);
        }
        if(!collision->car_pos.contains(id))
        {
            collision->car_pos.insert(id,QPoint(x,y));
        }
        else
        {
            collision->car_pos[id]=QPoint(x,y);
        }
        if(id_list.indexOf(id)==-1)
        {
            id_list.append(id);
            addGraph();
            QVector<QCPCurveData> trajectory_data;
            trajectory_data.append(QCPCurveData(0,x,y));
            trajectorydata_list.append(trajectory_data);
        }
        else
        {
            trajectory_index=id_list.indexOf(id);
            collision->clearPosInfo(trajectorydata_list[trajectory_index].last().key,trajectorydata_list[trajectory_index].last().value,id);
            trajectorydata_list[trajectory_index].append(QCPCurveData(trajectorydata_list[trajectory_index].size(),x,y));
            trajectory_list[trajectory_index]->data()->set(trajectorydata_list[trajectory_index],true);
        }
        ui->widget->replot();
        collision->setCheckPoint(x,y,id);
        if(!collision->isbusy)
        {
            collision->start();
        }
    }
}

void MainWindow::emitRegSignals(QMap<quint8,QPoint> q)
{
    quint8 buffer[PACKET_LENGTH];
    quint8 id;
    QList<quint8> keys=q.keys();
    int sum=0;
    if(!autosend_flag)
    {
        for(int i=0;i<keys.size();i++)
        {
            id=keys.at(i);
            buffer[0]=0x5E;
            buffer[1]=0x0C;
            buffer[2]=id;
            buffer[3]=AVOID_CMD;
            buffer[4]=(q[id].x()&0xFF00)>>8;
            buffer[5]=q[id].x()&0x00FF;
            buffer[6]=(q[id].y()&0xFF00)>>8;
            buffer[7]=q[id].y()&0x00FF;
            sum=0;
            qDebug()<<"ssssssssssssssssssss";
            for(int j=0;j<8;j++)
            {
                qDebug()<<buffer[j];
                sum+=buffer[j];
            }
            qDebug()<<"eeeeeeeeeeeeeeeeeeee";
            buffer[8]=(sum&0xFF00)>>8;
            buffer[9]=sum&0x00FF;
            memcpy(zigbee_send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
            writeZigbeeSerialport();
            send_timer->start(SENDTIME);
            autosend_flag=true;
            while(!reply_flag)
            {
                QCoreApplication::processEvents();
            }
            if(collision->car_state.contains(id))
            {
                collision->car_state[id]=STOP;
            }
            send_timer->stop();
            autosend_flag=false;
            reply_flag=false;
        }
        autosend_flag=true;
    }
}

void MainWindow::emitStopSignal(QList<quint8> q)
{
    quint8 buffer[PACKET_LENGTH];
    quint8 id;
    if(!autosend_flag)
    {
        for(int i=0;i<q.size();i++)
        {
            id=q.at(i);
            buffer[0]=0x5E;
            buffer[1]=0x0C;
            buffer[2]=id;
            buffer[3]=STOP_CMD;
            buffer[4]=0x00;
            buffer[5]=0x00;
            buffer[6]=0x00;
            buffer[7]=0x00;
            buffer[8]=((buffer[3]+id+0x6A)&0xFF00)>>8;
            buffer[9]=(buffer[3]+id+0x6A)&0x00FF;
            memcpy(zigbee_send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
            writeZigbeeSerialport();
            send_timer->start(SENDTIME);
            autosend_flag=true;
            while(!reply_flag)
            {
                QCoreApplication::processEvents();
            }
            if(collision->car_state.contains(id))
            {
                collision->car_state[id]=STOP;
            }
            send_timer->stop();
            autosend_flag=false;
            reply_flag=false;
        }
        collision->regCars(q,&autosend_flag);
    }
}

void MainWindow::emitAdvSignal(quint8 id)
{
    quint8 buffer[PACKET_LENGTH];
    qDebug()<<autosend_flag;
    autosend_flag=false;
    if(!autosend_flag)
    {
        buffer[0]=0x5E;
        buffer[1]=0x0C;
        buffer[2]=id;
        buffer[3]=ADV_CMD;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x00;
        buffer[7]=0x00;
        buffer[8]=((ADV_CMD+id+0x6A)&0xFF00)>>8;
        buffer[9]=(ADV_CMD+id+0x6A)&0x00FF;
        memcpy(zigbee_send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
        writeZigbeeSerialport();
        send_timer->start(SENDTIME);
        autosend_flag=true;
        while(!reply_flag)
        {
            QCoreApplication::processEvents();
        }
        if(collision->car_state.contains(id))
        {
            collision->car_state[id]=ADV;
        }
        send_timer->stop();
        autosend_flag=false;
        reply_flag=false;
    }
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
    map_lenth=x;
    map_width=y;
    ui->widget->xAxis->setRange(0,x);
    ui->widget->yAxis->setRange(0,y);
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
            buffer[0]=0x5E;
            buffer[1]=0x0C;
            buffer[2]=task_list[i].id;
            buffer[3]=TASK_CMD;
            buffer[4]=(task_list[i].x&0xFF00)>>8;
            buffer[5]=task_list[i].x&0x00FF;
            buffer[6]=(task_list[i].y&0xFF00)>>8;
            buffer[7]=task_list[i].y&0x00FF;
            sum=0;
            for(int j=0;j<8;j++)
            {
                sum+=buffer[j];
            }
            buffer[8]=(sum&0xFF00)>>8;
            buffer[9]=sum&0x00FF;
            memcpy(zigbee_send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
            writeZigbeeSerialport();
            send_timer->start(SENDTIME);
            autosend_flag=true;
            while(!reply_flag)
            {
                QCoreApplication::processEvents();
            }
            if(!collision->car_state.contains(task_list[i].id))
            {
                collision->car_state.insert(task_list[i].id,ADV);
            }
            else
            {
                collision->car_state[task_list[i].id]=ADV;
            }
//            qDebug()<<collision->car_state;
//            collision->car_target.insert(task_list[i].id,QPoint(task_list[i].x,task_list[i].y));
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

void MainWindow::addGraph()
{
    QCPCurve *trajectory = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    trajectory->setPen(QPen(colorlib[trajectory_list.size()%16]));
    trajectory_list.append(trajectory);
}

quint16 MainWindow::crc_chk(quint8 *data,quint8 length)
{
    int j;
    unsigned int crc_reg=0xffff;
    while(length--)
    {
        crc_reg^=*data++;
        for(j=0;j<8;j++)
        {
            if(crc_reg&0x01)
            {
                crc_reg=(crc_reg>>1)^0xa001;
            }
            else
            {
                crc_reg=crc_reg>>1;
            }

        }
    }
    return crc_reg;
}
