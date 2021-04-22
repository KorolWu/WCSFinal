#include "elevatorstatus.h"
#include <QFile>

ElevatorStatusWidget::ElevatorStatusWidget(int elevatorId, QWidget *parent) : QWidget(parent)
{
    this->m_elevatorId = elevatorId;
    this->resize(400,300);
    //this->setStyleSheet("background-color:white;");

    QFile file(":/resouse/warehouse.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
    m_ptimer = new QTimer(this);
    connect(m_ptimer,&QTimer::timeout,this,&ElevatorStatusWidget::updateStatus);
    initUI();
    m_ptimer->start(500);
    qDebug()<<"current clicked elevator id is "<<m_elevatorId;
}

void ElevatorStatusWidget::updateStatus()
{
    Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.isOnline? m_pOnline_image->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pOnline_image->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.isLock? m_pLock_lab->setStyleSheet("border-image:url(:/resouse/Image/green.png)"):m_pLock_lab->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    m_pCurrent_layer->setValue(Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.curruntLayer);
    m_pcurachelayer->setValue(Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.curachelayer);

}

void ElevatorStatusWidget::onCheckBoxStatusChange()
{
    if(m_pSelectRadioButton->text() == "立库")
        m_pSelectRadioButton->setText("缓存");
    else
        m_pSelectRadioButton->setText("立库");
}

void ElevatorStatusWidget::onResetClicked()
{
    if(Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.isLock)
        Myconfig::GetInstance()->m_elevatorMap[m_elevatorId].status.isLock = false;
    Myconfig::GetInstance()->m_elevatorMutex.unlock();
}

void ElevatorStatusWidget::initUI()
{
    v = new QVBoxLayout(this);
    v->addStretch();
    QHBoxLayout *h = new QHBoxLayout();
    QLabel *l = new QLabel("ID :");
    QLabel *i = new QLabel(QString("%1").arg(m_elevatorId));
    h->addWidget(l);
    h->addStretch();
    h->addWidget(i);
    h->addStretch();
    m_pOnline_image = new QLabel();
    m_pOnline_image->setFixedSize(15,15);
    m_pOnline_image->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    m_pOnline_lab = new QLabel("在线");
    h->addWidget(m_pOnline_lab);
    h->addWidget(m_pOnline_image);
    h->addStretch();
    m_pLock_lab = new QLabel();
    m_pLock_lab->setFixedSize(15,15);
    m_pLock_lab->setStyleSheet("border-image:url(:/resouse/Image/grey.png)");
    QLabel *lock = new QLabel("使用中");
    h->addWidget(lock);
    h->addWidget(m_pLock_lab);
    h->addStretch();
    m_pReset = new QPushButton("复位");
    connect(m_pReset,&QPushButton::clicked,this,&ElevatorStatusWidget::onResetClicked);
    h->addWidget(m_pReset);
    h->addStretch();
    v->addLayout(h);
    v->addStretch();
    h = new QHBoxLayout();
    QLabel *c = new QLabel("当前层 :");
    m_pCurrent_layer = new QSpinBox();
    h->addWidget(c);
    h->addWidget(m_pCurrent_layer);
    QLabel *e = new QLabel("缓存层");
    m_pcurachelayer = new QSpinBox();
    h->addWidget(e);
    h->addWidget(m_pcurachelayer);
    v->addLayout(h);
    v->addStretch();

    m_pSelectRadioButton = new QCheckBox("立库");
    m_pSelectRadioButton->resize(50,30);
    connect(m_pSelectRadioButton,&QCheckBox::stateChanged,this,&ElevatorStatusWidget::onCheckBoxStatusChange);
    m_pMove = new QPushButton("Move");
    connect(m_pMove,&QPushButton::clicked,this,&ElevatorStatusWidget::onMove);
    m_pOrder_layer = new QSpinBox();
    h = new QHBoxLayout();
    h->addWidget(m_pSelectRadioButton);
    h->addWidget(m_pMove);
    h->addWidget(m_pOrder_layer);
    v->addLayout(h);
    v->addStretch();
    this->setLayout(v);

}

void ElevatorStatusWidget::onMove()
{
    OrderStru o;
    o.z = m_pOrder_layer->value();
    o.order = Call;
    if(m_pSelectRadioButton->isChecked())
    {
        o.startaddress = 52;
    }
    else
    {
        o.startaddress = 50;
    }
    o.Datatype = 4;  //varint type
    o.childtype = 2; //write or read 5=read other is write
    AbstructInstruction *e = new CarElevatorInstruction();
    e->setParameter(o,68);
    e->runInstruction();
    QString resultMsg ="";
    e->getResult(resultMsg);
}

