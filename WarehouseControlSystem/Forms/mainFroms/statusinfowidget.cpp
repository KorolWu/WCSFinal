#include "statusinfowidget.h"

StatusInfoWidget::StatusInfoWidget(int width, int height, QWidget *parent) : QWidget(parent)
{
    this->m_width = width;
    this->m_height = height;
    //this->resize(this->m_width,this->m_height);

    init();
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&StatusInfoWidget::changeStatus);
    m_timer->start(1000);
}

void StatusInfoWidget::appendLog(QString &str)
{
    QString time_str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    m_textEdit->append(QString("[%1] %2").arg(time_str).arg(str));
}
///
/// \brief StatusInfoWidget::SetSumData
/// \param size
///
void StatusInfoWidget::SetSumData(int size)
{
    QString sizestr = QString::number(size);
    if(m_count->text() != sizestr)
    {
        m_count->setText(sizestr);
    }
}

void StatusInfoWidget::init()
{
    m_vbox = new QVBoxLayout();
    m_mainWidget = new QWidget(this);
    m_mainWidget->resize(m_width,m_height);
    QLabel *s = new QLabel("模块状态检测");
    m_vbox->addWidget(s);
    m_hbox = new QHBoxLayout();
    QLabel *l = new QLabel("WMS:");
    m_wmsLabel = new QLabel();
    m_wmsLabel->setFixedSize(30,30);
    m_hbox->addWidget(l);
    m_hbox->addWidget(m_wmsLabel);
    m_hbox->addStretch();

    QLabel *e = new QLabel("Elevator:");
    m_elevatorLabel = new QLabel();
    m_elevatorLabel->setFixedSize(30,30);
    m_hbox->addWidget(e);
    m_hbox->addWidget(m_elevatorLabel);
    m_hbox->addStretch();

    QLabel *b = new QLabel("Runner:");
    m_runner = new QLabel();
    m_runner->setFixedSize(30,30);
    m_hbox->addWidget(b);
    m_hbox->addWidget(m_runner);
    m_hbox->addStretch();

    QLabel *a = new QLabel("Redis:");
    m_agvLabel = new QLabel();
    m_agvLabel->setFixedSize(30,30);
    m_hbox->addWidget(a);
    m_hbox->addWidget(m_agvLabel);
    m_hbox->addStretch();

    QLabel *m = new QLabel("Mysql:");
    m_mysqlLabel = new QLabel();
    m_mysqlLabel->setFixedSize(30,30);
    m_hbox->addWidget(m);
    m_hbox->addWidget(m_mysqlLabel);
    m_hbox->addStretch();

    m_textEdit = new QTextEdit();
    m_textEdit->setFixedWidth(m_width-30);
   // m_textEdit->setStyleSheet("background:transparent;background-color: rgb(193, 205 ,205);color:rgb(79,79,79);font-family:Times;font:17px");
    m_textEdit->setStyleSheet("background:transparent;background-color: rgb(200, 200, 200,80);color:rgb(50,50,50);");
    m_textEdit->setReadOnly(true);
    m_vbox->addLayout(m_hbox);
    m_vbox->addStretch(1);
    m_vbox->addWidget(m_textEdit);
    m_vbox->addStretch(1);
    QLabel *ea = new QLabel();
    ea->setText("地址:");
    QLabel *address = new QLabel();
    address->setText("昆山市玉山镇恒盛路1299号2号房");
    QHBoxLayout *l1 = new QHBoxLayout();
    l1->addWidget(ea);
    l1->addWidget(address);
    l1->addStretch();


    QLabel *eb = new QLabel();
    eb->setText("总容量:");
    m_count = new QLabel();
    m_count->setText("1200");
    QHBoxLayout *l2  = new QHBoxLayout();
    l2->addWidget(eb);
    l2->addWidget(m_count);
    l2->addStretch();

    QLabel *et = new QLabel();
    et->setText("类型:");
    QLabel *type = new QLabel();
    type->setText(" 标件仓 ");
    QHBoxLayout *l3 = new QHBoxLayout();
    l3->addWidget(et);
    l3->addWidget(type);
    l3->addStretch();
    m_vbox->addLayout(l3);
    m_vbox->addLayout(l2);
    m_vbox->addLayout(l1);
    m_vbox->setStretchFactor(m_hbox,1);
    m_vbox->setStretchFactor(m_textEdit,14);
    m_vbox->setStretchFactor(l1,1);
    m_vbox->setStretchFactor(l2,1);
    m_vbox->setStretchFactor(l3,1);
    m_vbox->addStretch(2);

    m_mainWidget->setLayout(m_vbox);
    m_mainWidget->setStyleSheet("QLabel{font-family:Times; font:17px;background:transparent}");
}

void StatusInfoWidget::changeStatus()
{
    int a = 1;
    m_wmsLabel->setStyleSheet(a==1?"border-image:url(:/resouse/Image/ok (1).ico)":"border-image:url(:/resouse/Image/err.ico)");
    m_elevatorLabel->setStyleSheet(Myconfig::GetInstance()->m_elevatorMap.begin().value().status.isOnline?"border-image:url(:/resouse/Image/ok (1).ico)":"border-image:url(:/resouse/Image/err.ico)");
    m_runner->setStyleSheet(Myconfig::GetInstance()->m_runer.runneratastru.connectresult ==1?"border-image:url(:/resouse/Image/ok (1).ico)":"border-image:url(:/resouse/Image/err.ico)");
    m_agvLabel->setStyleSheet(Myconfig::GetInstance()->m_redisInfo.state==0?"border-image:url(:/resouse/Image/ok (1).ico)":"border-image:url(:/resouse/Image/err.ico)");
    m_mysqlLabel->setStyleSheet(a==1?"border-image:url(:/resouse/Image/ok (1).ico)":"border-image:url(:/resouse/Image/err.ico)");
}
