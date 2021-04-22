#include "stackerrunnerwg.h"
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"
#include <QGroupBox>
const QString stylestr =  "QPushButton{font:12px;background-color:rgb(150,150,150)}"
                          "QLabel{font:12px;}"
                          "QPushButton:hover{background-color:rgba(220,220,220,110);}"
                          "QPushButton:pressed{background-color:rgba(85,170,255,150);}"
                          "QTableView{font:12px;selection-background-color:rgba(68,69,71,100);alternate-background-color: rgb(220, 220, 220);}"
                          "QMenu::item:selected {background-color:rgba(0,190,230,100);}"\
                          "QGroupBox:title{color:rgb(50,100,220);subcontrol-origin: margin;left: 0px;}"\
                          "QGroupBox{border: 1px solid lightgray;border-radius:2px;margin-top:6px;background-color:transparent}";
StackerRunnerWg::StackerRunnerWg(QWidget *parent):QWidget(parent)
{
    auto vbox = new QVBoxLayout;
    for(int i = 0; i < 2; ++i)
    {
        mconnectlabel[i] = new QLabel("",this);
        mIdlabel[i] = new QLabel("硬件ID",this);
        mConnectButton[i] = new QPushButton(tr("Connect"),this);
        mRunnerIP[i] = new QLineEdit(this);
        mRunnerIP[i]->setReadOnly(true);
        //界面数据布局规划
        auto hbox = new QHBoxLayout;
        hbox->addWidget(new QLabel("状态:"),1);
        hbox->addWidget(mconnectlabel[i],1);
        hbox->addWidget(mIdlabel[i],1);
        hbox->addWidget(new QLabel("IP:"),1,Qt::AlignRight);
        hbox->addWidget(mRunnerIP[i],4);
        hbox->addWidget(mConnectButton[i],1);
        hbox->addWidget(new QSplitter(this),12);
        auto gridbox = new QGridLayout;
        QGroupBox *monitorbox = new QGroupBox(this);
        monitorbox->setTitle("监视");
        monitorbox->setLayout(gridbox);
        QStringList list;
        list<<"可取"<<"可放"<<"可入库请求扫码"<<"可出库请求扫码";
        QMap<QString,QLabel*> RnamesMap;
        for(int k = 0; k < list.size(); ++k)
        {
            QString showstr = list[k]+":";
            gridbox->addWidget(new QLabel(showstr),0,k*2,1,1,Qt::AlignLeft);
            QLabel *lable = new QLabel();
            gridbox->addWidget(lable,0,k*2+1,1,1,Qt::AlignLeft);
            UpdateLabelState(0,lable);
            RnamesMap.insert(list[k],lable);
        }
        QStringList bitlist;
        bitlist<<"出库缓存状态"<<"入库缓存状态";
        QVector<QLabel *> bitstatevec;
        for(int n = 0; n < bitlist.size();++n)
        {
            QString showstr = bitlist[n]+":";
            gridbox->addWidget(new QLabel(showstr),1+n,0,1,1,Qt::AlignLeft);
            for(int m = 0;m < 8;++m)
            {
                QLabel *label = new QLabel();
                gridbox->addWidget(label,1+n,m+1,1,1,Qt::AlignLeft);
                UpdateLabelState(0,label);
                bitstatevec.append(label);
            }
        }
        vbox->addLayout(hbox,1);
        vbox->addWidget(monitorbox,6);
        vbox->addWidget(new QSplitter(this),1);
        QPixmap iconconnect(":/resouse/Image/grey.png");
        iconconnect = iconconnect.scaled(15,15);
        mconnectlabel[i]->setPixmap(iconconnect);
        if(i == 0)
        {
            m_RnamesMap.insert("出入库流道",RnamesMap);
            m_bitstateMap.insert("出入库流道",bitstatevec);
        }
        else{
            m_RnamesMap.insert("出库流道",RnamesMap);
            m_bitstateMap.insert("出库流道",bitstatevec);
        }
    }
    QStringList runnercmdlist;
    m_runneridCombox = new QComboBox(this);
    runnercmdlist<<"启动流道"<<"关闭流道"<<"入库扫码完成"<<"出库扫码完成"<<"取货完成"<<"放货完成"<<"盘点放货完成"<<"入库出箱";
    m_RunercmdComboBox = new QComboBox(this);
    m_RunercmdComboBox->addItems(runnercmdlist);
    runnercmdlist.clear();
    runnercmdlist<<"出入库流道"<<"出库流道";
    m_runneridCombox->addItems(runnercmdlist);
    m_startcmdBtn = new QPushButton("指令发送");
    connect(m_startcmdBtn,&QPushButton::clicked,this,&StackerRunnerWg::slotCmdDispatch);
    auto hopbox = new QHBoxLayout;
    hopbox->addWidget(new QLabel("选择流道:"),1);
    hopbox->addWidget(m_runneridCombox,2);
    hopbox->addWidget(new QLabel("执行指令:"),1);
    hopbox->addWidget(m_RunercmdComboBox,2);
    hopbox->addWidget(m_startcmdBtn,1);
    hopbox->addWidget(new QSplitter(this),10);

    QGroupBox *opbox = new QGroupBox("Operate:",this);
    opbox->setLayout(hopbox);
    vbox->addWidget(opbox);
    this->setFixedWidth(parent->width());
    this->setFixedHeight(parent->height()*2/3);
    this->setLayout(vbox);
    m_updateTimer = new QTimer();
    connect(m_updateTimer,&QTimer::timeout,this,&StackerRunnerWg::SlotUpdateState);
    m_updateTimer->start(200);
    this->setStyleSheet(stylestr);
}
///
/// \brief StackerRunnerWg::SetParam
/// \param runnerid
/// \param outrunnerid
///
void StackerRunnerWg::SetParam(int runnerid, int outrunnerid)
{
    m_runnerid = runnerid;
    m_outrunnerid = outrunnerid;
    mIdlabel[0]->setText("硬件ID:"+QString::number(m_runnerid));
    mIdlabel[1]->setText("硬件ID:"+QString::number(outrunnerid));
    if((!Myconfig::GetInstance()->m_RunnerMap.contains(runnerid))||(!Myconfig::GetInstance()->m_RunnerMap.contains(outrunnerid)))
        return;
    mRunnerIP[0]->setText(Myconfig::GetInstance()->m_RunnerMap[runnerid].deviceIp);
    mRunnerIP[1]->setText(Myconfig::GetInstance()->m_RunnerMap[outrunnerid].deviceIp);

}
///
/// \brief StackerRunnerWg::~StackerRunnerWg
///
StackerRunnerWg::~StackerRunnerWg()
{
    m_updateTimer->stop();

    m_RnamesMap.clear();
    m_bitstateMap.clear();

}
///
/// \brief StackerRunnerWg::UpdateRunnerState
/// \param id
///
void StackerRunnerWg::UpdateRunnerState(QString runnername)
{
    int id = -1;
    if(runnername == "出入库流道" )
        id = m_runnerid;
    else {
        id = m_outrunnerid;
    }
    if(!Myconfig::GetInstance()->m_RunnerMap.contains(id))
        return;
    for(auto  it = m_RnamesMap[runnername].begin(); it != m_RnamesMap[runnername].end();++it)
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
        UpdateLabelState(value,m_RnamesMap[runnername][it.key()]);
    }
    //更新 缓存区状态显示
    for(int i = 1; i < 3;++i)
    {
        uint8_t value = Myconfig::GetInstance()->m_RunnerMap[id].bitstate[i].statebit;
        for(int j = 0; j < 8;++j )
        {
            int bitvalue = (value>>j)&0x01;
            UpdateLabelState(bitvalue,m_bitstateMap[runnername].at(8*(i-1)+j));
        }
    }
}

void StackerRunnerWg::UpdateLabelState(int value, QLabel *label)
{
    if( value == 0)
    {
        QPixmap iconconnect(":/resouse/Image/grey.png");
        iconconnect = iconconnect.scaled(15,15);
        label->setPixmap(iconconnect);
    }
    else{
        QPixmap iconconnect(":/resouse/Image/green.png");
        iconconnect = iconconnect.scaled(15,15);
        label->setPixmap(iconconnect);
    }
}

void StackerRunnerWg::UpdateConnectState(QString runnername)
{
    int id = -1;
    int i = 0;
    if(runnername == "出入库流道" )
    {  id = m_runnerid;
        i = 0;
    }
    else {
        id = m_outrunnerid;
        i = 1;
    }
    if(!Myconfig::GetInstance()->m_RunnerMap.contains(id))
        return;
    bool connected = Myconfig::GetInstance()->m_RunnerMap[id].isOnline;
    mConnectButton[i]->setText(connected?tr("Disconnect"):tr("Connect"));
    mConnectButton[i]->setDisabled(connected);
    UpdateLabelState(static_cast<int>(connected),mconnectlabel[i]);
}
///
/// \brief StackerRunnerWg::SlotUpdateState
///
void StackerRunnerWg::SlotUpdateState()
{
    UpdateRunnerState("出入库流道");
    UpdateRunnerState("出库流道");
    //更新在线状态
    UpdateConnectState("出入库流道");
    UpdateConnectState("出库流道");
}
///
/// \brief StackerRunnerWg::slotCmdDispatch
///
void StackerRunnerWg::slotCmdDispatch()
{
    //test 指令发送的部分2021-01-23
    int id  = -1;
    if(m_runneridCombox->currentIndex() == 0)
    {
        id = m_runnerid;
    }
    else{
        id = m_outrunnerid;
    }
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
