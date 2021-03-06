#include "carlistform.h"

CarListForm::CarListForm(int width, int height, QWidget *parent):QWidget(parent)
{
    this->m_width = width;
    this->m_height = height;
    m_pressflag = false;
    m_InfoVec.clear();
    initUi();
    //connect(this,&CarListForm::updateList,this,&CarListForm::initStatusForms);
    m_pTimer = new QTimer(this);
    connect(m_pTimer,&QTimer::timeout,this,&CarListForm::updateCarListHeader);
    m_pTimer->start(1000);
}

void CarListForm::initUi()
{
    m_pMainWidget = new QWidget(this);
    m_pMainWidget->setObjectName("KWidget");
    m_pMainWidget->resize(m_width,m_height);
    QFont m_font ("Times", 20);
    m_pHeadLabel = new QLabel(m_pMainWidget);
    m_pHeadLabel->resize(m_width,m_height/7);
    m_pHeadLabel->setStyleSheet("background-color:rgb(24, 24, 72)");
    m_pInfoLabel = new QLabel("设备列表",m_pMainWidget);
    m_pInfoLabel->setObjectName("headLabel");
    m_pInfoLabel->setStyleSheet("color:white");
    m_pInfoLabel->setFont(m_font);
    m_pInfoLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    m_pInfoLabel->move(10,10);
    m_pMinBtn = new QPushButton(this);
    m_pMinBtn->setText("-");
    m_pMinBtn->setFont(m_font);
    m_pMinBtn->resize(45,30);
    m_pMinBtn->setStyleSheet("background-color:rgb(24, 24, 72);color:white");
    m_pMinBtn->move(int(m_width*0.85),10);
    connect(m_pMinBtn,&QPushButton::clicked,this,&CarListForm::onMinimize);
    QStringList list;
    list<<"全部"<<"在线"<<"离线"<<"异常"<<"维修";
    int interval = m_width/6;//65;

    QFont m_font_l ("Times", 12);
    for(int i = 0;i< 5;i++)
    {
        QLabel *l = new QLabel("asd",m_pMainWidget);
        l->setStyleSheet("color:white");
        l->setText(list[i]);
        l->move(interval*i+interval-20, int(m_height/7*0.6));
        l->setAttribute(Qt::WA_TranslucentBackground, true);
        l->setFont(m_font_l);
    }
    for(int i = 0;i < 5;i ++)
    {
        QLabel *b = new QLabel(m_pMainWidget);
        b->setStyleSheet("color:white");
        b->setAttribute(Qt::WA_TranslucentBackground, true);
        b->move(interval*i+interval-10,int(m_height/7*0.8));
        b->setText("0");
        b->setFont(m_font_l);
        m_InfoVec.append(b);
    }
    QStringList list_h;
    list_h<<"设备编号"<<"状态"<<"设备任务"<<"执行状态"<<"电量";
    interval = m_width/6;//70;
    QFont m_font_s ("Times", 10);
    for(int i = 0;i <5;i++)
    {
        QLabel *e = new QLabel(m_pMainWidget);
        e->setAttribute(Qt::WA_TranslucentBackground, true);
        if(i == 0)
            e->move(interval*(i+1)-20,int(m_height/7*1.1));
        else if(i == 2)
            e->move(interval*(i+1)-20,int(m_height/7*1.1));
        else if(i == 4)
            e->move(interval*(i+1)-20,int(m_height/7*1.1));
        else
            e->move(interval*(i+1)-20,int(m_height/7*1.1));
        e->setStyleSheet("color:black");
        e->setFont(m_font_s);
        e->setText(list_h[i]);

    }
    initStatusForms();
    this->setStyleSheet("#KWidget{background-color:rgb(210, 210, 210)}");
}

void CarListForm::updateUI()
{
    list = m_pMainWidget->findChildren<CarStatusWidget*>("kStatusWidget");
    if(list.size() == 0)
        return;
    foreach (CarStatusWidget* w, list) {
        w->hide();
        w->deleteLater();
    }
    initStatusForms();
}

void CarListForm::initStatusForms()
{
    auto it = Myconfig::GetInstance()->m_CarMap.begin();
    for(int i = 0;i < Myconfig::GetInstance()->m_CarMap.size();i++)
    {
        //        if(KDeviceSingleton::getInstance()->m_DeviceMap[it.value().deviceIp])
        //        {
        //            BaseDevice *o = KDeviceSingleton::getInstance()->m_DeviceMap[it.value().deviceIp];
        //            CarStatusWidget *f = new CarStatusWidget(m_width,it.value(),o,m_pMainWidget);
        //            f->setObjectName("kStatusWidget");
        //            f->move(5,m_height/5+i*60);
        //            f->show();
        //        }
        CarStatusWidget *f = new CarStatusWidget(m_width,it.key(),m_pMainWidget);
        f->setObjectName("kStatusWidget");
        f->move(5,m_height/5+i*60);
        f->show();
        it++;
    }
}

void CarListForm::updateCarListHeader()
{
    m_onLineNum = 0;
    m_disonLineNum = 0;
    m_totalNum = 0;
    auto it = Myconfig::GetInstance()->m_CarMap.begin();
    for(int i = 0;i < Myconfig::GetInstance()->m_CarMap.size();i++)
    {
        m_totalNum ++;
        if(it.value().deveceStatus.isOnline)
            m_onLineNum ++;
        else
            m_disonLineNum ++;
        it++;
    }
    m_InfoVec.at(0)->setText(QString("%1").arg(m_totalNum));
    m_InfoVec.at(1)->setText(QString("%1").arg(m_onLineNum));
    m_InfoVec.at(2)->setText(QString("%1").arg(m_disonLineNum));
}

void CarListForm::onMinimize()
{
    this->hide();
    emit minimize();
}

void CarListForm::mousePressEvent(QMouseEvent *event)
{
    m_pressflag = true;
    m_beginP = event->globalPos();
    m_windowP = this->frameGeometry().topLeft();
    QWidget::mousePressEvent(event);
}

void CarListForm::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressflag = true;
    QWidget::mouseReleaseEvent(event);
}

void CarListForm::mouseMoveEvent(QMouseEvent *event)
{
    if(m_pressflag)
    {
        QPoint relativePos = event->globalPos() - m_beginP;
        this->move(m_windowP + relativePos );
    }
    QWidget::mouseMoveEvent(event);
}


CarStatusWidget::CarStatusWidget(int width, int carId, QWidget *parent) :QWidget(parent)
{
    m_pCar = nullptr;
    //o->registObserver(this);
    //m_ip = carId.deviceIp;
    w = new QWidget(this);
    this->m_carNum = carId;
    interval = width/6;//50;
    y = 10;
    w->resize(width-10,40);
    initUI();
    //    QFont font("Times", 9 ,QFont::Bold);
    //    numLab = new QLabel(QString("%1").arg(m_carNum),w);
    //    numLab->setAttribute(Qt::WA_TranslucentBackground, true);
    //    numLab->setFont(font);
    //    numLab->move(10,y);

    //    staLab = new QLabel(carId.deveceStatus.isOnline? "在线":"离线",w);
    //    staLab->setAttribute(Qt::WA_TranslucentBackground, true);
    //    staLab->move(interval*2,y);
    //    taskLab = new QLabel("---",w);
    //    taskLab->setAttribute(Qt::WA_TranslucentBackground, true);
    //    taskLab->move(interval*3+10,y);
    //    QString status = "";
    //    if(carId.deveceStatus.status == 1)
    //        status = " 待命中";
    //    else if(carId.deveceStatus.status == 2)
    //        status  = "工作中";
    //    else
    //        status  = " 故障";
    //    workLab = new QLabel(status,w);
    //    workLab->setAttribute(Qt::WA_TranslucentBackground, true);
    //    workLab->move(interval*5-10,y);
    //    QString batter = QString("%1%").arg(carId.deveceStatus.batter);
    //    batterLab = new QLabel(batter,w);
    //    batterLab->setAttribute(Qt::WA_TranslucentBackground, true);
    //    batterLab->move(interval*6+20,y);
    //    w->setStyleSheet("QWidget:hover{background-color:white; color: black;}QWidget:pressed{background-color:rgb(85, 170, 255);}");
}

void CarStatusWidget::initUI()
{
    numLab = new QLabel(w);
    numLab->setAttribute(Qt::WA_TranslucentBackground, true);
    numLab->move(interval,10);

    staLab = new QLabel(w);
    staLab->setAttribute(Qt::WA_TranslucentBackground, true);
    staLab->move(interval*2-20,10);
    staLab->resize(50,20);

    taskLab = new QLabel("---",w);
    taskLab->setAttribute(Qt::WA_TranslucentBackground, true);
    taskLab->move(interval*3-20,y);
    workLab = new QLabel(w);
    workLab->setAttribute(Qt::WA_TranslucentBackground, true);
    workLab->move(interval*4-20,y);
    workLab->resize(50,20);
    // workLab->move(interval*5-10,y);
    batterLab = new QLabel(w);
    batterLab->setAttribute(Qt::WA_TranslucentBackground, true);
    batterLab->move(interval*5-20,y);
    batterLab->resize(50,20);
    //
    m_pResetBtn = new QPushButton(w);
    //m_pResetBtn->setIcon(QIcon(":/resouse/Image/reconnect.png"));
    m_pResetBtn->move (interval*6-68,y-3);
    m_pResetBtn->setText("Reset");//background-image: url(:res/1.png);
    m_pResetBtn->setStyleSheet("QPushButton{background:transparent;color:rgb(201,64,73)}"
                                  "QPushButton:pressed{background-repeat: no-repeat;background-position:left;border:none;color:rgb(200, 200, 200);}");
    m_pResetBtn->setFlat(true);
    m_pResetBtn->setVisible(false);
    connect(m_pResetBtn,&QPushButton::clicked,this,&CarStatusWidget::onReseted);
    //w->setStyleSheet("QWidget:hover{background-color:white; color: black;}QWidget:pressed{background-color:rgb(85, 170, 255)};}");
    m_ptimer = new QTimer(this);
    connect(m_ptimer,&QTimer::timeout,this,&CarStatusWidget::onTimeoutUpdateUI);
    m_ptimer->start(300);
}

void CarStatusWidget::onTimeoutUpdateUI()
{
    CarInfoStru c = Myconfig::GetInstance()->m_CarMap[m_carNum];
    QFont font("Times", 9 ,QFont::Bold);
    numLab->setText(QString("%1").arg(m_carNum));
    numLab->setFont(font);
    //numLab->move(10,10);

    staLab->setText(c.deveceStatus.isOnline? "在线":"离线");
    QString status = "";
    if(c.deveceStatus.status == 3)
        status = " 待命中";
    else if(c.deveceStatus.status == 1)
        status  = "工作中";
    else
    {
        status  = " 故障";
        m_pResetBtn->setVisible(true);
    }
    workLab->setText(status);
    QString batter = QString("%1%").arg(c.deveceStatus.batter);
    batterLab->setText(batter);
}

void CarStatusWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //QWidget *w = qobject_cast<QWidget *>(m_pParent);
    m_pCar = new CarStatusFrom(m_carNum);
    QRect  desktop = QGuiApplication::primaryScreen()->geometry();
    m_pCar->move(desktop.width()/2,desktop.height()/2);
    m_pCar->show();
    QWidget::mouseDoubleClickEvent(event);
}
///
/// \brief CarStatusWidget::onReseted
/// 点击reset按钮时，为了安全起见，要确保wcs已经暂停，但是当多任务系统工作时，不建议这么做！
///
void CarStatusWidget::onReseted()
{
    if(Myconfig::GetInstance()->m_flag == true)
    {
        QMessageBox msgBox;
         msgBox.setText("请先暂停wcs ！");
         msgBox.exec();
    }
    Myconfig::GetInstance()->m_CarMap[m_carNum].deveceStatus.status = 3;
    TCommtransceivermanager::GetInstance()->ResetHWcommob(m_carNum);
    m_pResetBtn->setVisible(false);
}
///
/// \brief CarStatusWidget::updateStatusOnBase Observer has been Notifly()
///
void CarStatusWidget::updateStatusOnBase()
{
    if(Myconfig::GetInstance()->m_CarMap.contains(m_carNum))
    {
        m_car = Myconfig::GetInstance()->m_CarMap[m_carNum];
        numLab->setText(m_car.deviceNum);
        staLab->setText(m_car.deveceStatus.isOnline? "在线":"离线");
        taskLab->setText("---");
        QString status = "";
        if(m_car.deveceStatus.isLocking == false)
            status = " 待命中";
        else if(m_car.deveceStatus.isLocking == true)
            status  = "工作中";
        else
            status  = " 故障";
        workLab->setText(status);
        //qDebug()<<"listInfo batter:"<<m_car.deviceIp<<m_car.deveceStatus.isLocking;
        batterLab->setText(QString("%1%").arg(m_car.deveceStatus.batter));
    }
}
