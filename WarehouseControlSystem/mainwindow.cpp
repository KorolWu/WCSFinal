#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QIcon>
#include "UnitClass/logininfowg.h"
#include "UnitClass/myIniconfig.h"
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"V1.1.2";
    this->setWindowFlags(Qt::FramelessWindowHint);
    QFile file(":/resouse/warehouse.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();

    getConfigParameter();
    QString sql_open_err;
    CRUDBaseOperation::getInstance()->openDB(sql_open_err);
    getParameterFromDB();
    loadCarInitialLayer();
    checkReidsConnected();
    GetSystemLogObj()->writeLog("WCS initialization ...",0);
    initUI();
    LogManager *l = new LogManager(this);
    l->start();
    m_pstoreWg = new StoreInfoWidget(p_main_widget);
    m_palarmWg = new AlarmInfoWg(p_main_widget);
    m_pmonitorui = new MonitorUI(p_main_widget);
    m_testorders = new TestcreateOrdersPath(p_main_widget);
    p_mUserManagerWg = new UserInfoWg(p_main_widget);
    TCommtransceivermanager::GetInstance()->InitHWcommob(); //所有数据通讯对象创建
    connect(TCommtransceivermanager::GetInstance(),&TCommtransceivermanager::signalError,this,&MainWindow::slotCommError);
    m_stackerWg = new StackerWg(p_main_widget);
    m_stackerWg->SetOPCCommID(62541);
    m_stackerRunnerWg = new StackerRunnerWg(p_main_widget);
    m_stackerRunnerWg->SetParam(9,21);
    deleteChildrenList();
    m_pMainWidget->show();

    m_pHttpServer = new  JQHttpServer::TcpServerManage(2);
    connect(m_pHttpServer,&JQHttpServer::TcpServerManage::onRedReady,this,&MainWindow::onReplyReady);
    m_pHttpServer->setHttpAcceptedCallback( [this]( const QPointer< JQHttpServer::Session > &session )
    {
        session->replyText(QString(getReply()));
    } );
    if(m_pHttpServer->listen( QHostAddress::Any, 23412 ))
    {
        GetSystemLogObj()->writeLog("HttpServer 启动成功",0);
        m_pMainWidget->appLog("HttpServer 启动成功");
    }
    else
    {
        GetSystemLogObj()->writeLog("HttpServer 启动失败",3);
        m_pMainWidget->appLog("HttpServer 启动失败");
    }
    if(sql_open_err != "")
    {
        GetSystemLogObj()->writeLog(sql_open_err,2);
        m_pMainWidget->appLog("Mysql 启动失败");
    }
    else
        m_pMainWidget->appLog("Mysql 启动成功");

    p_mCarList = new CarListForm(int(desk_rect.width()*0.2),int(desk_rect.height()*0.8),p_main_widget);
    p_mCarList->move(1200,3);
    p_mCarList->show();

    m_listIsShow = true;
    connect(p_mCarList,&CarListForm::minimize,this,&MainWindow::showCarList);
    p_mDispatchThread = new QThread();
    DispatchCenter *m_dispatchCenter = new DispatchCenter;
    m_dispatchCenter->moveToThread(p_mDispatchThread);
    connect(this,&MainWindow::dispatched,m_dispatchCenter,&DispatchCenter::dispatchTaskThreadV1);
    p_mDispatchThread->start();

    //emit dispatched();
    m_updatestoretimer = new QTimer(this);
    connect(m_updatestoretimer,&QTimer::timeout,&m_updateRealData,&UpdateRealtimeDataObject::SaveStoreinfotoDatabase);
    m_updateComTimer = new QTimer(this);
    connect(m_updateComTimer,&QTimer::timeout,&m_updateRealData,&UpdateRealtimeDataObject::RequestTimingupdateHWinfo);
    m_updateCarComTimer =  new QTimer(this);
    connect(m_updateCarComTimer,&QTimer::timeout,&m_updateRealData,&UpdateRealtimeDataObject::RequestTimingupdateCarHWinfo);
    m_updateStackerComTimer =  new QTimer(this);
    connect(m_updateStackerComTimer,&QTimer::timeout,&m_updateRealData,&UpdateRealtimeDataObject::UpdateStackerDataRequest);
    Runnerthread = nullptr;
    //启动前更新D2(流道向wcs发送的入库缓存值)的值
    initRunnerD2();
    if(sql_open_err == "")
    {
        m_updatestoretimer->start(1000);
        m_updateComTimer->start(800);
        m_updateCarComTimer->start(500);
        m_updateStackerComTimer->start(200);
        m_pMainWidget->StartUpdateShowInfo();
        //增加运行流道内容的模块部分
        Runnerthread = new QThread();
        m_ExcrunnerOb = new ExcRunnerOutCmd();
        m_ExcrunnerOb->SetExcRunnerParam(9,40);//runner id = 9; scanCode id = 40;
        m_ExcrunnerOb->moveToThread(Runnerthread);
        connect(this,&MainWindow::SignaldisRunner,m_ExcrunnerOb,&ExcRunnerOutCmd::ExcRun);
        Runnerthread->start();
        emit SignaldisRunner();
    }
    m_pMainWidget->appLog("WMS 初始化成功!");
}

MainWindow::~MainWindow()
{
    delete m_updatestoretimer;
    delete m_updateComTimer;
    delete m_updateCarComTimer;
    delete m_updateStackerComTimer;
    delete ui;
}

void MainWindow::closeWcs()
{
    //在关闭窗体之前，将内存里的坐标写入数据库
    QString sql;//= "UPDATE t_device_info SET %1 = %2 WHERE id = %3;";
    for(auto it = Myconfig::GetInstance()->m_CarMap.begin();it != Myconfig::GetInstance()->m_CarMap.end();it++)
    {
        sql += QString("UPDATE t_device_info SET x=%1,y=%2,z=%3 WHERE id = %4;").arg(it.value().deveceStatus.carCurrentPosion.x)\
                .arg(it.value().deveceStatus.carCurrentPosion.y).arg(it.value().deveceStatus.carCurrentPosion.z).arg(it.key());
    }
    if(!CRUDBaseOperation::getInstance()->queryUseStr(sql))
    {
        qDebug()<<sql<<endl<<"update position failed";
        GetSystemLogObj()->writeLog("将内存里的坐标写入数据库失败!",2);//0x7f97609e8780
    }
    m_pMainWidget->CloseShowInfo();
    m_updatestoretimer->stop();
    m_updateComTimer->stop();
    m_updateCarComTimer->stop();
    m_updateStackerComTimer->stop();
    m_updateRealData.SaveStoreinfotoDatabase();
    Myconfig::GetInstance()->m_flag = false;
    Myconfig::GetInstance()->m_exit =false;
    QThread::msleep(1000);
    if((!Myconfig::GetInstance()->m_OuttaskMap[9].isEmpty()) || (!Myconfig::GetInstance()->m_IntaskMap[9].isEmpty()))
    {
        QMessageBox msgBox;
        msgBox.setText("The queue task has not completed.");
        msgBox.setInformativeText("Do you really want to turn off?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            closeObject();
            break;
        case QMessageBox::No:
            return;
        default:
            // should never be reached
            break;
        }
    }
    closeObject();
    GetSystemLogObj()->writeLog("WCS Normal shutdown !",0);
    this->close();
}

void MainWindow::closeObject()
{

    // QThreadPool::globalInstance()->deleteLater();
    Myconfig::GetInstance()->m_flag = false;

    QThreadPool::globalInstance()->waitForDone();
    if(QThreadPool::globalInstance()->activeThreadCount() > 0)
    {
        QThreadPool::globalInstance()->clear();
    }
    CRUDBaseOperation::getInstance()->closeDB();
    if(m_pHttpServer != nullptr)
        m_pHttpServer->deleteLater();
    p_mDispatchThread->quit();
    p_mDispatchThread->wait();
    p_mDispatchThread->deleteLater();

}

void MainWindow::initUI()
{

    desk_rect = QGuiApplication::primaryScreen()->geometry();
    this->resize(desk_rect.width(),desk_rect.height());

    QLabel *head_lab = new QLabel(this);
    head_lab->resize(desk_rect.width(),desk_rect.height()/10);
    head_lab->setStyleSheet("background-color:rgb(198,198,198)");
    QLabel *log_lab = new QLabel(this);
    log_lab->move(55,15);
    log_lab->resize(desk_rect.height()/10,desk_rect.height()/10-30);
    log_lab->setStyleSheet("border-image:url(:/resouse/Image/mt.png);");
    QLabel *info_lab =new QLabel(this);
    info_lab->setText("Warehouse Control System");
    info_lab->setFont(QFont("宋体",20));
    info_lab->move(55+desk_rect.height()/10+20,70);
    info_lab->resize(600,30);
    m_list_lab = new QLabel(this);
    m_list_lab->resize(300,7);
    m_list_lab->setStyleSheet("background-color:rgb(24, 24, 72)");
    m_list_lab->move(desk_rect.width() - 310,1);
    m_list_lab->hide();
    treewidget = new QWidget(this);
    treewidget->setStyleSheet("background-color:rgb(150,150,150)");
    treewidget->resize(desk_rect.width()/7,desk_rect.height()/10*9);
    treewidget->move(0,desk_rect.height()/10);
    p_main_widget = new QWidget(this);
    p_main_widget->resize(desk_rect.width()/7*6-5-60,desk_rect.height()/10*9-5);
    p_main_widget->move(desk_rect.width()/7+3,desk_rect.height()/10+3);
    p_main_widget->setStyleSheet("background-color:rgb(230,230,230)");
    t = new CurrentTask(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    connect(this,&MainWindow::httpRedReady,t,&CurrentTask::handelHttpTask);
    car_from = new AgvForm(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    m_pTaskAll = new AllTask(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    m_pSubTask = new SubTask(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    m_pLog = new LogForms(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    p_mElevator = new ElevatorFrom(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    p_mRunerForm = new RunerForm(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    m_pDispatchForm = new DispatchWidget(desk_rect.width()/7*6-5,desk_rect.height()/10*9-5,p_main_widget);
    p_mRunerForm->setParameter(Myconfig::GetInstance()->m_runer);
    p_treeView = new QTreeView(treewidget);
    m_pMainWidget = new MainWidget(desk_rect.width()/7*6-5-60,desk_rect.height()/10*9-5,p_main_widget);
    //    p_treeView->setStyleSheet("QTreeView{border: 1px solid lightgray;}"
    //                              "QTreeView::item {height: 40px;border-radius: 2px;border: 1px solid transparent;background: transparent;color: black;}"
    //                              "QTreeView::item:has-children {border: none;border-bottom: 1px solid lightgray;}"
    //                              "QTreeView::item:hover {border: 1px solid rgb(170, 190, 230);background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 rgb(230, 240, 250),"
    //                              "stop: 0.5 rgb(220, 235, 255), stop: 1.0 rgb(210, 230, 255));}"
    //                              "QTreeView::item:selected {border: 1px solid rgb(230, 240, 250);background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 rgb(230, 240, 250),"
    //                              "stop: 0.5 rgb(220, 235, 255), stop: 1.0 rgb(210, 230, 255));});)");

    p_treeView->setStyleSheet("QTreeView{border: 1px solid lightgray;}"
                              "QTreeView::item {height: 40px;border-radius: 2px;border: 1px solid transparent;background: transparent;color: black;}"
                              "QTreeView::item:has-children {border: none;border-bottom: 1px solid lightgray;}"
                              "QTreeView::item:hover {border: 1px solid rgb(170, 190, 230);background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 rgb(230, 240, 250),"
                              "stop: 0.5 rgb(220, 235, 255), stop: 1.0 rgb(210, 230, 255));}"
                              "QTreeView::item:selected {border: 1px solid rgb(230, 240, 250);background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 rgb(230, 240, 250),"
                              "stop: 0.5 rgb(220, 235, 255), stop: 1.0 rgb(210, 230, 255));}"
                              );

    connect(p_treeView,&QTreeView::clicked,this,&MainWindow::onTreeviewClicked);
    p_treeView->setFixedSize(desk_rect.width()/7-5,desk_rect.height()/10*9-5);
    p_treeStandarModel = new QStandardItemModel(p_treeView);
    p_treeView->setModel(p_treeStandarModel);
    p_treeView->setFont(QFont("宋体",12));
    p_treeView->header()->hide();
    p_treeView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    p_standarItem = new QStandardItem("用户管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/user.png"));
    p_ordersItem = new QStandardItem("生成任务");
    p_loginroleItem = new  QStandardItem("登录管理");

    p_standarItem->appendRow(p_ordersItem);
    p_standarItem->appendRow(p_loginroleItem);

    p_treeStandarModel->appendRow(p_standarItem);
    p_standarItem = new QStandardItem("设备管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/devices.png"));
    p_agvItem = new QStandardItem("小车管理");
    p_agvItem->setIcon(QIcon(":/resouse/Image/car.png"));
    p_standarItem->appendRow(p_agvItem);
    p_elevator = new QStandardItem("电梯管理");
    p_elevator->setIcon(QIcon(":/resouse/Image/elevator.png"));
    p_standarItem->appendRow(p_elevator);
    p_runer = new QStandardItem("流道管理");
    p_runer->setIcon(QIcon(":/resouse/Image/elevator.png"));
    p_standarItem->appendRow(p_runer);
    p_stacker = new QStandardItem("堆垛机管理");
    p_stacker->setIcon(QIcon(":/resouse/Image/elevator.png"));
    p_standarItem->appendRow(p_stacker);

    p_stackerRunner = new QStandardItem("堆垛机流道管理");
    p_stackerRunner->setIcon(QIcon(":/resouse/Image/elevator.png"));
    p_standarItem->appendRow(p_stackerRunner);


    p_treeStandarModel->appendRow(p_standarItem);
    p_standarItem = new QStandardItem("货架管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/warehouset.png"));
    p_treeStandarModel->appendRow(p_standarItem);
    p_standarItem = new QStandardItem("报警管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/alarm_bell.png"));
    p_treeStandarModel->appendRow(p_standarItem);
    p_standarItem = new QStandardItem("日志管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/log.png"));
    p_treeStandarModel->appendRow(p_standarItem);

    p_standarItem = new QStandardItem("任务管理");
    p_standarItem->setIcon(QIcon(":/resouse/Image/tasks.png"));
    p_all_tasks = new QStandardItem("已完成任务");
    p_all_tasks->setIcon(QIcon(":/resouse/Image/all_tasks.png"));
    p_standarItem->appendRow(p_all_tasks);

    p_current_tasks = new QStandardItem("当前任务");
    p_current_tasks->setIcon(QIcon(":/resouse/Image/current_tasks.png"));
    p_standarItem->appendRow(p_current_tasks);
    p_sub_tasks = new QStandardItem("子任务");
    p_sub_tasks->setIcon(QIcon(":/resouse/Image/current_tasks.png"));
    p_standarItem->appendRow(p_sub_tasks);
    p_treeStandarModel->appendRow(p_standarItem);

    p_dispatch = new QStandardItem("手动调度");
    p_dispatch->setIcon(QIcon(":/resouse/Image/current_tasks.png"));
    p_treeStandarModel->appendRow(p_dispatch);

    p_main_from = new QStandardItem("信息统计");
    p_main_from->setIcon(QIcon(":/resouse/Image/log.png"));
    p_treeStandarModel->appendRow(p_main_from);

    p_map = new QStandardItem("地图构建");
    p_map->setIcon(QIcon(":/resouse/Image/log.png"));
    p_treeStandarModel->appendRow(p_map);

    //    m_pmode_info_label = new QLabel(this);
    //    m_pmode_info_label->setText("now is auto model");
    //    m_pmode_info_label->move(desk_rect.width()*0.6,desk_rect.height()/22);
    int butn_height = int(desk_rect.height()/10*0.5);
    int butn_width = int(desk_rect.width()/11*0.5);
    m_pmode_btn = new QPushButton("手动",this);
    connect(m_pmode_btn,&QPushButton::clicked,this,&MainWindow::changeMode);
    m_pmode_btn->setIcon(QIcon(":/resouse/Image/hand.ico"));
    m_pmode_btn->resize(butn_width,butn_height);
    m_pmode_btn->move(int(desk_rect.width()*0.75)-40,desk_rect.height()/22);


    m_psuspend = new QPushButton("暂停ing",this);
    connect(m_psuspend,&QPushButton::clicked,this,&MainWindow::onSuspend);
    m_psuspend->setIcon(QIcon(":/resouse/Image/player_pause.ico"));
    m_psuspend->resize(butn_width,butn_height);
    m_psuspend->move(int(desk_rect.width()*0.8)-20,desk_rect.height()/22);

    user_btn = new QPushButton("用户",this);
    user_btn->setIcon(QIcon(":/resouse/Image/user.png"));
    user_btn->move(int(desk_rect.width()*0.85),desk_rect.height()/22);
    user_btn->resize(butn_width,butn_height);
    connect(user_btn,&QPushButton::clicked,this,&MainWindow::slotlogin);
    exit_btn = new QPushButton("安全退出",this);
    exit_btn->resize(butn_width,butn_height);
    connect(exit_btn,&QPushButton::clicked,this,&MainWindow::closeWcs);
    exit_btn->setIcon(QIcon(":/resouse/Image/shutdown.png"));
    exit_btn->move(int(desk_rect.width()*0.9)+20,desk_rect.height()/22);

}

void MainWindow::loadCarInitialLayer()
{
    for(auto it = Myconfig::GetInstance()->m_CarMap.begin();it != Myconfig::GetInstance()->m_CarMap.end();it++)
    {
        LayerStru l;
        l.carId = it.key();
        l.layerstate = InLayer;
         Myconfig::GetInstance()->m_layerStatusMap.insert(it.value().deveceStatus.carCurrentPosion.z,l);
    }
}

void MainWindow::checkReidsConnected()
{
    RedisBaseModule redisConnect(Myconfig::GetInstance()->m_redisInfo.ip.toStdString().c_str(),Myconfig::GetInstance()->m_redisInfo.port);
    if(redisConnect.getConnect())
    {
        Myconfig::GetInstance()->m_redisInfo.state = 0;
        GetSystemLogObj()->writeLog("Redis 启动成功",0);
    }
    else
    {
        GetSystemLogObj()->writeLog("Redis 启动失败",2);
    }
}


void MainWindow::deleteChildrenList()
{
    //    QList<BaseFrom *> list = p_main_widget->findChildren<BaseFrom*>();
    //    if(list.size() == 0)
    //        return;
    //    foreach (BaseFrom* w, list) {
    //        w->hide();
    //        w->deleteLater();
    //    }
    t->hide();
    m_pLog->hide();
    car_from->hide();
    m_pTaskAll->hide();
    p_mElevator->hide();
    m_pstoreWg->hide();
    m_palarmWg->hide();
    m_pSubTask->hide();
    m_pmonitorui->hide();
    p_mRunerForm->hide();
    m_testorders->hide();
    m_pDispatchForm->hide();
    m_pMainWidget->hide();
    p_mUserManagerWg->hide();
    m_stackerWg->hide();
    m_stackerRunnerWg->hide();
}

void MainWindow::getParameterFromDB()
{
    ReadTableData r;
    r.readt_elevator();
    r.readt_crrunt_task();
    r.readt_runerInfo();
    r.Readt_hwcomm_infotable();
    r.readt_device_info();
    r.ReadStoreposinfoDataBase();
    //qDebug()<<Myconfig::GetInstance()->m_CarMap.size();
    r.readt_codePosition();
}

void MainWindow::getConfigParameter()
{
    QString currPath = QCoreApplication::applicationDirPath();
    QStringList list = currPath.split("/");
    if(list.size() > 1)
        list.removeAt(list.size()-1);
    QString absPath = list.join("/");
    MyIniConfig f;
    f.Config(absPath+"/config.ini");
    Myconfig::GetInstance()->m_databaseInfo.databaseName = f.Get("DataBase","databaseName").toString();
    Myconfig::GetInstance()->m_databaseInfo.ip = f.Get("DataBase","ip").toString();
    Myconfig::GetInstance()->m_databaseInfo.passWord = f.Get("DataBase","passWord").toString();
    Myconfig::GetInstance()->m_databaseInfo.port = f.Get("DataBase","port").toInt();
    Myconfig::GetInstance()->m_databaseInfo.sqlName = f.Get("DataBase","sqlName").toString();
    Myconfig::GetInstance()->m_databaseInfo.userName = f.Get("DataBase","userName").toString();
    Myconfig::GetInstance()->m_pixel = f.Get("MapConfig","pixel").toInt();
    Myconfig::GetInstance()->outOfWarehouseUrl = f.Get("WMS","outOfWarehouse").toString();
    Myconfig::GetInstance()->inOfWareHouseUrl = f.Get("WMS","inOfWarehouse").toString();
    Myconfig::GetInstance()->m_byteorder = f.Get("Byteorder","order").toString();
    Myconfig::GetInstance()->m_redisInfo.ip = f.Get("Redis","ip").toString();
    Myconfig::GetInstance()->m_redisInfo.port = f.Get("Redis","port").toInt();
    Myconfig::GetInstance()->m_SrmReadPrefix = f.Get("SRM","SrmReadPrefix").toString();
    Myconfig::GetInstance()->m_SrmWritePrefix = f.Get("SRM","SrmWritePrefix").toString();
    qDebug()<<"[read fix]"<<Myconfig::GetInstance()->m_SrmReadPrefix<<"[write]"<<Myconfig::GetInstance()->m_SrmWritePrefix;
}

void MainWindow::delay_msc(int msc)
{
    QEventLoop loop;
    QTimer::singleShot(msc,&loop,SLOT(quit()));
    loop.exec();
}

QByteArray MainWindow::getReply()
{
    QVariantHash data;
    data.insert("success","true");
    data.insert("code","200");
    data.insert("msg","WCSReply");
    QJsonObject rootObj = QJsonObject::fromVariantHash(data);
    QJsonDocument document;
    document.setObject(rootObj);
    return document.toJson(QJsonDocument::Compact);
}

void MainWindow::onTreeviewClicked(const QModelIndex &index)
{
    deleteChildrenList();
    QString row_name = index.data().toString();
    int level = Myconfig::GetInstance()->m_curLoginlevel;
    if(row_name == "登录管理" /*&& level < 1*/)
    {
        //        //qDebug()<<"handle 权限管理...";
        //        m_pmonitorui->show();
        p_mUserManagerWg->show();
    }
    else if(row_name == "生成任务")
    {
        m_testorders->show();
    }
    else if(row_name == "报警管理")
    {
        m_palarmWg->show();
    }
    else if((row_name == "小车管理" || row_name == "设备管理")&& level < 2)
    {
        car_from->refreshTable();
        car_from->show();
    }
    else if(row_name == "货架管理")
    {
        //增加货架管理信息的界面
        m_pstoreWg->show();
    }
    else if(row_name == "电梯管理"&& level < 2)
    {
        p_mElevator->show();
    }
    else if(row_name == "流道管理"&& level < 2)
    {
        p_mRunerForm->show();
    }
    else if(row_name == "堆垛机管理"&& level < 2)
    {
        m_stackerWg->show();
    }
    else if(row_name == "堆垛机流道管理"&& level < 2)
    {
        m_stackerRunnerWg->show();
    }
    else if(row_name == "当前任务")
    {
        t->refreshTable();
        t->show();
    }
    else if(row_name == "已完成任务"||row_name == "任务管理")
    {
        m_pTaskAll->show();
    }
    else if(row_name == "子任务")
    {
        m_pSubTask->show();
    }
    else if(row_name == "日志管理")
    {
        m_pLog->show();
    }
    else if(row_name == "手动调度"&& level < 2)
    {
        m_pDispatchForm->show();
    }
    else if(row_name == "信息统计")
    {
        m_pMainWidget->show();
    }
    else if(row_name == "地图构建")
    {
        m_pmonitorui->show();
    }
}

void MainWindow::slotlogin()
{
    LoginInfoWg login;
    if (login.exec() == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回，继续执行主控制流
    {
        //获取当前用户信息和级别
        user_btn->setText(login.GetUserName());
    }
}

void MainWindow::onReplyReady(QString str)
{
    static int i = 1;
    qDebug()<<"in Warehouse Control System"<<QString::number(i);
    i++;
    qDebug()<<"rechttp:"<<str;
    emit httpRedReady(str);
}

void MainWindow::showCarList()
{
    m_list_lab->show();
    m_listIsShow = !m_listIsShow;
}

void MainWindow::changeMode()
{
    if(m_pmode_btn->text() == "手动")
    {
        m_pmode_btn->setText("自动");
        m_pmode_btn->setIcon(QIcon(":/resouse/Image/auto.ico"));
        Myconfig::GetInstance()->m_run_mode = 1;
    }
    else
    {
        m_pmode_btn->setText("手动");
        m_pmode_btn->setIcon(QIcon(":/resouse/Image/hand.ico"));
        Myconfig::GetInstance()->m_run_mode = 0;
    }
}
///
/// \brief MainWindow::onSuspend
/// 调度线程停止\开始分配任务
///
void MainWindow::onSuspend()
{
    Myconfig::GetInstance()->m_flag = !Myconfig::GetInstance()->m_flag;
    if(m_psuspend->text() == "暂停ing")
    {
        m_psuspend->setText("启动ing");
        Myconfig::GetInstance()->m_run_mode = 0;
        m_psuspend->setIcon(QIcon(":/resouse/Image/player_play.ico"));
        m_psuspend->setStyleSheet("background-color:rgb(250,240,230);color:white");
        emit dispatched();
    }
    else
    {
        Myconfig::GetInstance()->m_run_mode = 1;
        m_psuspend->setText("暂停ing");
        m_psuspend->setIcon(QIcon(":/resouse/Image/player_pause.ico"));
        m_psuspend->setStyleSheet("background-color:rgb(160,160,160);color:green");
    }
}

void MainWindow::slotCommError(QString info) // 把信息打印到界面中
{
    m_pMainWidget->appLog(info);
}

void MainWindow::initRunnerD2()
{
    OrderStru runnerstru;
    runnerstru.childtype = 2;
    runnerstru.Datatype = 168; //寄存器 读写
    runnerstru.startaddress = 2;
    runnerstru.numberOfEntries = 1;
    runnerstru.values.append(0);
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,9);//给出流道缓存区位置

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{

    if(event->pos().x() >= desk_rect.width()*0.85 && event->pos().y() <= 4)
    {
        m_list_lab->hide();
        p_mCarList->show();
    }
    QWidget::mouseMoveEvent(event);
}
