#include "torquechart.h"
#include <QDebug>
#include <QRadioButton>
#include <QSplitter>

TorqueChart::TorqueChart(int w, int h, QWidget *parent)
{
    Q_UNUSED(parent);
    //曲线
    m_plinseries = new QSplineSeries();
    m_plinseries->setColor(QColor(255,0,0));
    m_plinseries->setPen(QPen(QBrush(QColor(255, 215, 0)), 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
    //图表
    m_pchart = new QChart();
    // m_pchart->setTheme(QChart::ChartThemeBlueNcs);
    m_pchart->setBackgroundBrush(QBrush(QColor(18, 162, 111, 4)));
    //chart容器
    m_pmainWidget = new QWidget(this);
    m_pHbox = new QVBoxLayout();
    m_choosedate = new QDateEdit(m_pmainWidget);
    m_choosedate->setStyleSheet("background-color:rgb(215,215,215);border-style: none;font:14px;color:rgb(50,50,50);border-width: 1px;border-color: rgb(100,100,100);spacing: 4px;");
    m_choosedate->setDateTime(QDateTime::currentDateTime());
    m_choosedate->setCalendarPopup(true);
    m_choosedate->setDisplayFormat("yyyy-MM-dd");
    m_choosedate->setFixedWidth(this->width()/5);
    connect(m_choosedate,&QDateEdit::dateChanged,this,&TorqueChart::slotUpDateLastDate);
    //qDebug()<<"日期格式"<<m_choosedate->text();
    //m_pHbox->addWidget(m_choosedate);
    //增加QHBOXLAYOUT
    QHBoxLayout  *pHbox = new QHBoxLayout();
    pHbox->addWidget(m_choosedate);
    QString str0[] = {"按天","按月","按年"};
    m_group = new QButtonGroup(this);
    connect(m_group, SIGNAL(buttonToggled(int,bool)), this, SLOT(slotoperatingButtonsToggled(int,bool)));
    for(int i = 0;i < 3;i++)
    {
        QRadioButton *radio = new QRadioButton(str0[i],this);
        //radio->setStyleSheet("QRadioButton{spacing: 2px;color: white;border-style: solid; background-color: LightGray;}QRadioButton:indicator{width: 45px;height: 30px; }");
        radio->setStyleSheet("background:transparent;");
        m_group->addButton(radio);
        pHbox->addWidget(radio);
    }
    m_group->buttons()[0]->setChecked(true);
    m_pHbox->addLayout(pHbox,2);

    m_pchatrview = new QChartView(m_pchart,m_pmainWidget);
    m_pHbox->addWidget(m_pchatrview,6);
   //m_pHbox->addSpacing(10);
    QSplitter *splitter = new QSplitter;
    splitter->setAutoFillBackground(false);
    m_pHbox->addWidget(splitter,1);
    m_pHbox->setContentsMargins(10,10,10,10);
    m_pmainWidget->setLayout(m_pHbox);
    m_pmainWidget->resize(w,h);
    //操作图表的轴
    m_paxis = new QValueAxis;
    m_pyaxis = new QValueAxis;
    //QDateTimeAxis  TimeAxis;
    //TimeAxis->setFormat("hh:mm");
    m_paxis->setLabelFormat("%u"); //设置X刻度的格式
    m_pyaxis->setLabelFormat("%u"); //设置Y刻度的格式
    m_pchart->legend()->hide();
    //增加线条
    m_pchart->addSeries(m_plinseries);
    //niaoyonmgmeiyou
    m_pchart->createDefaultAxes();
    m_pchart->setTitle("<h3>今日任务时间分配图</h3>");
    m_pchart->axisX()->setRange(0,24);
    // m_pchart->axisY()->setRange(0,200);
    m_pchart->setAxisY(m_pyaxis,m_plinseries);
    m_pchart->axisY()->setTitleText("任务数量(个)");
    m_pyaxis->setTickCount(10);
    m_pchart->setAxisX(m_paxis,m_plinseries);
    m_pchart->axisX()->setTitleText("Time(sec)");
    m_paxis->setTickCount(13);
    //this->setLayout(m_pHbox);
    //this->setGeometry(50,50,w,h);
    //this->setStyleSheet(tr("background-color:rgb(35,35,35);color:white;border-radius:15px;"));

}

void TorqueChart::slotUpDateLastDate()
{
    if(m_group->buttons()[0]->isChecked())
    {
        QString datestr = m_choosedate->text();
        //qDebug()<<"改变日期格式"<<datestr;
        m_pchart->setTitle(QString("<h3>[%1]任务时间分配图</h3>").arg(datestr));
        emit signalUpDateTime(datestr);
    }
}

void TorqueChart::slotoperatingButtonsToggled(int id, bool status)
{
    Q_UNUSED(id);
    Q_UNUSED(status);
    //int tmp = m_group->checkedId();
    QString str = m_group->checkedButton()->text();
   //QByteArray byteArray = str.toLocal8Bit();
   //qDebug("flag = %d, status = %d, tmp = %d, checked = %s", id, status, tmp, byteArray.data());
}

///
/// \brief TorqueChart::AddChartPoint
/// \param x
/// \param y
///判断x和y的值 超值就重新设置范围
void TorqueChart::AddChartPoint(int x, int y)
{
    //    if(x >= m_paxis->max() *0.9)
    //    {
    //        m_pchart->axisX()->setRange(m_paxis->min(),x * 1.1);
    //    }
    //    if(x < m_paxis->min() )
    //    {
    //        m_pchart->axisX()->setRange(x * 1.1,m_paxis->max());
    //    }
    if(y >= m_pyaxis->max() *0.9)
    {
        m_pchart->axisY()->setRange(m_pyaxis->min(),y *1.1);
    }
    if(y < m_pyaxis->min())
    {
        m_pchart->axisY()->setRange(y * 1.1,m_pyaxis->max());
    }
    m_plinseries->append(x,y);

}
///
/// \brief TorqueChart::ClearChart
///清除曲线并重新设置 X,Y 的值
void TorqueChart::ClearChart()
{
    m_plinseries->clear();
    //m_pchart->axisX()->setRange(0,3.5);
    //m_pchart->axisY()->setRange(0,3.5);
}

TorqueChart::~TorqueChart()
{
    delete m_pchart;
    delete m_pHbox;
    delete m_plinseries;
    delete m_pchatrview;
    delete m_paxis;
    delete m_pyaxis;
    //delete this;
}
