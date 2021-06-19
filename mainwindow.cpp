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

/*
函数名：widgetInit
参数：无
返回值：无
功能：初始化界面显示和变量
*/
void MainWindow::widgetInit()
{
    zigbee_serial_state=CLOSESTATE;         /*********************************/
    zigbee_serial_choice="";                /*初始化zigbee（uwb）选择的串口名为空*/
    uwb_serial_state=CLOSESTATE;            /**初始化zigbee（uwb）串口状态为关闭**/
    uwb_serial_choice="";                   /*********************************/
    max_task_id=0;                          //初始化最大任务id为0
    reply_flag=false;                       //初始化回复标志位为false
    autosend_flag=false;                    //初始化自动发送标志位为false
    collision = new detector();
    processor = new process(collision);
    processor->setReplyFlag(&reply_flag);   //传入回复标志位的地址给处理数据的线程
    ui->zigbee_openButton->setIcon(QPixmap(":/state/close.png"));   /************/
    ui->zigbee_openButton->setText("打开串口");                      /***按钮贴图***/
    ui->uwb_openButton->setIcon(QPixmap(":/state/close.png"));      /*设置按钮文本*/
    ui->uwb_openButton->setText("打开定位");                         /************/
    ui->widget->setInteractions(QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);//曲线可选、图例可选、绘图表可选
    timer->start(BASETIME);                 //开始定时搜索可用串口
    qRegisterMetaType<QList<quint8>>("QList<quint8>");
}

/*
函数名：changeZigbeeSerialState
参数：无
返回值：无
功能：按下打开/关闭zigbee串口按钮时改变按钮显示
*/
void MainWindow::changeZigbeeSerialState()
{
    if(zigbee_serial_state==CLOSESTATE && openZigbeeSerialport())
    {
        ui->zigbee_openButton->setIcon(QPixmap(":/state/open.png"));    //改变按钮贴图和文本
        ui->zigbee_openButton->setText("关闭串口");
        zigbee_serial_state=OPENSTATE;
    }
    else if(zigbee_serial_state==OPENSTATE)
    {
        ui->zigbee_openButton->setIcon(QPixmap(":/state/close.png"));   //改变按钮贴图和文本
        ui->zigbee_openButton->setText("打开串口");
        zigbee_serial_state=CLOSESTATE;
        closeZigbeeSerialport();
    }
}

/*
函数名：changeUWBSerialState
参数：无
返回值：无
功能：按下打开/关闭uwb串口按钮时改变按钮显示
*/
void MainWindow::changeUWBSerialState()
{
    if(uwb_serial_state==CLOSESTATE && openUWBSerialport())
    {
        ui->uwb_openButton->setIcon(QPixmap(":/state/open.png"));   //改变按钮贴图和文本
        ui->uwb_openButton->setText("取消定位");
        uwb_serial_state=OPENSTATE;
    }
    else if(uwb_serial_state==OPENSTATE)
    {
        ui->uwb_openButton->setIcon(QPixmap(":/state/close.png"));   //改变按钮贴图和文本
        ui->uwb_openButton->setText("打开定位");
        uwb_serial_state=CLOSESTATE;
        closeUWBSerialport();
    }
}

/*
函数名：recordZigbeeSerialChoice
参数：无
返回值：无
功能：记录zigbee串口下拉列表的选择
*/
void MainWindow::recordZigbeeSerialChoice(int choice)
{
    zigbee_serial_choice=ui->zigbee_serialportBox->itemText(choice);   //记录zigbee串口下拉列表的选择
    if(zigbee_serial_state==OPENSTATE)
    {
        closeZigbeeSerialport();   //重启串口
        openZigbeeSerialport();
    }
}

/*
函数名：recordUWBSerialChoice
参数：无
返回值：无
功能：记录uwb串口下拉列表的选择
*/
void MainWindow::recordUWBSerialChoice(int choice)
{
    uwb_serial_choice=ui->uwb_serialportBox->itemText(choice);          //记录uwb串口下拉列表的选择
    if(uwb_serial_state==OPENSTATE)
    {
        closeUWBSerialport();     //重启串口
        openUWBSerialport();
    }
}

/*
函数名：searchSerialport
参数：无
返回值：无
功能：寻找可用串口并显示
*/
void MainWindow::searchSerialport()
{
    QList<QString> temp_list;
    if(QSerialPortInfo::availablePorts().size()!=0)     //可用串口非空
    {
        if(ui->zigbee_serialportBox->itemText(0)=="")   //原先zigbee可用串口列表只包含空选项
        {
            ui->zigbee_serialportBox->removeItem(0);    //清除空选项
        }
        if(ui->uwb_serialportBox->itemText(0)=="")      //原先uwb可用串口列表只包含空选项
        {
            ui->uwb_serialportBox->removeItem(0);       //清除空选项
        }
        if(serialport_list.isEmpty())                   //之前记录的串口列表为空
        {
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
            {
                serialport_list.append(info.portName());//将所有串口添加进列表
            }
            for(int i=0;i<serialport_list.size();i++)
            {
                ui->zigbee_serialportBox->addItem(serialport_list[i]);//显示所有串口到zigbee串口列表
                ui->uwb_serialportBox->addItem(serialport_list[i]);   //显示所有串口到uwb串口列表
            }
        }
        else
        {
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
            {
                temp_list.append(info.portName());     // 记录所有串口
            }
            if(temp_list!=serialport_list)             //可用串口有变化
            {
                ui->zigbee_serialportBox->clear();     //清除下拉框所有选项
                ui->uwb_serialportBox->clear();
                for(int i=0;i<temp_list.size();i++)
                {
                    ui->zigbee_serialportBox->addItem(temp_list[i]);    //重新添加下拉框选项
                    ui->uwb_serialportBox->addItem(temp_list[i]);
                }
                serialport_list=temp_list;             //储存本次搜寻结果
            }
        }
        ui->zigbee_serialportBox->setCurrentText(zigbee_serial_choice); //显示当前串口选择
        ui->uwb_serialportBox->setCurrentText(uwb_serial_choice);
    }
    else
    {
        ui->zigbee_serialportBox->clear();      //没有可用串口时清除下拉框所有选项
        ui->zigbee_serialportBox->addItem("");  //添加一个空选项
        ui->uwb_serialportBox->clear();
        ui->uwb_serialportBox->addItem("");
        serialport_list.clear();                //记录本次搜寻结果为空
    }
}

/*
函数名：openZigbeeSerialport
参数：无
返回值：true：打开成功
       false：打开失败
功能：打开zigbee所在串口
*/
bool MainWindow::openZigbeeSerialport()
{
    zigbee_serialport->setPortName(ui->zigbee_serialportBox->currentText());        //设置串口名
    zigbee_serialport->setBaudRate(ui->zigbee_baudrateBox->currentText().toInt());  //设置波特率
    zigbee_serialport->setStopBits(QSerialPort::OneStop);                           //设置停止位
    zigbee_serialport->setFlowControl(QSerialPort::NoFlowControl);                  //设置控制流
    zigbee_serialport->setDataBits(QSerialPort::Data8);                             //设置数据位
    zigbee_serialport->setParity(QSerialPort::NoParity);                            //设置检验位
    QSerialPortInfo serial_info(ui->zigbee_serialportBox->currentText());           //实例化zigbee串口
    if(serial_info.isBusy()==true)                      //串口正忙
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!串口已被占用!";
        QMessageBox::critical(this,dlgTitle,strInfo);   //弹出错误提示窗口
        zigbee_serial_state=OPENSTATE;
        changeZigbeeSerialState();
        return false;
    }
    if(!zigbee_serialport->open(QIODevice::ReadWrite))  //串口无法读写
    {
        QString dlgTitle="错误";
        QString strInfo="打开串口失败!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return false;
    }
    return true;
}

/*
函数名：openUWBSerialport
参数：无
返回值：true：打开成功
       false：打开失败
功能：打开uwb所在串口
*/
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

/*
函数名：closeZigbeeSerialport
参数：无
返回值：无
功能：关闭zigbee所在串口
*/
void MainWindow::closeZigbeeSerialport()
{
    zigbee_serialport->close();
}

/*
函数名：closeUWBSerialport
参数：无
返回值：无
功能：关闭uwb所在串口
*/
void MainWindow::closeUWBSerialport()
{
    quint8 buffer[11]={0x01,0x10,0x00,0x28,0x00,0x01,0x02,0x00,0x00,0xA0,0x78};
    memcpy(uwb_send_buffer,buffer,11*sizeof(quint8));
    writeUWBSerialport();                               //发送停止定位命令
    uwb_serialport->close();
}

/*
函数名：writeZigbeeSerialport
参数：无
返回值：无
功能：向zigbee发送数据
*/
void MainWindow::writeZigbeeSerialport()
{
    QByteArray buffer;
    buffer.resize(PACKET_LENGTH);
    memcpy(buffer.data(),zigbee_send_buffer,PACKET_LENGTH*sizeof(quint8));
    zigbee_serialport->write(buffer);                   //写数据到串口
}

/*
函数名：writeUWBSerialport
参数：无
返回值：无
功能：向uwb发送数据
*/
void MainWindow::writeUWBSerialport()
{
    QByteArray buffer;
    buffer.resize(11);
    memcpy(buffer.data(),uwb_send_buffer,11*sizeof(quint8));
    uwb_serialport->write(buffer);                      //写数据到串口
}

/*
函数名：readZigbeeSerialport
参数：无
返回值：无
功能：读zigbee的数据
*/
void MainWindow::readZigbeeSerialport()
{
    QByteArray src_data=zigbee_serialport->readAll();   //读串口数据
    processor->setCacheData(src_data);                  //将数据传入待处理区域
    if(!processor->isbusy)                              //线程空闲
    {
        processor->start();                             //开启新线程处理数据
    }
}

/*
函数名：readUWBSerialport
参数：无
返回值：无
功能：读uwb的数据
*/
void MainWindow::readUWBSerialport()
{
    QByteArray src_data=uwb_serialport->readAll();        //读uwb串口数据
    if(src_data.size()==31)                               //数据位数为31
    {
        quint8 *buffer=(quint8*)src_data.data();
        if(crc_chk(buffer,29)!=(buffer[30]<<8|buffer[29]))//crc校验
        {
            return;
        }
        qint16 id=buffer[3]<<8|buffer[4];                 //解析标签id和位置
        qint16 x=buffer[7]<<8|buffer[8];
        qint16 y=buffer[9]<<8|buffer[10];
        x=(x<0)?0:x;                                      //将位置点限制于地图内
        x=(x>map_lenth)?map_lenth:x;
        y=(y<0)?0:y;
        y=(y>map_width)?map_width:y;
        quint8 trajectory_index;
        if(!collision->car_state.contains(id))            //记录车辆状态的列表无该车辆的id
        {
            collision->car_state.insert(id,ADV);          //记录该车辆的id并记录为前进状态
        }
        if(!collision->car_pos.contains(id))              //记录车辆位置的列表无该车辆的id
        {
            collision->car_pos.insert(id,QPoint(x,y));    //记录该车辆的id并记录位置
        }
        else
        {
            collision->car_pos[id]=QPoint(x,y);           //更新位置
        }
        if(id_list.indexOf(id)==-1)                       //id列表中无该车辆
        {
            id_list.append(id);                           //添加id
            addGraph();                                   //添加绘图曲线
            QVector<QCPCurveData> trajectory_data;
            trajectory_data.append(QCPCurveData(0,x,y));  //添加轨迹起始点
            trajectorydata_list.append(trajectory_data);  //添加轨迹数据到轨迹数据列表
        }
        else
        {
            trajectory_index=id_list.indexOf(id);         //获取id在id列表中的位置
            collision->clearPosInfo(trajectorydata_list[trajectory_index].last().key,trajectorydata_list[trajectory_index].last().value,id);//清除碰撞检测器中该车辆的旧位置
            trajectorydata_list[trajectory_index].append(QCPCurveData(trajectorydata_list[trajectory_index].size(),x,y));//添加当前位置到轨迹点中
            trajectory_list[trajectory_index]->data()->set(trajectorydata_list[trajectory_index],true);//设置该车辆的轨迹的轨迹点
        }
        ui->widget->replot();                             //重新绘制轨迹图
        collision->setCheckPoint(x,y,id);                 //设置车辆碰撞检测点
        if(!collision->isbusy)                            //碰撞检测器空闲
        {
            collision->start();                           //开始检测碰撞
        }
    }
}

/*
函数名：emitRegSignals
参数：需要调控的所有车辆的id与避让点
返回值：无
功能：向需要调控的车辆发送避让点信息进行避让
*/
void MainWindow::emitRegSignals(QMap<quint8,QPoint> q)
{
    quint8 buffer[PACKET_LENGTH];
    quint8 id;
    QList<quint8> keys=q.keys();                          //获取队列的所有键
    int sum=0;
    if(!autosend_flag)                                    //未自动发送
    {
        for(int i=0;i<keys.size();i++)
        {
            id=keys.at(i);                                //获取id
            buffer[0]=0x5E;                               //包头
            buffer[1]=0x0C;
            buffer[2]=id;                                 //发送目标车辆id
            buffer[3]=AVOID_CMD;                          //避让命令
            buffer[4]=(q[id].x()&0xFF00)>>8;              //避让点x高八位
            buffer[5]=q[id].x()&0x00FF;                   //避让点x低八位
            buffer[6]=(q[id].y()&0xFF00)>>8;              //避让点y低八位
            buffer[7]=q[id].y()&0x00FF;                   //避让点y低八位
            sum=0;
            for(int j=0;j<8;j++)
            {
                sum+=buffer[j];                           //计算检验和
            }
            buffer[8]=(sum&0xFF00)>>8;                    //校验和高八位
            buffer[9]=sum&0x00FF;                         //校验和低八位
            memcpy(zigbee_send_buffer,buffer,PACKET_LENGTH*sizeof(quint8));
            writeZigbeeSerialport();                      //发送
            send_timer->start(SENDTIME);                  //开始定时发送
            autosend_flag=true;
            while(!reply_flag)
            {
                QCoreApplication::processEvents();        //事件循环直到收到回复
            }
            if(collision->car_state.contains(id))
            {
                collision->car_state[id]=STOP;            //车辆标记为停止状态
            }
            send_timer->stop();                           //停止自动发送
            autosend_flag=false;
            reply_flag=false;
        }
        autosend_flag=true;
    }
}

/*
函数名：emitStopSignal
参数：需要停止的所有车辆的id
返回值：无
功能：向需要停止的车辆发送停止命令
*/
void MainWindow::emitStopSignal(QList<quint8> q)
{
    quint8 buffer[PACKET_LENGTH];
    quint8 id;
    if(!autosend_flag)
    {
        for(int i=0;i<q.size();i++)
        {
            id=q.at(i);                                   //见emitRegSignals
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
        collision->regCars(q);                            //调控车辆
    }
}

/*
函数名：emitAdvSignal
参数：需要继续前行的车辆id
返回值：无
功能：向需要继续前行的车辆发送前进命令
*/
void MainWindow::emitAdvSignal(quint8 id)
{
    quint8 buffer[PACKET_LENGTH];
    autosend_flag=false;
    if(!autosend_flag)
    {
        buffer[0]=0x5E;                                   //见emitRegSignals
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

/*
函数名：addTask
参数：无
返回值：无
功能：添加任务的逻辑代码
*/
void MainWindow::addTask()
{
    ui->taskBox->setCurrentText(QString::number(max_task_id));  //切换至最后一个任务
    changeTask(ui->taskBox->currentIndex());                    //显示最后一个任务的详情
    if(ui->editEndX->text().isEmpty() && ui->editEndY->text().isEmpty())//检查填写
    {
        QString dlgTitle="错误";
        QString strInfo="请先填写坐标信息!";
        QMessageBox::critical(this,dlgTitle,strInfo);
        return;
    }
    ui->editID->clear();                                        //清除相关显示
    ui->editEndX->clear();
    ui->editEndY->clear();
    ui->taskBox->addItem(QString::number(max_task_id));         //添加任务
    ui->taskBox->setCurrentText(QString::number(max_task_id));  //设置下拉框文本
    ui->deleteTaskButton->setEnabled(true);                     //失能删除任务按钮
}

/*
函数名：deleteTask
参数：无
返回值：无
功能：删除任务的逻辑代码
*/
void MainWindow::deleteTask()
{
    int index=ui->taskBox->currentText().toInt();           //获取下拉框中的任务id数字
    ui->taskBox->removeItem(ui->taskBox->currentIndex());   //删除下拉框相关项
    task_list.removeAt(task_id_list.indexOf(index));        //从任务列表中删除任务
    task_id_list.removeAt(task_id_list.indexOf(index));     //从任务id列表中删除id
    if(task_id_list.size()==1)                              //只剩一个任务
    {
        ui->deleteTaskButton->setEnabled(false);            //失能删除任务的按钮
    }
    ui->taskBox->setCurrentText(QString::number(task_id_list[0]));//任务下拉框显示第一个下拉框选项
    changeTask(0);                                          //显示第一个下拉框的任务的任务详情
}

/*
函数名：changeTask
参数：下拉列表的选项
返回值：无
功能：切换到下拉列表中的任务，显示该任务详情
*/
void MainWindow::changeTask(int i)
{
    Q_UNUSED(i);
    int index=task_id_list.indexOf(ui->taskBox->currentText().toInt());//获取任务id在任务id列表中的位置
    if(index==-1)                                         //找不到任务id
    {
        ui->editID->clear();                              //清空相关信息的显示
        ui->editEndX->clear();
        ui->editEndY->clear();
        return;
    }
    ui->editID->setText(QString::number(task_list[index].id));  //显示相关信息
    ui->editEndX->setText(QString::number(task_list[index].x));
    ui->editEndY->setText(QString::number(task_list[index].y));
}

/*
函数名：updateMapSize
参数：无
返回值：无
功能：更新地图大小，更新显示，创建碰撞检测器中的地图
*/
void MainWindow::updateMapSize()
{
    if(ui->editMapX->text().isEmpty() || ui->editMapY->text().isEmpty()) //界面地图尺寸填写为空
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
    ui->widget->xAxis->setRange(0,x);                     //设置x轴长度
    ui->widget->yAxis->setRange(0,y);                     //设置y轴长度
    ui->widget->replot();                                 //重新绘制界面
    collision->createMap(x,y);                            //在碰撞检测器中创建地图模型
}

/*
函数名：postTaskInfo
参数：无
返回值：无
功能：发送任务列表的所有信息
*/
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
            buffer[2]=task_list[i].id;                    //任务中的车辆id
            buffer[3]=TASK_CMD;                           //任务指令
            buffer[4]=(task_list[i].x&0xFF00)>>8;         //任务中目的地的x坐标的高八位
            buffer[5]=task_list[i].x&0x00FF;              //任务中目的地的x坐标的低八位
            buffer[6]=(task_list[i].y&0xFF00)>>8;         //任务中目的地的y坐标的高八位
            buffer[7]=task_list[i].y&0x00FF;              //任务中目的地的y坐标的低八位
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
            send_timer->stop();
            autosend_flag=false;
            reply_flag=false;
        }
    }
}

/*
函数名：recordEditedTask
参数：无
返回值：无
功能：记录编辑完的任务信息
*/
void MainWindow::recordEditedTask()
{
    int index=task_id_list.indexOf(ui->taskBox->currentText().toInt());//用任务id找寻任务在列表中的位置
    if(index==-1)                                                      //没有此任务
    {
        task newtask;                                                  //创建新任务
        newtask.id=ui->editID->text().toInt();                         //保存ui中填入的车辆id
        newtask.x=ui->editEndX->text().toInt();
        newtask.y=ui->editEndY->text().toInt();
        task_list.append(newtask);                                     //将任务添加至任务列表
        task_id_list.append(max_task_id);                              //将任务id添加至任务id列表
        max_task_id++;
    }
    else
    {
        task_list[index].id=ui->editID->text().toInt();                //改变任务信息
        task_list[index].x=ui->editEndX->text().toInt();
        task_list[index].y=ui->editEndY->text().toInt();
    }
}

/*
函数名：addGraph
参数：无
返回值：无
功能：添加轨迹
*/
void MainWindow::addGraph()
{
    QCPCurve *trajectory = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);//实例化轨迹对象
    trajectory->setPen(QPen(colorlib[trajectory_list.size()%16]));            //设置轨迹颜色
    trajectory_list.append(trajectory);                                       //添加轨迹到轨迹列表
}

/*
函数名：crc_chk
参数：数据首地址的指针，数据长度
返回值：数据的crc16的校验结果
功能：计算数据的crc校验结果
*/
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
