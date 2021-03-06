#include "alltask.h"

AllTask::AllTask(int width, int height, QWidget *parent):BaseFrom(parent)
{
    this->m_width = width;
    this->m_height = height;
    this->resize(m_width,m_height);
    int height_fristLine = height/94;
    m_in_btn = new QPushButton("只看入库",this);
    m_in_btn->setIcon(QIcon(":/resouse/Image/in.png"));
    //connect(m_in_btn,&QPushButton::clicked,this,&CurrentTask::onAddClicked);
    m_in_btn->move(m_width/158.5,height_fristLine);
    m_out_btn= new QPushButton("只看出库",this);
    //connect(m_out_btn,&QPushButton::clicked,this,&CurrentTask::onDeleteClicked);
    m_out_btn->setIcon(QIcon(":/resouse/Image/out.png"));
    m_out_btn->move(m_width/10.5,height_fristLine);
    m_all_btn= new QPushButton("查看全部",this);
    m_all_btn->move(m_width/5.3,height_fristLine);
    m_all_btn->setIcon(QIcon(":/resouse/Image/all.png"));

    m_start_date = new QDateEdit(this);
    m_start_date->setDateTime(QDateTime::currentDateTime());
    m_start_date->setCalendarPopup(true);
    m_start_date->move(m_width/3.5,height_fristLine);

    m_end_date = new QDateEdit(this);
    m_end_date->setDateTime(QDateTime::currentDateTime());
    m_end_date->setCalendarPopup(true);
    m_end_date->move(m_width/2.64,height_fristLine);

    m_query_btn = new QPushButton("查询",this);
    m_query_btn->move(m_width/2.11,height_fristLine);

    m_Refresh_btn = new QPushButton("刷新",this);
    m_Refresh_btn->setIcon(QIcon(":/resouse/Image/Refresh.png"));

    m_Refresh_btn->move(m_width/1.8,height_fristLine);
    connect(m_Refresh_btn,&QPushButton::clicked,this,&AllTask::onRefreshed);
    m_hbox = new QHBoxLayout();
    m_hbox->addWidget(m_in_btn);
    m_hbox->addWidget(m_out_btn);
    m_hbox->addWidget(m_all_btn);
    m_hbox->addWidget(m_start_date);
    m_hbox->addWidget(m_end_date);
    m_hbox->addWidget(m_query_btn);
    m_hbox->addWidget(m_Refresh_btn);
    m_hbox->addStretch();

    this->setStyleSheet("QPushButton{font: 14px;min-width:100px;min-height:33px;background-color:rgb(150,150,150);}QPushButton:hover{background: rgb(220, 220, 220);}QPushButton:pressed{background-color:rgb(85, 170, 255);}QLabel{font: 16px}");
    initTableView();
}

void AllTask::initTableView()
{
    m_table_view = new QTableView();
    m_table_view->horizontalHeader()->setStyleSheet(headstlye);
    m_table_view->move(5,60);
    m_table_view->verticalHeader()->hide();
    m_table_view->resize(m_width,m_height);
    m_vbox = new QVBoxLayout();
    m_vbox->addLayout(m_hbox);
    m_vbox->addWidget(m_table_view);
    model = new QSqlQueryModel();
    //model->setTable("t_all_task");
    m_table_view->setModel(model);
    //model->select();

    //page ob 功能实现方式
    m_pagewg = new SpiltPagesByQSqlTableModel();
    m_pagewg->SetParam(model,"t_all_task",25);
    m_pagewg->InitpagefunWg();
    m_pagewg->move(m_width/2-2*m_pagewg->width(),m_height-60);
    m_vbox->addWidget(m_pagewg);
    this->setLayout(m_vbox);

    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    LabelDelegateV1 *l1 = new LabelDelegateV1();
    m_table_view->setItemDelegateForColumn(2,l1);

    QStringList header;
    header<<"ID"<<"任务号"<<"任务状态"<<"料箱号"<<"优先级"<<"来源"<<"目标"<<"穿梭车号"<<"创建时间"<<"结束时间";
    for(int i = 0 ;i <header.size();i++)
    {
        model->setHeaderData(i, Qt::Horizontal, header.at(i));
    }
    setTableStysle();
}

void AllTask::onRefreshed()
{
//    QString taskNum = "";
//     QString sql = QString("select * from t_all_task where taskNum = %1;").arg(taskNum);
//    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
//    model->setQuery(sql);
//    model->query();
//    //根据条件查询选择
//    int total =  model->rowCount();
//    sql.chop(1);
//    if(taskNum == "")
//    {
         QString sql = "select * from t_all_task ORDER BY id DESC LIMIT 300;";
        model->setQuery("select * from t_all_task ORDER BY id DESC LIMIT 300;");
        model->query();
        int total = model->rowCount();
        qDebug()<<"total"<<total;
        sql = "";
 //    }
    //  qDebug()<<sql << total;
    m_pagewg->updateParam(total,sql);
    //SetTableviewsetColumnWidth();
    setTableStysle();
}

void AllTask::setTableStysle()
{
    m_table_view->setColumnWidth(0,m_width/20*1);
    m_table_view->setColumnWidth(1,m_width/20*2);
    m_table_view->setColumnWidth(2,m_width/20*2);
    m_table_view->setColumnWidth(3,m_width/20*2);

    m_table_view->setColumnWidth(4,m_width/20*2);
    m_table_view->setColumnWidth(5,m_width/20*2);
    m_table_view->setColumnWidth(6,m_width/20*2);
    m_table_view->setColumnWidth(7,m_width/20*1);
    m_table_view->setColumnWidth(8,m_width/20*3);
    m_table_view->setColumnWidth(9,m_width/20*3);
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
}
