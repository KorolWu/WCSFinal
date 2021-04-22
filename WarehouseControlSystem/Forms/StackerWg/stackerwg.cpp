#include "stackerwg.h"
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextCharFormat>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTreeView>
#include <QHeaderView>
#include <QOpcUaProvider>
#include <QMetaEnum>
#include <QPixmap>
#include <QOpcUaReadResult>
#include <QOpcUaReadItem>
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"
#include <Myconfig.h>
#include <QGridLayout>
#include <QSplitter>

const QString stylestr =  "QPushButton{font:12px;background-color:rgb(150,150,150)}"
                          "QLabel{font:12px;}"
                          "QPushButton:hover{background-color:rgba(220,220,220,110);}"
                          "QPushButton:pressed{background-color:rgba(85,170,255,150);}"
                          "QTableView{font:12px;selection-background-color:rgba(68,69,71,100);alternate-background-color: rgb(220, 220, 220);}"
                          "QMenu::item:selected {background-color:rgba(0,190,230,100);}";
StackerWg::StackerWg(QWidget *parent): QWidget(parent)
  , mconnectlabel(new QLabel("",this))
  , mIdlabel(new QLabel("硬件ID",this))
  , mConnectButton(new QPushButton(tr("Connect"), this))
  , mServerUrl(new QLineEdit(this))
  , mOpcUaPlugin(new QComboBox(this))
  , mRefreshButton(new QPushButton(tr("refresh"), this))
  , mLog(new QPlainTextEdit(this))
  , m_nodeidEdit(new QLineEdit(this))
  , mDatatypecombox(new QComboBox(this))
  , m_valueEdit(new QLineEdit(this))
  , m_setSourceEdit(new QLineEdit(this))
  , m_setTraEdit(new QLineEdit(this))
  , mReadButton(new QPushButton(tr("读变量"), this))
  , mWriteButton(new QPushButton(tr("写变量"), this))
  , mcmdButton(new QPushButton(tr("执行指令"), this))
  , mEmpButton(new QPushButton(tr("急停"), this))
  , mClrAlarmButton(new QPushButton(tr("报警清除"), this))
  , mTestButton(new QPushButton(tr("调试"), this))
  , mCancelTaskButton(new QPushButton(tr("取消任务"), this))
  , mTreeView(new QTreeView(this))
  , mOpcUaModel(new OpcUaModel(this))
  , mOpcUaProvider(new QOpcUaProvider(this))
  , mClientConnected(false)
{
    mOpcUaClient = nullptr;
    mstackercmd = new StackerInstruction();
    mrunthread = new QThread();
    mstackercmd->moveToThread(mrunthread);
    connect(this,&StackerWg::signalRun,mstackercmd,&StackerInstruction::Run);
    mrunthread->start();
    //界面数据布局规划
    auto hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel("状态:"));
    hbox->addWidget(mconnectlabel);
    hbox->addWidget(mIdlabel);
    hbox->addWidget(mOpcUaPlugin);
    hbox->addWidget(new QLabel("OPCURL:"));
    hbox->addWidget(mServerUrl);
    hbox->addWidget(mConnectButton);
    hbox->addWidget(mRefreshButton);
    hbox->addWidget(new QSplitter(this));
    m_setSourceEdit->setText("2,3,2");
    m_setTraEdit->setText("2,3,2");
    //需要监视变量数据
    QStringList list;
    list<<"心跳信号"<<"报警信号"<<"载台有货"<<"取货完成"<<"放货完成"<<"任务完成"<<"ack";
    QStringList valuelist;
    valuelist<<"堆垛机编号"<<"当前任务号"<<"工作模式"<<"报警代码"<<"执行状态";
    auto gridbox = new QGridLayout;
    for(int i = 0; i < list.size(); ++i)
    {
        QString showstr = list[i]+":";
        gridbox->addWidget(new QLabel(showstr),0,i*2,1,1,Qt::AlignLeft);
        QLabel *lable = new QLabel();
        gridbox->addWidget(lable,0,i*2+1,1,1,Qt::AlignLeft);
        QPixmap iconconnect(":/resouse/Image/grey.png");
        iconconnect = iconconnect.scaled(15,15);
        lable->setPixmap(iconconnect);
        m_RnamesMap.insert(list[i],lable);
    }
    for(int i = 0; i < valuelist.size(); ++i)
    {
        QString showstr = valuelist[i]+":";
        gridbox->addWidget(new QLabel(showstr),2,i*2,2,1,Qt::AlignLeft);
        QLabel *lable = new QLabel();
        gridbox->addWidget(lable,2,i*2+1,2,1,Qt::AlignLeft);
        lable->setStyleSheet("font:18px;color:rgb(50,100,220)");
        m_RnamesMap.insert(valuelist[i],lable);
    }
    valuelist.clear();
    valuelist<<"当前列X"<<"当前层Y"<<"当前货叉位Z"<<"当前巷道";
    for(int i = 0; i < valuelist.size(); ++i)
    {
        QString showstr = valuelist[i]+":";
        gridbox->addWidget(new QLabel(showstr),4,i*2);
        QLabel *lable = new QLabel();
        gridbox->addWidget(lable,4,i*2+1,1,1,Qt::AlignLeft);
        lable->setStyleSheet("font:18px;color:rgb(50,100,220)");
        m_RnamesMap.insert(valuelist[i],lable);
    }
    auto ophbox = new QHBoxLayout;
    ophbox->addWidget(new QLabel(tr("变量ID:")));
    ophbox->addWidget(m_nodeidEdit);
    QStringList typelist;
    typelist<<"Boolean"<<"Int32"<<"UInt32"<<"Double"<<"Float"<<"String"<<"LocalizedText"<<"DateTime"\
           <<"UInt16"<<"Int16"<<"UInt64"<<"Int64";
    mDatatypecombox->addItems(typelist);
    ophbox->addWidget(mDatatypecombox);
    ophbox->addWidget(new QLabel(tr("value:")));
    ophbox->addWidget(m_valueEdit);
    ophbox->addWidget(mReadButton);
    ophbox->addWidget(mWriteButton);
    ophbox->addWidget(mEmpButton);
    ophbox->addWidget(mClrAlarmButton);
    ophbox->addWidget(new QSplitter(this));

    auto cmdhbox = new QHBoxLayout;
    cmdhbox->addWidget(new QLabel(tr("设置源位置:")));
    cmdhbox->addWidget(m_setSourceEdit);
    cmdhbox->addWidget(new QLabel(tr("设置目标位置:")));
    cmdhbox->addWidget(m_setTraEdit);
    cmdhbox->addWidget(mcmdButton);
    cmdhbox->addWidget(mTestButton);
    cmdhbox->addWidget(mCancelTaskButton);
    cmdhbox->addWidget(new QSplitter(this));

    auto vbox = new QVBoxLayout;
    vbox->addLayout(hbox,1);
    vbox->addWidget(mTreeView,8);
    vbox->addWidget(new QLabel(tr("监视:")),1);
    vbox->addLayout(gridbox,4);
    vbox->addWidget(new QLabel(tr("operate:")),1);
    vbox->addLayout(ophbox);
    vbox->addWidget(new QLabel(tr("指令执行:")),1);
    vbox->addLayout(cmdhbox);
    vbox->addWidget(new QLabel(tr("Log:")),1);
    vbox->addWidget(mLog,3);

    mServerUrl->setText("opc.tcp://127.0.0.1:43344");
    //opc.tcp://118.24.36.220:62547/DataAccessServer
    mOpcUaPlugin->addItems(mOpcUaProvider->availableBackends());
    mLog->setReadOnly(true);
    mLog->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->setFixedWidth(parent->width());
    mTreeView->setModel(mOpcUaModel);
    mTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTreeView->setTextElideMode(Qt::ElideRight);
    mTreeView->setAlternatingRowColors(true);
    mTreeView->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->setLayout(vbox);
    if (mOpcUaPlugin->count() == 0) {
        mOpcUaPlugin->setDisabled(true);
        mConnectButton->setDisabled(true);
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));
    }
    //connect(mConnectButton, &QPushButton::clicked, this, &StackerWg::connectToServer);
    connect(mRefreshButton, &QPushButton::clicked, this, &StackerWg::SlotrefreshTree);
    connect(mReadButton, &QPushButton::clicked, this, &StackerWg::SlotReadVariable);
    connect(mWriteButton, &QPushButton::clicked, this, &StackerWg::SlotWriteVariable);
    connect(mcmdButton, &QPushButton::clicked, this, &StackerWg::SlotExcCmd);
    connect(mEmpButton, &QPushButton::clicked, this, &StackerWg::SlotExcEmpcmd);
    connect(mClrAlarmButton, &QPushButton::clicked, this, &StackerWg::SlotExcClrAlarmcmd);
    connect(mTestButton, &QPushButton::clicked, this, &StackerWg::SlotExcTestCmd);
    connect(mCancelTaskButton, &QPushButton::clicked, this, &StackerWg::SlotExcCancelTaskcmd);
    m_updateTimer = new QTimer();
    connect(m_updateTimer,&QTimer::timeout,this,&StackerWg::SlotUpdateState);
    m_updateTimer->start(200);
    mRefreshButton->setIcon(QIcon(":/resouse/Image/Refresh.png"));
    QPixmap iconconnect(":/resouse/Image/grey.png");
    iconconnect = iconconnect.scaled(15,15);
    mconnectlabel->setPixmap(iconconnect);
    mServerUrl->setFixedWidth(180);
    mEmpButton->setStyleSheet("font:12px;color:rgb(200,50,50)");
    mEmpButton->setIcon(QIcon(":/resouse/Image/empstop.png"));
    mClrAlarmButton->setStyleSheet("font:12px;color:rgb(0,200,50)");
    m_commID = -1;
    this->setStyleSheet(stylestr);
}
///
/// \brief StackerWg::~StackerWg
///
StackerWg::~StackerWg()
{
    StopStacker();
    if(mrunthread->isRunning())
    {
        mrunthread->quit();
        mrunthread->wait();
    }
    m_updateTimer->stop();
    m_RnamesMap.clear();
}
///
/// \brief StackerWg::setOpcUaClient
/// \param client
///
void StackerWg::setOpcUaClient(QOpcUaClient * client)
{
    mOpcUaClient = client;
}
///
/// \brief StackerWg::SetOPCCommID
/// \param ID
///
void StackerWg::SetOPCCommID(int ID)
{
    //根据对象设置显示的数据
    TCommOPCUAClient *ob = nullptr;
    if(TCommtransceivermanager::GetInstance()->GetCommOb(ID) != nullptr)
    {
        if(TCommtransceivermanager::GetInstance()->GetCommOb(ID)->GetHWprotype() == 8)
        {
            ob = dynamic_cast<TCommOPCUAClient *>(TCommtransceivermanager::GetInstance()->GetCommOb(ID));
        }
    }
    if(ob != nullptr )
    {
        mIdlabel->setText("ID:"+QString::number(ID));
        if(Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.contains(ID))
        {
            QString name ="";
            name = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap[ID].name;
            if(name != "")
            {
                mServerUrl->setText(name);
            }
            mOpcUaClient = ob->mOpcUaClient;
            m_commID = ID;
            connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &StackerWg::clientDisconnected);
            connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &StackerWg::clientError);
            connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &StackerWg::clientState);
            //                connect(mOpcUaClient,&QOpcUaClient::batchReadFinished,this, &MainWindow::readFinished);
            //             connect(mOpcUaClient,&QOpcUaClient::batchWriteFinished,this, &MainWindow::WriteFinished);
        }
    }

}

void StackerWg::connectToServer()
{
    if(mOpcUaClient)
        return;
    if (mClientConnected) {
        mOpcUaClient->disconnectFromEndpoint();
        return;
    }
    mOpcUaClient = mOpcUaProvider->createClient(mOpcUaPlugin->currentText());
    if (!mOpcUaClient) {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        // log(message, QString(), Qt::red);
        QMessageBox::critical(this, tr("Failed to connect to server"), message);
        return;
    }
    mOpcUaClient->connectToEndpoint(mServerUrl->text());
}

void StackerWg::clientDisconnected()
{
    QString str = "client Disconnected";
    //qDebug() << "client Disconnected";
    mLog->appendPlainText(str);
}

void StackerWg::clientError(QOpcUaClient::ClientError error)
{
    QString str = "Client error changed"  + QString::number(error) ;
    //qDebug() << "Client error changed" << error;
    mLog->appendPlainText(str);
}

void StackerWg::clientState(QOpcUaClient::ClientState state)
{
    QString str = "Client state changed value: " + QString::number(state);
    qDebug() << "Client state changed" << state;
    mLog->appendPlainText(str);
}

void StackerWg::SlotUpdateState()
{
    updateUiState();
}
///
/// \brief StackerWg::SlotrefreshTree
///
void StackerWg::SlotrefreshTree()
{
    //连接界面进行刷新操作
    if(mClientConnected)
    {
        mOpcUaModel->setOpcUaClient(mOpcUaClient);
        mTreeView->header()->setSectionResizeMode(1 /* Value column*/, QHeaderView::Interactive);
    }
}
///
/// \brief StackerWg::SlotReadVariable
///
void StackerWg::SlotReadVariable()
{
    //得到变量的名字
    QString name =  m_nodeidEdit->text();
    OrderStru stru;
    stru.childtype = 5;
    stru.Rcmdstackernames.append(name);
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(stru,m_commID);
}
///
/// \brief StackerWg::SlotWriteVariable
///
void StackerWg::SlotWriteVariable()
{
    QString name =  m_nodeidEdit->text();
    int type = mDatatypecombox->currentIndex();
    OrderStru stru;
    stru.childtype = 1;
    stru.Datatype = type;
    QVariant value;
    value = GetDatatypeValue(mDatatypecombox->currentText(),m_valueEdit->text());
    QMap<QString,QVariant> valuemap;
    valuemap.insert(name,value);
    stru.WcmdstackerValues = valuemap;
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(stru,m_commID);
}
///
/// \brief StackerWg::SlotExcCmd
///执行指令按钮 搬运指令执行
void StackerWg::SlotExcCmd()
{
    //获取设置指令位置的按钮
    OrderStru cmsstru;
    cmsstru.order = Order::Stacker_Carry;
    QStringList sourcelist;
    sourcelist = m_setSourceEdit->text().split(",");
    QStringList tralist = m_setTraEdit->text().split(",");
    if(sourcelist.size() == 3 && tralist.size() == 3)
    {
        cmsstru.sourcePos.x = sourcelist[0].toInt();
        cmsstru.sourcePos.y = sourcelist[1].toInt();
        cmsstru.sourcePos.z = sourcelist[2].toInt();

        cmsstru.destinationPos.x = tralist[0].toInt();
        cmsstru.destinationPos.y = tralist[1].toInt();
        cmsstru.destinationPos.z = tralist[2].toInt();
    }
    else{
        mLog->appendPlainText("源位置坐标和目标位置请用‘，’分割XYZ值");
        return;
    }
     StopStacker();
     QString strmsg = "";
     mstackercmd->setParameter(cmsstru,m_commID);
     strmsg = "搬运指令执行中...";
     mLog->appendPlainText(strmsg);
    //emit signalRun(); //放在线程中处理
     mstackercmd->runInstruction();  
     mstackercmd->getResult(strmsg);
     mLog->appendPlainText(strmsg);
}
///
/// \brief StackerWg::SlotExcEmpcmd
///
void StackerWg::SlotExcEmpcmd()
{
     StopStacker();
    QString strmsg = "";
    strmsg = "发送急停指令...";
    QString estop ="ns=3;s=\"F_WCS_DB\".\"emergencyStop\"";  //"ns=3;s=\"F_WCS_DB\".\"alarmAck\""
    SetVarValue(1,estop,QOpcUa::Types::Int16);
    mLog->appendPlainText(strmsg);
}
///
/// \brief StackerWg::SlotExcClrAlarmcmd
///
void StackerWg::SlotExcClrAlarmcmd()
{
    StopStacker();
    QString strmsg = "";
    strmsg = "发送清除报警指令...";
    QString alarm = "ns=3;s=\"F_WCS_DB\".\"alarmAck\"";
    SetVarValue(1,alarm,QOpcUa::Types::Int16);
    mLog->appendPlainText(strmsg);
}
///
/// \brief StackerWg::SlotExcTestCmd
///
void StackerWg::SlotExcTestCmd()
{
    //测试程序
    //获取设置指令位置的按钮
    OrderStru startcmsstru;
    startcmsstru.order = Order::Stacker_Carry;
    startcmsstru.sourcePos.x = 2;
    startcmsstru.sourcePos.y = 3;
    startcmsstru.sourcePos.z = 2;
    startcmsstru.destinationPos.x =22;
    startcmsstru.destinationPos.y = 5;
    startcmsstru.destinationPos.z = 1;
     OrderStru endcmsstru;
    endcmsstru.order = Order::Stacker_Carry;
    endcmsstru.sourcePos.x = 22;
    endcmsstru.sourcePos.y = 5;
    endcmsstru.sourcePos.z = 1;
    endcmsstru.destinationPos.x =2;
    endcmsstru.destinationPos.y = 3;
    endcmsstru.destinationPos.z = 2;
    //bool isstart = true;
    int j = 0;

    OrderStru ostartcmsstru;
    ostartcmsstru.order = Order::Stacker_Carry;
    ostartcmsstru.sourcePos.x = 21;
    ostartcmsstru.sourcePos.y = 1;
    ostartcmsstru.sourcePos.z = 1;
    ostartcmsstru.destinationPos.x =1;
    ostartcmsstru.destinationPos.y = 1;
    ostartcmsstru.destinationPos.z = 1;
     OrderStru oendcmsstru;
    oendcmsstru.order = Order::Stacker_Carry;
    oendcmsstru.sourcePos.x = 1;
    oendcmsstru.sourcePos.y = 1;
    oendcmsstru.sourcePos.z = 1;
    oendcmsstru.destinationPos.x =21;
    oendcmsstru.destinationPos.y = 1;
    oendcmsstru.destinationPos.z = 1;



    for(int i = 0; i < 20;++i)
    {
        StopStacker();
        QString strmsg = "";
        switch (j) {
        case 0:
        {
            mstackercmd->setParameter(startcmsstru,m_commID);
            j = 1;
            break;
        }
        case 1:
        {
            mstackercmd->setParameter(ostartcmsstru,m_commID);
            j = 2;
            break;
        }
        case 2:
        {
            mstackercmd->setParameter(endcmsstru,m_commID);
            j = 3;
            break;
        }
        case 3:
        {
            mstackercmd->setParameter(oendcmsstru,m_commID);
            j = 0;
            break;
        }
        default:
            break;
        }
//        if(isstart)
//        {
//            mstackercmd->setParameter(startcmsstru,m_commID);
//            isstart = false;
//        }
//        else{
//            mstackercmd->setParameter(ostartcmsstru,m_commID);
//            isstart = true;
//        }
        strmsg = "搬运指令执行中...";
        mLog->appendPlainText(strmsg);
        //emit signalRun(); //放在线程中处理
        mstackercmd->runInstruction();
        int result = mstackercmd->getResult(strmsg);
        mLog->appendPlainText(strmsg);
        if(result < 0 )
            break;
        QEventLoop loop;//定义一个新的事件循环
        QTimer::singleShot(5000, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
        loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    }
}
///
/// \brief StackerWg::SlotExcCancelTaskcmd
/// 主要 用于当前任务出现异常之后，重新让堆垛机回复正常可接收正常任务
void StackerWg::SlotExcCancelTaskcmd()
{
    StopStacker();
    QString strmsg = "";
    strmsg = "发送任务取消指令...";
    QString alarm = "ns=3;s=\"F_WCS_DB\".\"Task_Finish_ACK\"";
    SetVarValue(1,alarm,QOpcUa::Types::Int16);
    mLog->appendPlainText(strmsg);
}

///
/// \brief StackerWg::updateUiState
///
void StackerWg::updateUiState()
{
    if(!Myconfig::GetInstance()->m_stackerMap.contains(m_commID))
        return;
    if(mClientConnected != Myconfig::GetInstance()->m_stackerMap[m_commID].online)
    {
        mClientConnected = Myconfig::GetInstance()->m_stackerMap[m_commID].online;
        mConnectButton->setText(mClientConnected?tr("Disconnect"):tr("Connect"));
        mOpcUaPlugin->setDisabled(mClientConnected);
        mServerUrl->setDisabled(mClientConnected);
        if(!mClientConnected)
        {
            QPixmap iconconnect(":/resouse/Image/grey.png");
            iconconnect = iconconnect.scaled(15,15);
            mconnectlabel->setPixmap(iconconnect);
        }
        else{
            QPixmap iconconnect(":/resouse/Image/green.png");
            iconconnect = iconconnect.scaled(15,15);
            mconnectlabel->setPixmap(iconconnect);
        }
    }
    //读数据更新内容
    UpateShowParam();
}
///
/// \brief StackerWg::UpateShowParam
/// \param role
/// \param value
///
void StackerWg::UpateShowParam(QString key , QString valuestr,bool istext)
{
    if(!m_RnamesMap.contains(key))
        return;
    if(istext)
    {
        if(m_RnamesMap[key]->text() != valuestr)
        {
            m_RnamesMap[key]->setText(valuestr);
        }
    }
    else{
        if(valuestr.toInt() == 0)
        {
            QPixmap iconconnect(":/resouse/Image/grey.png");
            iconconnect = iconconnect.scaled(15,15);
            m_RnamesMap[key]->setPixmap(iconconnect);
        }
        else{
            QPixmap iconconnect(":/resouse/Image/green.png");
            iconconnect = iconconnect.scaled(15,15);
            m_RnamesMap[key]->setPixmap(iconconnect);
        }
    }
}
///
/// \brief StackerWg::UpateShowParam
///
///
void StackerWg::UpateShowParam()
{
    StackerRead Rstru = Myconfig::GetInstance()->m_stackerMap[m_commID].Stacker_r;
    // BOOL 更新赋值
    QString modestr = "";
    modestr = GetModeValueStr(Rstru.mode);
    UpateShowParam("工作模式",modestr);
    modestr = QString::number(Rstru.posX);
    UpateShowParam("当前列X",modestr);
    modestr = QString::number(Rstru.posY);
    UpateShowParam("当前层Y",modestr);
    modestr = QString::number(Rstru.posZ);
    UpateShowParam("当前货叉位Z",modestr);
    modestr = QString::number(Rstru.alarm);
    UpateShowParam("报警信号",modestr,false);
    modestr = QString::number(Rstru.srmNo);
    UpateShowParam("堆垛机编号",modestr);
    modestr = GetStateValueStr(Rstru.state);
    UpateShowParam("执行状态",modestr);
    modestr = QString::number(Rstru.TaskNo);
    UpateShowParam("当前任务号",modestr);
    modestr = QString::number(Rstru.liftFull);
    UpateShowParam("载台有货",modestr,false);
    modestr = QString::number(Rstru.handShake);
    UpateShowParam("心跳信号",modestr,false);
    modestr = QString::number(Rstru.ActualLine);
    UpateShowParam("当前巷道",modestr);
    modestr = QString::number(Rstru.pickFinish);
    UpateShowParam("取货完成",modestr,false);
    modestr = QString::number(Rstru.taskFinish);
    UpateShowParam("任务完成",modestr,false);
    modestr = QString::number(Rstru.Alarm_number);
    UpateShowParam("报警代码",modestr);
    modestr = QString::number(Rstru.deliveryFinish);
    UpateShowParam("放货完成",modestr,false);
    modestr = QString::number(Rstru.ack);
    UpateShowParam("ack",modestr,false);

}
///
/// \brief StackerWg::GetModeValueStr
/// \param value
/// \return
///
QString StackerWg::GetModeValueStr(qint16 value)
{
    QString modestr = "关机";
    switch (value) {
    case 0:
    {
        modestr = "关机";
        break;
    }
    case 1:
    {
        modestr = "自动";
        break;
    }
    case 2:
    {
        modestr = "手动";
        break;
    }
    case 3:
    {
        modestr = "半自动";
        break;
    }
    case 4:
    {
        modestr = "维修";
        break;
    }
    default:
        break;
    }
    return  modestr;
}
//srm当前任务执行状态 =0空闲 =1取货定位中 =2请求取货 =3取货中 =4取货完成放货定位中 =5请求放货 =6放货中 =99报警
QString StackerWg::GetStateValueStr(qint16 value)
{
    QString statestr = "";
    switch (value) {
    case 0:
    {
        statestr = "空闲";
        break;
    }
    case 1:
    {
        statestr = "取货定位中";
        break;
    }
    case 2:
    {
        statestr = "请求取货";
        break;
    }
    case 3:
    {
        statestr = "取货中";
        break;
    }
    case 4:
    {
        statestr = "取货完成放货定位中";
        break;
    }
    case 5:
    {
        statestr = "请求放货";
        break;
    }
    case 6:
    {
        statestr = "放货中";
        break;
    }
    case 99:
    {
        statestr = "报警";
        break;
    }
    default:
        break;
    }
    return  statestr;
}
///
/// \brief StackerWg::GetDatatypeValue
/// \param typestr
/// \return
///
///     //operate 区域界面
//Boolean         = 0,
//Int32           = 1,
//UInt32          = 2,
//Double          = 3,
//Float           = 4,
//String          = 5,
//LocalizedText   = 6,
//DateTime        = 7,
//UInt16          = 8,
//Int16           = 9,
//UInt64          = 10,
//Int64           = 11,
QVariant StackerWg::GetDatatypeValue(QString typestr ,QString value)
{
    if(typestr == "Boolean")
    {
        return  value.toInt();
    }
    else if(typestr == "Int32")
    {
        return  value.toInt();
    }
    else if(typestr == "UInt32")
    {
        return  value.toUInt();
    }
    else if(typestr == "Double")
    {
        return  value.toDouble();
    }
    else if(typestr == "Float")
    {
        return  value.toFloat();
    }
    else if(typestr == "String")
    {
        return  value;
    }
    else if(typestr == "LocalizedText")
    {
        return  value;
    }
    else if(typestr == "DateTime")
    {
        return  value;
    }
    else if(typestr == "UInt16")
    {
        return  value.toUShort();
    }
    else if(typestr == "Int16")
    {
        return  value.toShort();
    }
    else if(typestr == "UInt64")
    {
        return  value.toULongLong();
    }
    else if(typestr == "Int64")
    {
        return  value.toLongLong();
    }
    return typestr;
}

void StackerWg::SetVarValue(int16_t value, QString name , QOpcUa::Types type)
{
    OrderStru stru;
    stru.childtype = 1;
    stru.Datatype = static_cast<int>(type);
    QMap<QString,QVariant> valuemap;
    valuemap.insert(name,value);
    stru.WcmdstackerValues = valuemap;
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(stru,m_commID);
}

void StackerWg::StopStacker()
{
   //if(mrunthread->isRunning())
    {
        mstackercmd->isTerminate = true;
        //mrunthread->quit();
        //mrunthread->wait();
    }
}

