#include "dispatchwidget.h"

DispatchWidget::DispatchWidget(int width, int height, QWidget *parent) : QWidget(parent)
{
    this->m_height = height;
    this->m_width = width;
    m_time = 0;
    initUI();
    m_ptimer = new QTimer(this);
    connect(m_ptimer,&QTimer::timeout,this,&DispatchWidget::updateUi);
//    connect(m_plineBoxNum,&QLineEdit::textChanged,[this](){
//        if(m_StorenbrLineEdit->text() != "")
//            m_StorenbrLineEdit->setText("");
//    });
    m_ptimer->start(600);
}

DispatchWidget::~DispatchWidget()
{


}

//料箱子进入仓库的唯一入口，从这个地方记录当前等待入库的数量
//用户时常会在这里点击两下,导致小车重复执行两次同样的任务
void DispatchWidget::onBoxClicked()
{
    //    if(m_time != 0)
    //    {
    //        qDebug()<<"time end"<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    //        if(QDateTime::currentDateTime().toMSecsSinceEpoch() - m_time < 10000)
    //        {
    //            appendInfo(QString("<font color=\"#FF0000\">请等待上一个入库任务完成之后再点击,时机不对</font> "));
    //            m_plineBoxNum->setFocus();
    //            return;
    //        }
    //    }
    //    if(Myconfig::GetInstance()->m_in_taskMap.size() >= 1)
    //    {
    //        appendInfo(QString("<font color=\"#FF0000\">请等待上一个入库任务完成之后再点击</font> "));
    //        m_plineBoxNum->setFocus();
    //        return;
    //    }
    if(m_pInOutCombox->currentText() != "入库")
    {
        appendInfo(QString("<font color=\"#FF0000\">请将任务类型更改为入库</font> "));
        return;
    }
    //校验入库输入
    if(!CheckInIsOK())
        return ;

    QPushButton *b = static_cast<QPushButton*>(sender());
    if(b != nullptr)
    {
        m_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug()<<"time start"<<m_time;
        //m_pInOutCombox->setCurrentText("入库");

        int i = b->objectName().toInt();

        execRunnerInstruction(2,99);
        QThread::msleep(20);

        if(Myconfig::GetInstance()->m_cacheRunerMap[i] == true)
        {
            Myconfig::GetInstance()->m_runer.cache_in_current++;
            Myconfig::GetInstance()->m_cacheRunerMap[i] = false;
            qDebug()<<" cache"<<i<<" = false";
        }
        execRunnerInstruction(((i-1)*2 + 60),99);
        box_in();
        m_plineBoxNum->setText("");
    }
    if(!m_plineBoxNum->hasFocus())
        m_plineBoxNum->setFocus();
}

void DispatchWidget::onGetBoxClicked()
{
    if(m_pInOutCombox->currentText() != "出库")
    {
        appendInfo(QString("<font color=\"#FF0000\">请将任务类型更改为出库</font> "));
        return;
    }
    TaskInfoStru t;
    //    t.boxNum = m_plineBoxNum->text();
    //    if(t.boxNum == "")
    //    {
    //        appendInfo(QString("<font color=\"#FF0000\">料箱号不能为空</font> "));
    //        m_plineBoxNum->setFocus();
    //        return;
    //    }
    if(!CheckOutIsOK())
        return;
    //先使用箱子编号执行手动任务
    t.taskMainType = m_pInOutCombox->currentText();
    t.regCode = QString("HK_%1").arg(QDateTime::currentDateTime().toTime_t());
    t.creatTime = QDateTime::currentDateTime();//.toString("yyyy-MM-dd:hh:ss:mm");
    t.taskSonType = "下架";
    t.from = "Manual";
    t.shelves = "9";
    t.pripty= 1;
    t.fromLocationCode = m_StorenbrLineEdit->text();
    t.boxNum = m_plineBoxNum->text();
    if(m_pInOutCombox->currentText() == "出库")
    {
        //        if(Myconfig::GetInstance()->m_storeinfoMap.contains(m_plineBoxNum->text()))
        //        {
        Myconfig::GetInstance()->m_taskQueue.enqueue(t);
        Myconfig::GetInstance()->m_OuttaskMap[9].enqueue(t);
        //QString box_num = Myconfig::GetInstance()->m_storeinfoMap[m_plineBoxNum->text()].boxnbr;
        //        }
        //        else
        //        {
        //            appendInfo("We can't find this bin");
        //            return;
        //        }
    }
    appendInfo(m_pInOutCombox->currentText()+ "任务 料箱: ["+m_plineBoxNum->text()+"] 已插入队列,货架号为:["+t.fromLocationCode+"]");
    m_plineBoxNum->setText("");
    m_plineBoxNum->setFocus();

}

//void DispatchWidget::onScanCode()
//{
//    QPushButton *b = static_cast<QPushButton*>(sender());
//    QString data;
//    int device_id;
//    if(b->objectName() == "Scan_1")
//    {
//        data = "1B31";
//        device_id = 40;
//    }
//    else if(b->objectName() == "Scan_2")
//    {
//        data = "1B31";
//        device_id = 62;
//    }
//    else if(b->objectName() == "ClearTask")
//    {
//        Myconfig::GetInstance()->m_in_taskMap.clear();
//        Myconfig::GetInstance()->m_taskQueue.clear();
//        return;
//    }
//    else
//        return;
//    AbstructInstruction *c = new ScanCodeInstruction();
//    OrderStru o;
//    o.order = Order::Scan_Code;
//    o.strData = data;
//    o.order = Scan_Code;
//    c->setParameter(o,device_id);
//    QString execStr = "";
//    if(c->getResult(execStr) != 1)
//        appendInfo(execStr);
//    else
//        appendInfo("Code scanning message sent successfully");

//}
QString  styleStr ;  QString layerStyle; QString layerText;
void DispatchWidget::updateUi()
{
    for(int i = 0; i < 8; i++)
    {
        if(m_box_list[i] != nullptr)
        {
            if(Myconfig::GetInstance()->m_cacheRunerMap.contains(i+1))
                Myconfig::GetInstance()->m_cacheRunerMap[i+1] ? m_box_list[i]->setStyleSheet("background-image: url(:/resouse/Image/have_box.png);"): m_box_list[i]->setStyleSheet("background-image: url(:/resouse/Image/no_box.png);");
        }
    }
    // m_plineCacheIn->setText(QString("%1").arg(Myconfig::GetInstance()->m_runer.cache_in_current));
    // m_plineCacheOut->setText(QString("%1").arg(Myconfig::GetInstance()->m_taskMap.size()));
    //    if(Myconfig::GetInstance()->m_CarMap.contains(0))
    //    {
    //        Myconfig::GetInstance()->m_CarMap[0].deveceStatus.isLocking? styleStr="background-color:rgb(116,169,26)": styleStr = "background-color:rgb(201,64,73)";
    //        m_status_lab->setStyleSheet(styleStr);
    //    }
    // m_in_task_count->setText(Myconfig::GetInstance()->m_IntaskMap.contains(9)? QString("%1").arg(Myconfig::GetInstance()->m_IntaskMap[9].size()) : "0");
    // m_out_task_count->setText(Myconfig::GetInstance()->m_OuttaskMap.contains(9)?QString("%1").arg(Myconfig::GetInstance()->m_OuttaskMap[9].count()):"0");

    //    for(auto it = Myconfig::GetInstance()->m_layerStatusMap.begin();it != Myconfig::GetInstance()->m_layerStatusMap.end();it++)
    //    {
    //        if(it.value().isLocked)
    //        {
    //            layerStyle ="background-color:rgb(205, 85, 85)";
    //            layerText = QString("第%1层 \r\n 被 %2 锁定").arg(it.key()).arg(it.value().carId);
    //        }
    //        else
    //        {
    //            layerStyle = "background-color:rgb(124, 205, 124)";
    //            layerText = "未锁定";
    //        }
    //        if(m_layer_list[it.key()-1] != nullptr)
    //        {
    //            m_layer_list[it.key()-1]->setStyleSheet(layerStyle);
    //            m_layer_list[it.key()-1]->setText(layerText);
    //        }
    //    }
    UpdateLayerUI();
    auto updateRundata = [this](){
        m_layerPut_1->setText(QString("%1").arg(getHoldresRegestValue(10)));
        m_scan_1->setText(QString("%1").arg(getHoldresRegestValue(6)));
        m_scan_2->setText(QString("%1").arg(getHoldresRegestValue(8)));
        m_layerPut_2->setText(QString("%1").arg(getHoldresRegestValue(12)));
    };
    updateRundata();
}

//void DispatchWidget::onPost()
//{
//    HttpClien_k *h = new HttpClien_k();
//    QString result = "";
//    OrderStru o;
//    o.http.url = m_phttpUrl->text();
//    o.http.body = "body";
//    h->setParameter(o,0);
//    h->runInstruction();
//    int v = h->getResult(result);
//    if(v < 0)
//    {
//        appendInfo("request failed!");
//    }
//    else
//        appendInfo(result);
//}

void DispatchWidget::onCurrentTextChange(QString current_index)
{
    if(current_index == "出库")
        m_plineBoxNum->setText("");
    else
        m_plineBoxNum->setText("");
}

void DispatchWidget::initUI()
{
    initRightW();
    //initLayerUI();
    InitLayerUI();
    m_pLayerW->move(10,10);
    m_prightW->move(m_width - 250,10);
    m_poutWidget = new QWidget(this);
    m_poutWidget->setStyleSheet("QLabel{font-family:Times; font:15px;background:transparent}QWidget{background-color:rgb(190,190,190);border-radius: 6px;}QPushButton{font: 16px;width:140px;height:35;background-color:rgb(0,170,255);}QPushButton:hover{background: rgb(220, 220, 220);}QPushButton:pressed{background-color:rgb(85, 170, 255);}");
    m_poutWidget->resize(m_width - m_prightW->width()-310,m_height-10);
    m_poutWidget->move(190,10);

    //    QLabel *b = new QLabel();
    //    b->setText("In_Cache:");
    //    m_plineCacheIn = new QLineEdit();

    //    m_plineCacheIn->setMaximumWidth(60);
    //    QLabel *a = new QLabel();
    //    a->setText("Out_Cache:");

    //    m_plineCacheOut = new QLineEdit();
    //    m_plineCacheOut->setMaximumWidth(60);
    m_vbox = new QVBoxLayout();

    //  m_status_lab  = new QLabel();
    //  m_status_lab->move(20,10);
    //  m_status_lab->resize(20,20);
    // QLabel *in_task_lab = new QLabel("入库任务:");
    //  in_task_lab->move(40,10);
    // m_in_task_count = new QLabel();
    //  m_in_task_count->move(70,10);
    //  QLabel *out_task_lab = new QLabel("出库任务:");
    // out_task_lab->move(100,10);
    // m_out_task_count = new QLabel();
    //  m_out_task_count->move(140,10);
    // m_hbox = new QHBoxLayout();
    //  m_hbox->addWidget(m_status_lab);
    // m_hbox->addWidget(in_task_lab);
    // m_hbox->addWidget(m_in_task_count);
    // m_hbox->addStretch();
    // m_hbox->addWidget(out_task_lab);
    //  m_hbox->addWidget(m_out_task_count);
    //  m_hbox->addStretch();
    // m_vbox->addLayout(m_hbox);
    // m_vbox->addStretch(3);


    // m_hbox = new QHBoxLayout();
    // m_hbox->addWidget(b);
    //  m_hbox->addWidget(m_plineCacheIn);
    // m_hbox->addStretch();

    // m_hbox->addWidget(a);
    // m_hbox->addWidget(m_plineCacheOut);
    // m_hbox->addStretch();
    // m_vbox->addLayout(m_hbox);
    // m_vbox->addStretch(2);
    m_hbox = new QHBoxLayout();
    QLabel *l_put = new QLabel("一层可放:");
    m_layerPut_1 = new QLabel();
    QLabel *l_scan_1 = new QLabel("一层扫码:");
    m_scan_1 = new QLabel();

    QLabel *l_scan_2 = new QLabel("二层扫码:");
    m_scan_2 = new QLabel();

    QLabel *l_put_2 = new QLabel("二层可取:");
    m_layerPut_2 = new QLabel();
    m_hbox->addWidget(l_put);
    m_hbox->addWidget(m_layerPut_1);
    m_hbox->addStretch(1);
    m_hbox->addWidget(l_scan_1);
    m_hbox->addWidget(m_scan_1);
    m_hbox->addStretch(1);
    m_hbox->addWidget(l_scan_2);
    m_hbox->addWidget(m_scan_2);
    m_hbox->addStretch(1);
    m_hbox->addWidget(l_put_2);
    m_hbox->addWidget(m_layerPut_2);
    m_hbox->addStretch(5);
    m_vbox->addStretch(1);
    m_vbox->addLayout(m_hbox);
    m_vbox->addStretch(1);

    QLabel *e = new QLabel();
    e->setText("料箱号：");
    e->setStyleSheet("font-family:Times; font:22px;background:transparent");
    e->move(20,100);
    m_poutButton = new QPushButton();
    m_poutButton->setText("出库取箱");
    connect(m_poutButton,&QPushButton::clicked,this,&DispatchWidget::onGetBoxClicked);
    m_plineBoxNum = new QLineEdit();
    //m_plineBoxNum->setText("BOX0001");
    //("KS-10-A1-01-01");
    m_plineBoxNum->setMinimumWidth(200);
    m_plineBoxNum->setMinimumHeight(50);
    m_plineBoxNum->setStyleSheet("font-family:Times; font:22px;background:transparent");
    m_pInOutCombox = new QComboBox();
    m_pInOutCombox->setMinimumWidth(80);
    m_pInOutCombox->setMinimumHeight(50);
    m_pInOutCombox->setStyleSheet("font-family:Times; font:22px;background:transparent");
    QStringList str_l;
    str_l<<"入库"<<"出库";
    m_pInOutCombox->addItems(str_l);
    connect(m_pInOutCombox,&QComboBox::currentTextChanged,this,&DispatchWidget::onCurrentTextChange);

    m_StorenbrLineEdit = new QLineEdit(m_poutWidget);
    m_StorenbrLineEdit->setMinimumWidth(200);
    m_StorenbrLineEdit->setMinimumHeight(50);
    m_StorenbrLineEdit->setStyleSheet("font-family:Times; font:22px;background:transparent");
    m_StorenbrLineEdit->setVisible(false);
    m_hbox = new QHBoxLayout();
    m_hbox->addWidget(new QLabel("任务类型："));
    m_hbox->addWidget(m_pInOutCombox);
    m_hbox->addWidget(e);
    m_hbox->addWidget(m_plineBoxNum);
   // m_hbox->addWidget(new QLabel("货架号："));
    m_hbox->addWidget(m_StorenbrLineEdit);

    m_hbox->addStretch(3);
    m_hbox->addWidget(m_poutButton);
    m_hbox->addStretch(7);
    m_vbox->addLayout(m_hbox);

    //    m_pScan_1 = new QPushButton();
    //    m_pScan_1->setText("Scan_1");
    //    m_pScan_1->setObjectName("Scan_1");
    //    m_pScan_2 = new QPushButton();
    //    m_pScan_2->setText("Scan_2");
    //    m_pScan_2->setObjectName("Scan_2");
    //    m_clearBtn = new QPushButton();
    //    m_clearBtn->setText("ClearTask");
    //    m_clearBtn->setObjectName("ClearTask");

    // m_hbox = new QHBoxLayout();
    // m_hbox->addWidget(m_pScan_1);
    //m_hbox->addWidget(m_pScan_2);
    // m_hbox->addWidget(m_clearBtn);
    // m_hbox->addStretch();
    //  m_vbox->addLayout(m_hbox);
    // m_vbox->addStretch(2);
    // connect(m_pScan_1,&QPushButton::clicked,this,&DispatchWidget::onScanCode);
    //connect(m_pScan_2,&QPushButton::clicked,this,&DispatchWidget::onScanCode);
    // connect(m_clearBtn,&QPushButton::clicked,this,&DispatchWidget::onScanCode);

    // m_phttpUrl = new QLineEdit();
    //  m_http_btn = new QPushButton("Post");
    //  m_hbox = new QHBoxLayout();
    //   m_hbox->addWidget(m_phttpUrl);
    // m_hbox->addWidget(m_http_btn);
    // m_hbox->addStretch();
    //  m_vbox->addLayout(m_hbox);
    m_vbox->addStretch(1);
    // connect(m_http_btn,&QPushButton::clicked,this,&DispatchWidget::onPost);

    m_ptextLine_log = new QTextEdit(this);
    m_vbox->addWidget(m_ptextLine_log);
    m_ptextLine_log->setStyleSheet("font-family:Times; font:17px;background:transparent;border-width: 1px;border-color:rgb(150,150,150); border-style: solid;");
    m_vbox->addStretch(19);
    m_poutWidget->setLayout(m_vbox);
}

void DispatchWidget::initLayerUI()
{
    m_pLayerW = new QWidget(this);
    m_pLayerW->resize(160,m_height-10);
    QVBoxLayout *vbox = new QVBoxLayout(m_pLayerW);
    for(int i = 0; i < 8; i++)
    {
        m_layer_list[i] = new QLabel(m_pLayerW);
        vbox->addWidget(m_layer_list[i]);
        m_layer_list[i]->setText("未锁定");
        vbox->addSpacing(20);
        m_layer_list[i]->setAlignment(Qt::AlignCenter);
        m_layer_list[i]->setStyleSheet("background-color:rgb(124, 205, 124)");
    }
    m_pLayerW->setStyleSheet("QPushButton{background-image: url(:/resouse/Image/no_box.png);}"
                             "QPushButton:hover{background-image: url(:/2.bmp);}"
                             "QPushButton:pressed{background-image: url(:/resouse/Image/have_box.png);}"
                             "QPushButton{width:70px;height:70;}"
                             "QWidget{background-color:rgb(190,190,190);border-radius: 5px;}");
    m_pLayerW->setLayout(vbox);
}

int DispatchWidget::getHoldresRegestValue(int address)
{
    if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap.contains(address))
    {
        return Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[address];
    }
    return -1;
}

void DispatchWidget::initRightW()
{
    m_prightW = new QWidget(this);
    m_prightW->resize(160,m_height-10);
    m_pvbox_rightw = new QVBoxLayout(m_prightW);
    QFont font("宋体",12);
    for(int i = 0; i < 8; i++)
    {
        m_box_list[i] = new QPushButton();
        m_box_list[i]->setObjectName(QString("%1").arg(i+1));
        m_box_list[i]->setText(QString("A%1").arg(i+1));
        m_box_list[i]->setFont(font);
        connect(m_box_list[i],&QPushButton::clicked,this,&DispatchWidget::onBoxClicked);
        m_pvbox_rightw->addWidget(m_box_list[i]);
    }
    m_prightW->setLayout(m_pvbox_rightw);
    m_prightW->setStyleSheet("QPushButton{background-image: url(:/resouse/Image/no_box.png);}"
                             "QPushButton:hover{background-image: url(:/2.bmp);}"
                             "QPushButton:pressed{background-image: url(:/resouse/Image/have_box.png);}"
                             "QPushButton{width:70px;height:70;}"
                             "QWidget{background-color:rgb(190,190,190);border-radius: 5px;}");

}

void DispatchWidget::box_in()
{
    TaskInfoStru t;
    t.boxNum = m_plineBoxNum->text();
    t.shelves = m_StorenbrLineEdit->text();
    //    if(t.boxNum == "")
    //    {
    //        m_plineBoxNum->setFocus();
    //        return;
    //    }
    t.taskMainType = m_pInOutCombox->currentText();
    t.regCode = QString("HK_%1").arg(QDateTime::currentDateTime().toTime_t());
    t.taskSonType = "回库";
    t.creatTime = QDateTime::currentDateTime();//.toString("yyyy-MM-dd:hh:ss:mm");
    t.from = "Manual";
    t.pripty= 1;
    t.fromLocationCode = "9";
    if(m_pInOutCombox->currentText() == "入库")
    {
        Myconfig::GetInstance()->m_in_taskMap.insert(m_plineBoxNum->text(),t);
        Myconfig::GetInstance()->m_boxNum_in =  m_plineBoxNum->text();//for test

        Myconfig::GetInstance()->m_IntaskMap[9].insert(m_plineBoxNum->text(),t);
    }
    appendInfo(m_pInOutCombox->currentText()+ "任务 料箱: ["+m_plineBoxNum->text()+"] 已插入队列,货架号为:["+t.shelves+"]");
    m_plineBoxNum->setFocus();
}

void DispatchWidget::execRunnerInstruction(int startAddress, int value)
{
    OrderStru o;
    o.value = value;   //value
    o.order = call_Runner_Putbox;
    o.startaddress = startAddress; //(i-1)*2 + 60;  //start address
    o.Datatype = 168;//A8 D型寄存器的的代号
    o.childtype = 2;
    AbstructInstruction *e = new RunnerInstruction();
    e->setParameter(o,9);
    e->runInstruction();
    QString resultMsg ="";
    e->getResult(resultMsg);
    qDebug()<<resultMsg;
}

bool DispatchWidget::checkBoxNum(const QString &str,QString &nbr)
{
    KPosition task_p;//根据料箱号返回料箱所在坐标
    QString result = StoreInfo::BaseDataInfoOperate::GetWarehouselocationInfoForOut(str,task_p);
    if(result == "")
    {
        appendInfo(QString("<font color=\"#FF0000\">料箱号输入有误</font> "));
        m_plineBoxNum->setFocus();
        return false;
    }
    nbr = result;
    return true;
}

void DispatchWidget::appendInfo(QString str)
{
    m_ptextLine_log->append(QString(">%1  :"+str).arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void DispatchWidget::InitLayerUI()
{
    m_pLayerW = new QWidget(this);
    m_pLayerW->resize(160,m_height-10);
    QVBoxLayout *vbox = new QVBoxLayout(m_pLayerW);
    for(int i = 1;i<= 8;++i)
    {
        QQuickWidget *pWidget = new QQuickWidget();
        pWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        pWidget->setSource(QUrl("qrc:/qml/uiqml/LayerState.qml"));
        vbox->addWidget(pWidget);//Qt::WA_AlwaysStackOnTop
        pWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
        pWidget->setClearColor(QColor(Qt::transparent));
        QObject* pRoot = static_cast<QObject*>(pWidget->rootObject());
        m_qmlitem.insert(i,pRoot);
    }
    m_pLayerW->setStyleSheet("QPushButton{background-image: url(:/resouse/Image/no_box.png);}"
                             "QPushButton:hover{background-image: url(:/2.bmp);}"
                             "QPushButton:pressed{background-image: url(:/resouse/Image/have_box.png);}"
                             "QPushButton{width:70px;height:70;}"
                             "QWidget{background-color:rgb(190,190,190);border-radius: 5px;}");
    m_pLayerW->setLayout(vbox);
    //更新所有层数状态显示的参数
    for(auto it  = m_qmlitem.begin();it != m_qmlitem.end();++it)
    {
        UpDateLayerState(it.key(),"",1);
    }
}

void DispatchWidget::UpDateLayerState(int layer, QString carid, int state)
{
    if(m_qmlitem.contains(layer))
    {
        if(m_qmlitem[layer])
        {
            QMetaObject::invokeMethod(m_qmlitem[layer], "carReadyIn",
                                      Q_ARG(QVariant, carid),
                                      Q_ARG(QVariant, state),
                                      Q_ARG(QVariant, layer));
        }
    }
}

void DispatchWidget::UpdateLayerUI()
{
    for(auto it = Myconfig::GetInstance()->m_layerStatusMap.begin();\
        it != Myconfig::GetInstance()->m_layerStatusMap.end();it++)
    {
        QString id = "";
        if(it.value().layerstate == LayerState::InLayer || it.value().layerstate == LayerState::ReadyInLayer)
        {
            id = QString::number(it.value().carId);
        }
        UpDateLayerState(it.key(),id,it.value().layerstate);
    }
}
///
/// \brief DispatchWidget::CheckOutIsOK
/// \return
///
bool DispatchWidget::CheckOutIsOK()
{
    if(m_plineBoxNum->text() == "" && m_StorenbrLineEdit->text() == "")
    {
        appendInfo("料箱号与货架号不能同时为空");
        m_plineBoxNum->setFocus();
        return false;
    }
    if(m_StorenbrLineEdit->text() != "")
    {
        QString nbr = "";
        if((!StoreInfo::BaseDataInfoOperate::GetBoxinfo(m_StorenbrLineEdit->text(),nbr))|| nbr =="")
        {
            appendInfo("货架号不存在");
            return false;
        }
        m_plineBoxNum->setText(nbr);
    }
    else{
        if(m_plineBoxNum->text() != "") //由料箱子编号查询货架号
        {
            QString nbr = "";
            if(!checkBoxNum(m_plineBoxNum->text(),nbr))
            {
                appendInfo("料箱号不存在");
                return false ;
            }
            m_StorenbrLineEdit->setText(nbr);
        }

    }
//    if(m_plineBoxNum->text() != "") //由料箱子编号查询货架号
//    {
//        QString nbr = "";
//        if(!checkBoxNum(m_plineBoxNum->text(),nbr))
//        {
//            appendInfo("料箱号不存在");
//            return false ;
//        }
//        m_StorenbrLineEdit->setText(nbr);
//    }
//    else  {//货架号查询料箱编号
//        QString nbr = "";
//        if((!StoreInfo::BaseDataInfoOperate::GetBoxinfo(m_StorenbrLineEdit->text(),nbr))|| nbr =="")
//        {
//            appendInfo("货架号不存在");
//            return false;
//        }
//        m_plineBoxNum->setText(nbr);
//    }
    return  true;
}

bool DispatchWidget::CheckInIsOK()
{
    if(m_plineBoxNum->text() == "" && m_StorenbrLineEdit->text() == "")
    {
        appendInfo("料箱号与货架号不能同时为空");
        m_plineBoxNum->setFocus();
        return false;
    }

    if(m_StorenbrLineEdit->text() != "")
    {
        QString nbr = "";
        if(!StoreInfo::BaseDataInfoOperate::GetBoxinfo(m_StorenbrLineEdit->text(),nbr))
        {
            appendInfo("货架号不存在");
            return false;
        }
        if(nbr != "")
        {
            m_plineBoxNum->setText(nbr);
        }
    }
    else{
        QString nbr = "";
        if(!checkBoxNum(m_plineBoxNum->text(),nbr))
        {
            appendInfo("料箱号不存在");
            return false ;
        }
        m_StorenbrLineEdit->setText(nbr);
    }

//    if(m_plineBoxNum->text() != "") //由料箱子编号查询货架号
//    {
//        QString nbr = "";
//        if(!checkBoxNum(m_plineBoxNum->text(),nbr))
//        {
//            appendInfo("料箱号不存在");
//            return false ;
//        }
//        m_StorenbrLineEdit->setText(nbr);
//    }
//    else  {//货架号查询料箱编号
//        QString nbr = "";
//        if(!StoreInfo::BaseDataInfoOperate::GetBoxinfo(m_StorenbrLineEdit->text(),nbr))
//        {
//            appendInfo("货架号不存在");
//            return false;
//        }
//        m_plineBoxNum->setText(nbr);
//    }
    return  true;
}
