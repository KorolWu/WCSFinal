#include "mainwidget.h"
#include <Myconfig.h>
#include <QSqlQuery>

MainWidget::MainWidget(int width, int height, QWidget *parent) : QWidget(parent)
{
    this->m_width = width;
    this->m_height = height;
    initUI();
    m_updatetimer = new QTimer(this);
    connect(m_updatetimer,&QTimer::timeout,this,&MainWidget::SlotUpdateData);
    m_updatetimer->setInterval(5000);
    SlotUpdateData();
    m_pstatusInfoWidget->SetSumData(Myconfig::GetInstance()->m_storeinfoMap.size() );
    m_perrwidget->setSubTitle(QString("总计:%1").arg(m_totalErrCount));
    //updateTaskData();
    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    SlotUpdateTask(str);
}

MainWidget::~MainWidget()
{
    m_updatetimer->stop();
    delete m_updatetimer;
}

void MainWidget::initUI()
{
    m_pmain = new QWidget(this);
    m_pmain->resize(m_width,m_height);
    QFont font("宋体",18);
    QLabel *headLab = new QLabel("WCS 实时状态看板");
    headLab->setFont(font);
    headLab->setStyleSheet("background:transparent;");
   // qDebug()<<m_width<<"**"<<m_height;
    m_pgridlayout = new QGridLayout();
    m_pgridlayout->addWidget(headLab,0,0,1,2,Qt::AlignCenter);
    //仓库表格信息数据内容
    m_piew = new CPieWidget(m_width/2,m_height/2);//仓库位置分布图部署
    QMap<QString,float> warehousedatamap;
    warehousedatamap.insert("空闲:",2000);
    warehousedatamap.insert("已用:",200);
    warehousedatamap.insert("锁定:",2000);
    warehousedatamap.insert("其他:",200);
    m_piew->setData(warehousedatamap);
    m_piew->init();
    m_piew->setPieStyleSheet(PieStyle::DefaultPie);
    m_piew->update();
    m_piew->setCharTitle("仓位数据分布图");
    m_piew->setSubTitle(QString("总计:%1").arg(Myconfig::GetInstance()->m_storeinfoMap.size()));
    m_pstatusInfoWidget = new StatusInfoWidget(m_width/2,m_height/2);

    m_pgridlayout->addWidget(m_pstatusInfoWidget,1,0,10,1);
    m_tchar = new TorqueChart(m_width/2,m_height/2);
    connect(m_tchar,&TorqueChart::signalUpDateTime,this,&MainWidget::SlotUpdateTask);
    //addTestPonit();
    m_pgridlayout->addWidget(m_tchar,1,1,10,1);
   //故障信息数据分布图
    m_perrwidget = new CPieWidget(m_width/2,m_height/2);
    SetErrwidgetInfo();
    m_perrwidget->setPieStyleSheet(PieStyle::DountDefault);
    m_perrwidget->setCharTitle("故障分布图");


    m_perrwidget->update();
    m_pgridlayout->addWidget(m_perrwidget,11,1,10,1);

    m_pgridlayout->addWidget(m_piew,11,0,10,1);

    m_pmain->setLayout(m_pgridlayout);
    m_pmain->setStyleSheet("background-color: rgb(18, 62, 111, 40);border-radius:3px;");
}

void MainWidget::SetErrwidgetInfo()
{
    m_colorlist << QColor(116,169,26);
    m_colorlist << QColor(201,64,73);
    m_colorlist << QColor(210,151,153);
    m_colorlist << QColor(77,82,175);
    m_colorlist << QColor(158,102,155);
    m_colorlist << QColor(172,126,126);
    m_colorlist << QColor(90,179,86);
    QMap<QString,float> data;//1=小车故障 2=电梯故障 3=流道故障 4=流程故障 5=wms通讯 6=数据存储故障  7=其他故障
    data.insert("小车:",6);
    data.insert("流程:",3);
    data.insert("电梯:",2);
    data.insert("流道:",5);
    data.insert("WMS:",1);
    data.insert("数据存储:",1);
    data.insert("其他:",1);
    m_perrwidget->setColors(m_colorlist);
    m_perrwidget->setData(data);
    m_perrwidget->init();
}

void MainWidget::addTestPonit()
{
//    for(int i = 0;i < 24;i++)
//    {
//        m_tchar->AddChartPoint(i,qrand()/10);
//    }
}

void MainWidget::UpdateWarehouseData()
{
    QMap<QString,float> storestathash;
    storestathash.insert("空闲:",0);
    storestathash.insert("已用:",0);
    storestathash.insert("锁定:",0);
    storestathash.insert("其他:",0);
    //更新仓位仓库表的信息
    for(auto it = Myconfig::GetInstance()->m_storeinfoMap.begin(); it!=Myconfig::GetInstance()->m_storeinfoMap.end();++it )
    {
        char state = it.value().storestat;
        switch (state) {
        case 0:
            storestathash["空闲:"]++;
            break;
        case 1:
            storestathash["已用:"]++;
            break;
        case 2:
            storestathash["锁定:"]++;
            break;
        default:
            break;
        }
    }
    storestathash["其他:"] = Myconfig::GetInstance()->m_storeinfoMap.size() - storestathash["空闲:"]-storestathash["已用:"]-storestathash["锁定:"];
    m_piew->setData(storestathash);
}
///
/// \brief MainWidget::UpdateAlarmlogData
///
void MainWidget::UpdateAlarmlogData()
{
   QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QMap<QString,float> data;
    data.insert("小车:",0);
    data.insert("流程:",0);
    data.insert("电梯:",0);
    data.insert("流道:",0);
    data.insert("WMS:",0);
    data.insert("数据存储:",0);
    data.insert("其他:",0);
    //查询报警信息表格得到数据size //QString sqlstr = QString("SELECT * FROM t_alarmInfo WHERE %1 AND %2 AND %3 AND %4;").arg(deciveid).arg(stat).arg(level).arg(errortype);

    //1=小车故障 2=电梯故障 3=流道故障 4=流程故障 5=wms通讯 6=数据存储故障  7=其他故障
    for(int i = 1; i < 7;++i)
    {
        QString slecttstr = QString("SELECT count(ID) FROM t_alarmInfo WHERE Errorcode = %1").arg(i);
        QSqlQuery sql ;
        sql.exec(slecttstr);
        sql.next();
        int num = sql.value("count(ID)").toInt();
        switch (i) {
        case 1:
            data["小车:"] = num;
            break;
        case 2:
            data["电梯:"] = num;
            break;
        case 3:
            data["流道:"] = num;
            break;
        case 4:
            data["流程:"] = num;
            break;
        case 5:
            data["WMS:"] = num;
            break;
        case 6:
            data["数据存储:"] = num;
            break;
        default:
            break;
        }
    }
    QString slecttstr = QString("SELECT count(ID) FROM t_alarmInfo ");
    QSqlQuery sql ;
    sql.exec(slecttstr);
    sql.next();
    int sum = sql.value("count(ID)").toInt();
     data["其他:"] = sum - (data["小车:"]+ data["电梯:"] + data["流道:"]+data["流程:"]+data["WMS:"]+data["数据存储:"]);
    m_totalErrCount = sum;
    m_perrwidget->setData(data);
}

void MainWidget::updateTaskData()
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QMap<int,int> m_xyvalue;
    for(int i = 0;i < 24;i++)
    {
        m_xyvalue.insert(i,0);
        //m_tchar->AddChartPoint(i,qrand()/10);
    }
    //测试表格的任务数据查询数据
    QString str = QString("select HOUR(beginTime) as HOUR, \
                          COUNT(1) as COUNT  from t_all_task  \
                          where DATE_FORMAT(beginTime,'%Y-%m-%d') = '2020-07-21' GROUP BY HOUR(beginTime)");
    QSqlQuery sql;
    sql.exec(str);
    while (sql.next()) {
        int hour = sql.value("HOUR").toInt();
        int count = sql.value("COUNT").toInt();
        //qDebug()<<"查询到的数据: time"<<hour << "count:"<<count;
        if(m_xyvalue.contains(hour))
        {
            if(m_xyvalue[hour] != count)
            {
                m_xyvalue[hour]=count;
            }
        }
    }
    //求出最大值
    int max = 0;
    for(auto it = m_xyvalue.begin(); it != m_xyvalue.end(); ++it)
    {
        if(it.value() > max)
        {
            max = it.value();
        }
    }

   for(auto it = m_xyvalue.begin(); it != m_xyvalue.end(); ++it)
    {
        m_tchar->AddChartPoint(it.key(),it.value());
    }
}

void MainWidget::appLog(QString str)
{
    m_pstatusInfoWidget->appendLog(str);
}
///
/// \brief MainWidget::StartUpdateShowInfo
///数据库正常则打开数据进行更新
void MainWidget::StartUpdateShowInfo()
{
    if(!m_updatetimer->isActive())
    {
        m_updatetimer->start();
    }
}
//关闭页面更新的timer数据
void MainWidget::CloseShowInfo()
{
    if(m_updatetimer->isActive())
    {
       m_updatetimer->stop();
    }
}
///
/// \brief MainWidget::SlotUpdateData
///更新两个界面的数据信息
void MainWidget::SlotUpdateData()
{
  UpdateWarehouseData();
  UpdateAlarmlogData();
}
//2020-07-21
void MainWidget::SlotUpdateTask(QString datestr)
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    m_tchar->ClearChart();
    QMap<int,int> m_xyvalue;
    for(int i = 0;i < 24;i++)
    {
        m_xyvalue.insert(i,0);
        //m_tchar->AddChartPoint(i,qrand()/10);
    }
    //测试表格的任务数据查询数据
    QString str = QString("select HOUR(beginTime) as HOUR, \
                          COUNT(1) as COUNT  from t_all_task  \
                          where DATE_FORMAT(beginTime,'%Y-%m-%d') = '%1' GROUP BY HOUR(beginTime)").arg(datestr);
    QSqlQuery sql;
    sql.exec(str);
    while (sql.next()) {
        int hour = sql.value("HOUR").toInt();
        int count = sql.value("COUNT").toInt();
        //qDebug()<<"查询到的数据: time"<<hour << "count:"<<count;
        if(m_xyvalue.contains(hour))
        {
            if(m_xyvalue[hour] != count)
            {
                m_xyvalue[hour]=count;
            }
        }
    }
    //求出最大值
    int max = 0;
    for(auto it = m_xyvalue.begin(); it != m_xyvalue.end(); ++it)
    {
        if(it.value() > max)
        {
            max = it.value();
        }
    }
   for(auto it = m_xyvalue.begin(); it != m_xyvalue.end(); ++it)
    {
        m_tchar->AddChartPoint(it.key(),it.value());
    }
}
