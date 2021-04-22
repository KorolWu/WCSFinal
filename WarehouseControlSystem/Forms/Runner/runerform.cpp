#include "runerform.h"
#include "Myconfig.h"
RunerForm::RunerForm(int width, int height, QWidget *parent) : QWidget(parent)
{
    initUi();
    m_width = width;
    m_height = height;
    this->resize(width/2, height);
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&RunerForm::updateUI);
    m_timer->start(300);
}

void RunerForm::setParameter(RunerStru s)
{
    m_pIp->setText(s.deviceIp);
    //m_pPort->setText(QString("%1").arg(s.port));
    //m_prunerName->setText(s.deviceNum);
    m_pinCache->setText(QString("%1").arg(s.cache_in_max));
    m_poutCache->setText(QString("%1").arg(s.cache_out_max));
    m_porderIn->setText(s.request_in);
    m_preplyin->setText(s.response_in);
    m_porderOut->setText(s.request_out);
    m_preplyOut->setText(s.response_out);
}

void RunerForm::yesBtnClicked()
{
    testUrl();
    QString in = m_pinCache->text();
    QString out = m_poutCache->text();
    QString camera = m_porderIn->text();
    int deviceNum = m_prunerName->value();
    QString ip = QString("%1:%2").arg(m_pIp->text()).arg(m_pPort->value());
    int address = m_pPort->value();
//  Myconfig::GetInstance()->m_runer.cache_in_current = in.toInt();
//  Myconfig::GetInstance()->m_runer.cache_out_current = out.toInt();
    Myconfig::GetInstance()->m_boxNum_in = camera;
    qDebug()<<"in "<<in.toInt()<<"; out "<<out.toInt()<<"; box_num "<<camera<<deviceNum<<"-"<<ip<<"-"<<address;

    QString hw_sql = QString("INSERT t_hwcomm_info SET ID = '%1',type = 5,protype = 4, port = '%2',name = '%3';")
       .arg(deviceNum).arg(m_pPort->value()).arg(ip);
    bool result = CRUDBaseOperation::getInstance()->queryUseStr(hw_sql);
    qDebug()<<result;

}

void RunerForm::updateUI()
{
    Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[12] == 99? m_pickupImage->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pickupImage->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[10] == 99? m_putupImage->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_putupImage->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[6]  == 99? m_pscanImage1->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pscanImage1->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[8]  == 99? m_pscanImage2->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pscanImage2->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[18] == 99? m_pbackupImage->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pbackupImage->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_runer.runneratastru.connectresult == 1?   m_pisOnlineImage->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pisOnlineImage->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    UpRunnerstatebit();
}

void RunerForm::onClicked()
{
    QPushButton *btn =static_cast<QPushButton*>(sender());
    if(btn->objectName() == "send")
    {
        OrderStru o;
        o.value = m_pvalue->value();   //value
        o.order = call_Runner_Putbox;
        o.startaddress = m_paddress->value();  //start address
        o.Datatype = 168;//A8 D型寄存器的的代号
        o.childtype = 2;
        AbstructInstruction *e = new RunnerInstruction();
        e->setParameter(o,m_prunerName->value());
        e->runInstruction();
        QString resultMsg ="";
        e->getResult(resultMsg);
        qDebug()<<resultMsg;
    }
    else if(btn->objectName() == "start")
    {

        OrderStru o;
        if(btn->text() !="停止流道")
        {
            o.value = 99;   //value
            btn->setText("停止流道");
        }
        else
        {
            o.value = 0;
            btn->setText("启动流道");
        }
        o.order = call_Runner_Putbox;
        o.startaddress = 22;  //start address
        o.Datatype = 168;//A8 D型寄存器的的代号
        o.childtype = 2;
        AbstructInstruction *e = new RunnerInstruction();
        e->setParameter(o,m_prunerName->value());
        e->runInstruction();
        QString resultMsg ="";
        e->getResult(resultMsg);
        qDebug()<<resultMsg;
    }
}

void RunerForm::slotCmdDispatch()
{
    //test 指令发送的部分2021-01-23
    int id  = m_RunerIDComboBox->currentText().toInt();
    QString curcmd = m_RunercmdComboBox->currentText();
    OrderStru runnerstru;
    runnerstru.childtype = 6;
    if(curcmd == "启动流道")
    {
        runnerstru.order = Runner_Start;
    }
    else if(curcmd == "关闭流道")
    {
        runnerstru.order = Runner_Stop;
    }
    else if(curcmd == "入库扫码完成")
    {
        runnerstru.order = Runner_Scancode2_In;
    }
    else if(curcmd == "出库扫码完成")
    {
        runnerstru.order = Runner_Scancode1_Out;
    }
    else if(curcmd == "取货完成")
    {
        runnerstru.order = Runner_CarPickFinished;
    }
    else if(curcmd == "放货完成")
    {
        runnerstru.order = Runner_CarputFinished;
    }
    else if(curcmd == "盘点放货完成")
    {
        runnerstru.order = Runner_CarputcheckFinished;
    }
    else if(curcmd == "入库出箱")
    {
        runnerstru.order = Runner_InPushBox;
    }
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,id);
}

void RunerForm::noBtnClicked()
{
    if(Myconfig::GetInstance()->m_IntaskMap[9].size() != 0)
    {
        auto it = Myconfig::GetInstance()->m_IntaskMap[9].begin();
         TaskInfoStru t =it.value();
        KPosition p;
        QString ip = "";
        KDispatch k(p,ip,1,t);
        QByteArray body = k.makeBody(t,900,t.shelves,true,"");


        HttpClien_k *h = new HttpClien_k();
        OrderStru postParameter;
        postParameter.http.body = body;
        if(t.taskMainType == "出库")
            postParameter.http.url = Myconfig::GetInstance()->outOfWarehouseUrl;
        else
            postParameter.http.url = Myconfig::GetInstance()->inOfWareHouseUrl;
        h->setParameter(postParameter,0);
        h->runInstruction();
        QString errmsg;
        h->getResult(errmsg);

    }
    else {
        qDebug()<<"No In Task";
    }
}

void RunerForm::initUi()
{
    QFont serifFont("Times", 16);
    m_pvBox = new QVBoxLayout(this);
    QLabel *info = new QLabel("流道参数管理",this);
    m_pvBox->addWidget(info);
    QGroupBox *check_status_groupbox = new QGroupBox("状态监测",this);
    check_status_groupbox->resize(600,100);
    QHBoxLayout *hboxSokcet = new QHBoxLayout();
    QLabel *e = new QLabel("可取");
    m_pickupImage = new QLabel();
    m_pickupImage->setFixedSize(10,10);
    hboxSokcet->addWidget(e);
    hboxSokcet->addWidget(m_pickupImage);
    hboxSokcet->addStretch();
    QLabel *l = new QLabel("可放");
    m_putupImage = new QLabel();
     m_putupImage->setFixedSize(10,10);
    hboxSokcet->addWidget(l);
    hboxSokcet->addWidget(m_putupImage);
    hboxSokcet->addStretch();
    QLabel *b = new QLabel("扫码_1");
    m_pscanImage1 = new QLabel();
     m_pscanImage1->setFixedSize(10,10);
    hboxSokcet->addWidget(b);
    hboxSokcet->addWidget(m_pscanImage1);
    hboxSokcet->addStretch();
    QLabel *a = new QLabel("扫码_2");
    m_pscanImage2 = new QLabel();
     m_pscanImage2->setFixedSize(10,10);
    hboxSokcet->addWidget(a);
    hboxSokcet->addWidget(m_pscanImage2);
    hboxSokcet->addStretch();
    QLabel *q = new QLabel("超重");
    m_pbackupImage = new QLabel();
     m_pbackupImage->setFixedSize(10,10);
    hboxSokcet->addWidget(q);
    hboxSokcet->addWidget(m_pbackupImage);
    hboxSokcet->addStretch();

    QLabel *f = new QLabel("在线");
    m_pisOnlineImage = new QLabel();
     m_pisOnlineImage->setFixedSize(10,10);
    hboxSokcet->addWidget(f);
    hboxSokcet->addWidget(m_pisOnlineImage);
     hboxSokcet->addStretch();
    check_status_groupbox->setLayout(hboxSokcet);
    //hboxSokcet->setContentsMargins(5, 0, 5, 0);
    m_pvBox->addWidget(check_status_groupbox);

    QGroupBox *check_send = new QGroupBox("通讯测试",this);
    check_send->resize(600,100);
    hboxSokcet = new QHBoxLayout();
    QLabel *address1 = new QLabel("地址");
    m_paddress = new QSpinBox();

    m_psendBtn = new QPushButton();
    m_psendBtn->setText("发送");
    m_psendBtn->setObjectName("send");
    connect(m_psendBtn,&QPushButton::clicked,this,&RunerForm::onClicked);
    m_pStartBtn = new QPushButton();
    connect(m_pStartBtn,&QPushButton::clicked,this,&RunerForm::onClicked);
    m_pStartBtn->setText("启动流道");
    m_pStartBtn->setObjectName("start");
    hboxSokcet->addWidget(address1);
    hboxSokcet->addWidget(m_paddress);
    hboxSokcet->addStretch();
    QLabel *value = new QLabel("值");
    m_pvalue = new QSpinBox();
    hboxSokcet->addStretch();
    hboxSokcet->addWidget(value);
    hboxSokcet->addWidget(m_pvalue);
    hboxSokcet->addStretch();
    hboxSokcet->addWidget(m_psendBtn);
    hboxSokcet->addWidget(m_pStartBtn);
    hboxSokcet->addStretch();
    check_send->setLayout(hboxSokcet);
    m_pvBox->addWidget(check_send);

    m_pgboxSocketInfo = new QGroupBox("通讯信息", this);
    m_pgboxSocketInfo->resize(600,100);
    hboxSokcet = new QHBoxLayout();
    QLabel *name = new QLabel("编号 :");
    hboxSokcet->addWidget(name);
    m_prunerName = new QSpinBox();
    qDebug()<<"deviceNum:"<<Myconfig::GetInstance()->m_runer.deviceNum;
    m_prunerName->setMaximum(10000);
    m_prunerName->setValue(Myconfig::GetInstance()->m_runer.deviceNum.toInt());
    hboxSokcet->addWidget(m_prunerName);
    QLabel *ip = new QLabel("Ip:port");
    hboxSokcet->addWidget(ip);
    m_pIp = new QLineEdit();
    m_pIp->setText(Myconfig::GetInstance()->m_runer.deviceIp);
    hboxSokcet->addWidget(m_pIp);
    QLabel *address = new QLabel("Address:");
    hboxSokcet->addWidget(address);
    m_pPort = new QSpinBox();
    m_pPort->setValue(Myconfig::GetInstance()->m_runer.port);
    hboxSokcet->addWidget(m_pPort);
    m_pgboxSocketInfo->setFont(serifFont);
    m_pgboxSocketInfo->setLayout(hboxSokcet);
    hboxSokcet->setSpacing(30);
    m_pvBox->addWidget(m_pgboxSocketInfo);
    //2021-01-23  测试流道动作指令
    QStringList runneridlist;
    for(auto it = Myconfig::GetInstance()->m_RunnerMap.begin();it !=Myconfig::GetInstance()->m_RunnerMap.end();++it)
    {
      runneridlist.append(QString::number(it.key()));
    }
    m_RunerIDComboBox = new QComboBox(this);
    m_RunerIDComboBox->addItems(runneridlist);
     hboxSokcet->addWidget(m_RunerIDComboBox);
    QStringList runnercmdlist;
    runnercmdlist<<"启动流道"<<"关闭流道"<<"入库扫码完成"<<"出库扫码完成"<<"取货完成"<<"放货完成"<<"盘点放货完成"<<"入库出箱";
    m_RunercmdComboBox = new QComboBox(this);
    m_RunercmdComboBox->addItems(runnercmdlist);
    hboxSokcet->addWidget(m_RunercmdComboBox);
    m_startcmdBtn = new QPushButton("指令发送");
    connect(m_startcmdBtn,&QPushButton::clicked,this,&RunerForm::slotCmdDispatch);
    hboxSokcet->addWidget(m_startcmdBtn);
    //增加状态位图信息显示
    //第一个字节显示的状态信息
    auto gridbox = new QGridLayout;
    QStringList list;
    list<<"可取"<<"可放"<<"可入库请求扫码"<<"可出库请求扫码";
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
    m_pvBox->addLayout(gridbox);
    //--------------------------------
    m_prunerInfo = new QGroupBox("缓存区信息", this);
    m_prunerInfo->setFont(serifFont);
    m_prunerInfo->resize(600,100);
    QHBoxLayout *hboxRuner = new QHBoxLayout();
    QLabel *incache = new QLabel("入库缓存数量:");
    m_pinCache = new QLineEdit();
    hboxRuner->addWidget(incache);
    hboxRuner->addWidget(m_pinCache);
    QLabel *outcache = new QLabel("出库缓存数量:");
    m_poutCache = new QLineEdit();
    hboxRuner->addWidget(outcache);
    hboxRuner->addWidget(m_poutCache);
    hboxRuner->setSpacing(30);
    m_prunerInfo->setLayout(hboxRuner);
    m_pvBox->addWidget(m_prunerInfo);
    //--------------------------------
    m_preplyInfo = new QGroupBox("响应信息", this);
    m_preplyInfo->setFont(serifFont);
    QLabel *request = new QLabel("request In :");
    m_porderIn = new QLineEdit();

    QHBoxLayout *hboxOrder = new QHBoxLayout();
    hboxOrder->addWidget(request);
    hboxOrder->addWidget(m_porderIn);

    QLabel *reply = new QLabel("reply In :");
    m_preplyin = new QLineEdit();
    hboxOrder->addWidget(reply);
    hboxOrder->addWidget(m_preplyin);
    hboxOrder->setSpacing(30);
    QVBoxLayout *v = new QVBoxLayout();
    v->addLayout(hboxOrder);

    hboxOrder = new QHBoxLayout();
    QLabel *replyOut = new QLabel("request Out :");
    m_porderOut = new QLineEdit();
    hboxOrder->addWidget(replyOut);
    hboxOrder->addWidget(m_porderOut);
    QLabel *replyIn = new QLabel("reply Out :");
    m_preplyOut = new QLineEdit();
    hboxOrder->addWidget(replyIn);
    hboxOrder->addWidget(m_preplyOut);
    //hboxOrder->addStretch();
     hboxOrder->setSpacing(30);
    v->addLayout(hboxOrder);
    m_preplyInfo->setLayout(v);
    m_pvBox->addWidget(m_preplyInfo);

    m_pYesBtn = new QPushButton("确定", this);
    connect(m_pYesBtn,&QPushButton::clicked,this,&RunerForm::yesBtnClicked);
    m_pNoBtn = new QPushButton("取消",this);
    connect(m_pNoBtn,&QPushButton::clicked,this,&RunerForm::noBtnClicked);
    QHBoxLayout *h = new QHBoxLayout();
    h->addWidget(m_pYesBtn);
    h->addWidget(m_pNoBtn);
    h->setSpacing(120);
    h->addStretch();
    m_pvBox->addLayout(h);
    m_pvBox->setSpacing(60);
    m_pvBox->addStretch();
    //QLabel{font: 16px;min-width:8px;min-height:8px;}
    this->setStyleSheet("QLineEdit{font: 14px;width:70px;height:25;}QPushButton{font: 14px;width:70px;height:25;background-color:rgb(150,150,150);}QPushButton:hover{background: rgb(80, 80, 80);}QPushButton:pressed{background-color:rgb(85, 170, 255);}QGroupBox{border: 1px solid white;border-radius:8px;margin-top:6px;}QGroupBox:title{color:rgb(24,24,58);subcontrol-origin: margin;left: 10px;}");
    this->setLayout(m_pvBox);

}

void RunerForm::UpRunnerstatebit()
{
    for(auto  it = m_RnamesMap.begin(); it != m_RnamesMap.end();++it)
    {
        int id  = m_RunerIDComboBox->currentText().toInt();
        if(Myconfig::GetInstance()->m_RunnerMap.contains(id))
        {
            int value  =  -1;
            if(it.key() == "可取")
            {
                value = Myconfig::GetInstance()->m_RunnerMap[id].bitstate[0].bbit0;
            }
            else if(it.key() == "可放")
            {
                value = Myconfig::GetInstance()->m_RunnerMap[id].bitstate[0].bbit1;
            }
            else if(it.key() == "可入库请求扫码")
            {
                value = Myconfig::GetInstance()->m_RunnerMap[id].bitstate[0].bbit2;
            }
            else if(it.key() == "可出库请求扫码")
            {
                value = Myconfig::GetInstance()->m_RunnerMap[id].bitstate[0].bbit2;
            }
            if(value == -1)
            {
                continue ;
            }
            if( value == 0)
            {
                QPixmap iconconnect(":/resouse/Image/grey.png");
                iconconnect = iconconnect.scaled(15,15);
                m_RnamesMap[it.key()]->setPixmap(iconconnect);
            }
            else{
                QPixmap iconconnect(":/resouse/Image/green.png");
                iconconnect = iconconnect.scaled(15,15);
                m_RnamesMap[it.key()]->setPixmap(iconconnect);
            }
        }
    }
}

void RunerForm::testUrl()
{
    if(!Myconfig::GetInstance()->m_OuttaskMap[9].isEmpty())
    {
        TaskInfoStru t = Myconfig::GetInstance()->m_OuttaskMap[9].dequeue();//Myconfig::GetInstance()->m_IntaskMap[9].begin();
        KPosition p;
        QString ip = "";
        KDispatch k(p,ip,1,t);
        QByteArray body = k.makeBody(t,500,"9",true,"");


        HttpClien_k *h = new HttpClien_k();
        OrderStru postParameter;
        postParameter.http.body = body;
        if(t.taskMainType == "出库")
            postParameter.http.url = Myconfig::GetInstance()->outOfWarehouseUrl;
        else
            postParameter.http.url = Myconfig::GetInstance()->inOfWareHouseUrl;
        h->setParameter(postParameter,0);
        h->runInstruction();
        QString errmsg;
        h->getResult(errmsg);

    }
    else {
        qDebug()<<"No Out Task";
    }
}
