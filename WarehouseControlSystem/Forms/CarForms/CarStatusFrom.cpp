#include "CarStatusFrom.h"
#include <QDebug>
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"
CarStatusFrom::CarStatusFrom(int carId, QWidget *parent) : QWidget(parent)
{
    CarInfoStru c = Myconfig::GetInstance()->m_CarMap[carId];
    desktop =   QGuiApplication::primaryScreen()->geometry();
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QLabel{background:transparent;color:white}QPushButton{border:1px gray;background-color:gray;color:white;border-radius:3px;width:70px;height:25;} QPushButton:hover{background-color:white; color: black;}QPushButton:pressed{background-color:rgb(85, 170, 255);}");
    m_carNum = c.carId;
    m_id = carId;
    //qDebug()<<"carid"<<m_id;
    QFont font("宋体",14);
    QLabel *back_lab = new QLabel(this);
    back_lab->resize(PAD_X,PAD_Y);
    back_lab->setStyleSheet("background-color:rgb(198,198,198)");

    header_lab = new QLabel(this);
    header_lab->resize(PAD_X,100);
    header_lab->setStyleSheet("background-color:rgb(24, 24, 72)");

    online_image = new QLabel(this);
    online_image->resize(15,15);
    online_image->setStyleSheet(c.deveceStatus.isOnline?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
    online_image->move(30,10);
    online_lab = new QLabel(c.deveceStatus.isOnline? "在线":"离线",this);
    online_lab->move (60,10);
    online_lab->setStyleSheet("color:white");
    //增加重新连接按钮,
    int invacation_text = 10;
    m_reconnectbtn = new QPushButton(this);//reconnectcolor:rgb(80, 80, 200)
    m_reconnectbtn->setIcon(QIcon(":/resouse/Image/reconnect.png"));
    m_reconnectbtn->move (100,10);
    m_reconnectbtn->setText("连接");//background-image: url(:res/1.png);
    m_reconnectbtn->setStyleSheet("QPushButton{background:transparent}"
                                  "QPushButton:pressed{background-repeat: no-repeat;background-position:left;border:none;color:rgb(200, 200, 200);}");
    m_reconnectbtn->setFlat(true);
    connect(m_reconnectbtn,&QPushButton::clicked,this,&CarStatusFrom::SlotReconnect);
    m_reconnectbtn->setEnabled(false);


    closeBtn = new QPushButton(this);
    closeBtn->setStyleSheet("border-image:url(:/resouse/Image/close.png);background-color:rgb(24, 24, 72);");
    closeBtn->move(PAD_X-40,10);
    closeBtn->resize(30,30);
    connect(closeBtn,&QPushButton::clicked,this,&CarStatusFrom::fromClose);


    car_image = new QLabel(this);
    car_image->resize(50,50);
    car_image->setStyleSheet("border-image:url(:/resouse/Image/car.png);");
    int line1_y = 60;
    car_image->move(50,line1_y/2);
    car_number = new QLabel(this);
    car_number->resize(150,20);
    car_number->setText(c.deviceNum);
    car_number->move(70,line1_y-20);
    car_number->setFont(font);
    car_number->setAlignment(Qt::AlignCenter);
    car_number->setStyleSheet("color:white");
    l = new QLabel("---",this);
    l->setStyleSheet("color:white");
    l->move(120,line1_y);
    battery_image = new QLabel(this);
    battery_image->resize(30,50);
    battery_image->setStyleSheet("border-image:url(:/resouse/Image/battery.png);");
    battery_image->move(400,line1_y/2);
    battery_lab = new QLabel(this);
    battery_lab->resize(80,20);
    battery_lab->setText(QString::number(c.deveceStatus.batter)+"%");
    battery_lab->move(450,line1_y-20);
    battery_lab->setFont(font);
    battery_lab->setStyleSheet("color:white");

    QLabel *b = new QLabel("电池用量",this);
    b->setStyleSheet("color:white");
    b->move(450,line1_y);

   // this->setStyleSheet("QPushButton{border:1px gray;background-color:gray;color:white;border-radius:3px;width:70px;height:25;} QPushButton:hover{background-color:white; color: black;}QPushButton:pressed{background-color:rgb(85, 170, 255);}");
    table = new QTableWidget(this);
    table->setColumnCount(4);    //设置列数
    table->setRowCount(4);        //设置行数/
    table->resize(PAD_X-20,122);
    table->move(10,110);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->horizontalScrollBar()->setEnabled(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //table->setHorizontalScrollMode(Qt::ScrollBarAlwaysOff);

    QTableWidgetItem *item = new QTableWidgetItem ("设备IP");
    table->setItem(0, 0, item);
    item = new QTableWidgetItem (c.deviceIp);
    table->setItem(0, 1, item);
    item = new QTableWidgetItem ("执行状态");
    table->setItem(0, 2, item);
    item = new QTableWidgetItem (c.deveceStatus.enable? "enable" : "disable");
    table->setItem(0, 3, item);
    item = new QTableWidgetItem ("设备名称");
    table->setItem(1, 0, item);
    item = new QTableWidgetItem (QString("%1").arg(c.carId));
    table->setItem(1, 1, item);
    item = new QTableWidgetItem ("货架名称");
    table->setItem(1, 2, item);
    item = new QTableWidgetItem ("");
    table->setItem(1, 3, item);

    item = new QTableWidgetItem ("当前位置");
    table->setItem(2, 0, item);
    m_crunt_item = new QTableWidgetItem (QString("%1,%2").arg(c.deveceStatus.carCurrentPosion.x).arg(c.deveceStatus.carCurrentPosion.y));
    table->setItem(2, 1, m_crunt_item);
    item = new QTableWidgetItem ("目标位置");
    table->setItem(2, 2, item);
    item = new QTableWidgetItem (QString("%1,%2").arg(c.deveceStatus.carEndPosion.x).arg(c.deveceStatus.carEndPosion.y));
    table->setItem(2, 3, item);

    item = new QTableWidgetItem ("是否可用");
    table->setItem(3, 0, item);
    item = new QTableWidgetItem (c.deveceStatus.enable? "enable" : "disable");
    table->setItem(3, 1, item);
    item = new QTableWidgetItem ("任务编号");
    table->setItem(3, 2, item);
    item = new QTableWidgetItem ("---");
    table->setItem(3, 3, item);
    table->setStyleSheet("background:#FFFFFF;border:none;font-size:14px;font-family:Microsoft YaHei;color:#666666;");
    int interval = 90;
    up_btn = new QPushButton("左取货",this);
    up_btn->setObjectName("left_btn");
    up_btn->move(10,242);//10+interval*2,242
    down_btn = new QPushButton("右取货",this);
    down_btn->move(10+interval*1,242);//10+interval*3,242
    down_btn->setObjectName("right_btn");
    connect(up_btn,&QPushButton::clicked,this,&CarStatusFrom::onPickup);
    connect(down_btn,&QPushButton::clicked,this,&CarStatusFrom::onPickup);

    m_pMove_x = new QPushButton("移动X",this);
    connect(m_pMove_x,&QPushButton::clicked,this,&CarStatusFrom::relative_move_x);
    m_pMove_x->move(10+interval*3,242);

    m_pLineEdit_x = new QSpinBox(this);
    m_pLineEdit_x->setMaximum(20000000);
    m_pLineEdit_x->setMinimum(-2000000);
    m_pLineEdit_x->resize(80,27);
    m_pLineEdit_x->move(10+interval*2,242);

    m_pLineEdit_y = new QSpinBox(this);
    m_pLineEdit_y->setMaximum(10000000);
    m_pLineEdit_y->setMinimum(-20000000);
    m_pLineEdit_y->resize(80,27);
    m_pLineEdit_y->move(10+interval*4,242);
    m_pMove_y = new QPushButton("移动Y",this);
    connect(m_pMove_y,&QPushButton::clicked,this,&CarStatusFrom::relative_move_y);
    m_pMove_y->move(10+interval*5,242);

    int interval_image = 65;
    int interval_y = 278;
    int interval_image_text = 32;
    QLabel *ca = new QLabel("校准中",this);
    ca->move(interval_image_text+interval_image*2,interval_y+30);
    autoCalibration = new QLabel(this);
    autoCalibration->resize(15,15);
    autoCalibration->move(interval_image*2+15,interval_y+30);
    autoCalibration->setStyleSheet(c.deveceStatus.statusinfodstru.bcalibrating?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    QLabel *e = new QLabel("故障",this);
    e->move(interval_image_text+interval_image*3,interval_y+30);
    err_lab = new QLabel(this);
    err_lab->resize(15,15);
    err_lab->setStyleSheet(c.deveceStatus.statusinfodstru.berror?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
    err_lab->move(interval_image*3+15,interval_y+30);

    QLabel *r = new QLabel("就绪",this);
    r->move(interval_image_text+interval_image*4,interval_y+30);
    ready_lab = new QLabel(this);
    ready_lab->resize(15,15);
    ready_lab->setStyleSheet(c.deveceStatus.statusinfodstru.bready?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
    ready_lab->move(interval_image*4+15,interval_y+30);

    QLabel *w = new QLabel("执行中",this);
    w->move(interval_image_text+interval_image*5,interval_y+30);
    working_lab = new QLabel(this);
    working_lab->resize(15,15);
    working_lab->setStyleSheet(c.deveceStatus.statusinfodstru.bruning?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
    working_lab->move(15+interval_image*5,interval_y+30);

    m_pCharg_btn = new QPushButton("去充电",this);
    m_pCharg_btn->move(interval_image*3,272);
    connect(m_pCharg_btn,&QPushButton::clicked,this,&CarStatusFrom::onCharged);


    QLabel *n = new QLabel("未就绪",this);
    n->move(interval_image_text+interval_image*4,interval_y);
    notReady_lab = new QLabel(this);
    notReady_lab->resize(15,15);
    notReady_lab->setStyleSheet(c.deveceStatus.statusinfodstru.bunready?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
    notReady_lab->move(15+interval_image*4,interval_y);


    invacation_text += 7;
    QLabel *l = new QLabel("左有货",this);
    l->move(interval_image_text+interval_image*5,interval_y);
    m_pLeft_haveBox = new QLabel(this);
    m_pLeft_haveBox->resize(15,15);
    m_pLeft_haveBox->move(15+interval_image*5,interval_y);
    m_pLeft_haveBox->setStyleSheet(c.deveceStatus.statusinfodstru.bcalibrating?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    QLabel *rb = new QLabel("右有货",this);
    rb->move(interval_image_text+interval_image*6,interval_y);
    m_pRight_haveBox = new QLabel(this);
    m_pRight_haveBox->resize(15,15);
    m_pRight_haveBox->move(15+interval_image*6,interval_y);
    m_pRight_haveBox->setStyleSheet(c.deveceStatus.statusinfodstru.bcalibrating?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    QLabel *t = new QLabel("上有货",this);
    t->move(interval_image_text+interval_image*7,interval_y);
    m_pTop_haveBox = new QLabel(this);
    m_pTop_haveBox->resize(15,15);
    m_pTop_haveBox->move(15+interval_image*7,interval_y);
    m_pTop_haveBox->setStyleSheet(c.deveceStatus.statusinfodstru.bcalibrating?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    QLabel *l_e = new QLabel("执行完成",this);
    l_e->move(interval_image_text,interval_y+30);
    m_pExe_done = new QLabel(this);
    m_pExe_done->resize(15,15);
    m_pExe_done->move(15,interval_y+30);
    m_pExe_done->setStyleSheet(c.deveceStatus.readonlystatus==3?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    QLabel *l_l = new QLabel("电量低",this);
    l_l->move(interval_image_text+interval_image*1,interval_y+30);
    m_pElectricity_low = new QLabel(this);
    m_pElectricity_low->resize(15,15);
    m_pElectricity_low->move(interval_image+15,interval_y+30);
    m_pElectricity_low->setStyleSheet(c.deveceStatus.statusinfodstru.belectricity?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");

    m_pLeftPutDown_btn = new QPushButton("左放货",this);
    m_pLeftPutDown_btn->setObjectName("Left_putDown");
    m_pLeftPutDown_btn->move(10,272);
    connect(m_pLeftPutDown_btn,&QPushButton::clicked,this,&CarStatusFrom::onPickup);

    m_pRightPutDown_btn = new QPushButton("右放货",this);
    m_pRightPutDown_btn->setObjectName("Right_putDown");
    m_pRightPutDown_btn->move(10+interval,272);
    connect(m_pRightPutDown_btn,&QPushButton::clicked,this,&CarStatusFrom::onPickup);
    this->resize(550,PAD_Y);
//    if(KDeviceSingleton::getInstance()->m_DeviceMap.contains(c.deviceIp))
//    {
//        KDeviceSingleton::getInstance()->m_DeviceMap[c.deviceIp]->registObserver(this);
//    }
//    foreach (QPushButton *btn, this) {
//        btn->setStyleSheet("border:5fix");
//    }
    //connect(TCommtransceivermanager::GetInstance(),&TCommtransceivermanager::SignalCarStatusUpdate,this,&CarStatusFrom::updateStatusOnBase);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&CarStatusFrom::updateStatusOnBase);
    timer->start(300);
}

void CarStatusFrom::fromClose()
{
    if(KDeviceSingleton::getInstance()->m_DeviceMap.contains("m_id"))
    {
        KDeviceSingleton::getInstance()->m_DeviceMap["m_ip"]->removeObserver(this);
    }
    this->close();
}

void CarStatusFrom::relative_move_x()
{

      //move_test();
        if(check_move_state() == false)
            return;
        int value = m_pLineEdit_x->value();
    //    if(false == check_x(value))
    //        return;
        AbstructInstruction *c = new CarInstruction();
        OrderStru o;
        o.order = X;
        o.value = value;
        c->setParameter(o,m_id);
        c->runInstruction();
        QString result = "";
        int r = c->getResult(result);
        qDebug()<<result<<" | result :"<<r;
}

void CarStatusFrom::relative_move_y()
{
    if(check_move_state() == false)
        return;
    int value = m_pLineEdit_y->value();
    //    if(false == check_y(value))
    //        return;
    AbstructInstruction *c = new CarInstruction();
    OrderStru o;
    o.order = Y;
    o.value = value;
    c->setParameter(o,m_id);
    c->runInstruction();
    QString result = "";
    c->getResult(result);
}

void CarStatusFrom::left_pickup()
{

}

void CarStatusFrom::right_pickup()
{

}
///
/// \brief CarStatusFrom::ckeck_y
/// \param y 横移动时Y的值
/// \return 超出范围是返回false；
///
bool CarStatusFrom::check_y(int y)
{
    int x = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.x;
    int c_y = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y;
    int y_r = c_y + y;
    if(x == -2191)
    {
        if(y_r >= -14143 || y_r <= 1423 )
            return true;
    }
    else if(x == 0)
    {
        if(y_r >= -11353 || y_r <= 1423)
            return true;
    }
    else if(x == 2191)
    {
        if(y_r >= -11353 || y_r <= 8563)
            return true;
    }
    return false;
}

bool CarStatusFrom::check_x(int x)
{
    int y = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y;
    if(y != 0)
        return false;
    int c_x = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.x;
    if(2190 >= (c_x + x) && (c_x + x) >=-2190)
        return true;
    return false;
}

bool CarStatusFrom::check_move_state()
{
    if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready == 0 || Myconfig::GetInstance()->m_run_mode == 0\
            ||Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline == false)
    {
        return false;
    }
    return true;
}
///
/// \brief CarStatusFrom::SetBtnState
/// \param enble
///
void CarStatusFrom::SetBtnState(bool enble)
{
//   if(enble)//设置可用
//   {
//       if(!up_btn->isEnabled())
//       {
//          up_btn->setEnabled(true);
//       }
//       if(!down_btn->isEnabled())
//       {
//          down_btn->setEnabled(true);
//       }
//       if(!m_pMove_x->isEnabled())
//       {
//          m_pMove_x->setEnabled(true);
//       }
//       if(!m_pMove_y->isEnabled())
//       {
//          m_pMove_y->setEnabled(true);
//       }
//   }
//   else{//设置不可用
//       if(up_btn->isEnabled())
//       {
//          up_btn->setEnabled(false);
//       }
//       if(down_btn->isEnabled())
//       {
//          down_btn->setEnabled(false);
//       }
//       if(m_pMove_x->isEnabled())
//       {
//          m_pMove_x->setEnabled(false);
//       }
//       if(m_pMove_y->isEnabled())
//       {
//          m_pMove_y->setEnabled(false);
//       }
//   }
    if(!Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline && enble)//不在线且手动状态下才可以使用此按钮
    {
       if(!m_reconnectbtn->isEnabled())
           m_reconnectbtn->setEnabled(true);
   }
   else{
       if(m_reconnectbtn->isEnabled())
           m_reconnectbtn->setEnabled(false);
    }
}

void CarStatusFrom::move_test(int type)
{
    bool direction = false;
    int value = 0;
    AbstructInstruction *c = new CarInstruction();
    for(int i = 0;i < 100; i++)
    {
        if(check_move_state() == false)
            return;
        direction ? value = 3500 : value = -3500;
        //    if(false == check_x(value))
        //        return;
        OrderStru o;
        if(type == 1 )
            o.order = Y;
        else if(type == 2)
            o.order = X;
        o.value = value;
        c->setParameter(o,m_id);
        c->runInstruction();
        QString result = "";
        int r = c->getResult(result);
        qDebug()<<result<<" | result :"<<r;
        qDebug()<<"Value = "<<value<< "count = "<<i;
        if(r < 0)
        {
            qDebug()<<"Instruction exec failed!";
            return;
        }
        direction = !direction;
        Delay_MSec(500);
    }
}

void CarStatusFrom::Delay_MSec(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
///
/// \brief CarStatusFrom::SlotReconnect
/// 设置小车重新连接机制
void CarStatusFrom::SlotReconnect()
{
    TCommtransceivermanager::GetInstance()->ReconnectComm(m_id);
}

void CarStatusFrom::onCharged()
{
    if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready == 0 || Myconfig::GetInstance()->m_run_mode == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("不要在自动模式下使用复位功能。");
        msgBox.exec();
        return;
    }
    //1.确保在扫码位置

    KPosition start = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion;
    if( false == ((start.x == -2190 && start.y == 0) || (start.x == 2190 && start.y == 0) || (start.x == 0 && start.y == 0)))
    {
        QMessageBox msgBox;
        msgBox.setText("请扫码复位后再使用复位功能。");
        msgBox.exec();
        return;
    }
    //2.确定当前坐标 和目标坐标
    double z = start.z;
    KPosition end{0,1423,int(z),0};
    //3.生成充电轨迹
   TaskInfoStru t;
   t.taskMainType = "充电";
   t.boxNum= "null";
   t.taskNum = QString("%1").arg(QRandomGenerator::global()->bounded(0,32765));
   QString ip = "null";
    //4.执行轨迹
    KDispatch *k = new KDispatch(end,ip,m_carNum,t);//完成的状态，完成的结果，写入数据库的时间??
    QThreadPool::globalInstance()->start(k);
}
// batter status enable online? position
void CarStatusFrom::updateStatusOnBase()
{
    if(Myconfig::GetInstance()->m_CarMap.contains(m_id))
    {
         online_image->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         online_lab->setText(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline?"在线":"离线");
         battery_lab->setText(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.belectricity? "20%" : "100%");
         QTableWidgetItem *item = new QTableWidgetItem (Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.status == 2? "异常" : "待命中");
         table->setItem(0, 3, item);
         item = new QTableWidgetItem (Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.enable? "enable" : "disable");
         table->setItem(3, 1, item);

         m_crunt_item->setText(QString("%1,%2,%3").arg(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.x).arg(Myconfig::GetInstance()->m_CarMap[m_carNum].deveceStatus.carCurrentPosion.y).arg(Myconfig::GetInstance()->m_CarMap[m_carNum].deveceStatus.carCurrentPosion.z));
         err_lab->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         ready_lab->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         working_lab->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bruning?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         notReady_lab->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bunready?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         autoCalibration->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bcalibrating?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         m_pLeft_haveBox->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.senorgoodsstru.bLhavegoods?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         m_pRight_haveBox->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.senorgoodsstru.bRhavegoods?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         m_pTop_haveBox->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.senorgoodsstru.bloadhavegoods?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         m_pElectricity_low->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.belectricity?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         m_pExe_done->setStyleSheet(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.readonlystatus==3?"border-image:url(:/resouse/Image/green.png)":"border-image:url(:/resouse/Image/grey.png)");
         //更新按钮的状态是否可用
         if(!Myconfig::GetInstance()->m_run_mode)//自动
         {
            SetBtnState(false);
         }
         else{
             SetBtnState(true);
         }
    }
}

void CarStatusFrom::mousePressEvent(QMouseEvent *event)
{
    m_pressflag = true;
    m_beginP = event->globalPos();
    m_windowP = this->frameGeometry().topLeft();
    QWidget::mousePressEvent(event);
}

void CarStatusFrom::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressflag = true;
    QWidget::mouseReleaseEvent(event);
}

void CarStatusFrom::onPickup()
{
    if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready == 0 || Myconfig::GetInstance()->m_run_mode == 0)
    {
        return;
    }
    QString resultMsg = "";
    QPushButton *b = static_cast<QPushButton*>(sender());
    OrderStru o;
    if(b->objectName() == "left_btn")
        o.order = Order::Left_Pickup;
    else if(b->objectName() == "right_btn")
        o.order = Order::Right_Pickup;
    else if(b->objectName() == "Left_putDown")
        o.order = Order::Left_Putinto;
    else if(b->objectName() == "Right_putDown")
        o.order = Order::Right_Putinto;
        //move_test(1);
    AbstructInstruction *abstructInstruction = new CarInstruction();
    abstructInstruction->setParameter(o,m_id);
    abstructInstruction->runInstruction();
    abstructInstruction->getResult(resultMsg);
}

void CarStatusFrom::mouseMoveEvent(QMouseEvent *event)
{
    if(m_pressflag)
    {
        QPoint relativePos = event->globalPos() - m_beginP;
        this->move(m_windowP + relativePos );
    }
    QWidget::mouseMoveEvent(event);
}








