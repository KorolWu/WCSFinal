#include "subtask.h"
#include <QDebug>
SubTask::SubTask(int width, int height, QWidget *parent) : BaseFrom(parent)
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

    m_refresh_btn = new QPushButton(this);
    connect(m_refresh_btn,&QPushButton::clicked,this,&SubTask::selectTask);
    m_refresh_btn->setIcon(QIcon(":/resouse/Image/Refresh.png"));
    m_refresh_btn->move(m_width/3,height_fristLine);
    m_task_edit = new QLineEdit(this);
    m_task_edit->resize(200,34);
    m_task_edit->setPlaceholderText("请输入任务号");
    m_task_edit->move(m_width/2.94,height_fristLine);

    m_query_btn = new QPushButton("查询",this);
    connect(m_query_btn,&QPushButton::clicked,this,&SubTask::selectTask);
    m_query_btn->move(m_width/2.11,height_fristLine);
    m_hbox = new QHBoxLayout();
    m_hbox->addWidget(m_in_btn);
    m_hbox->addWidget(m_out_btn);
    m_hbox->addWidget(m_all_btn);
    m_hbox->addWidget(m_task_edit);
    m_hbox->addWidget(m_query_btn);
    m_hbox->addWidget(m_refresh_btn);
    m_hbox->addStretch();

//    m_refresh_btn = new QPushButton("Refresh",this);
//    connect(m_refresh_btn,&QPushButton::clicked,this,&SubTask::onRefresh);
    this->setStyleSheet("QPushButton{font: 14px;width:100px;height:33;background-color:rgb(150,150,150);}QPushButton:hover{background: rgb(220, 220, 220);}QPushButton:pressed{background-color:rgb(85, 170, 255);}QLabel{font: 16px}");

    initTableView();
}

void SubTask::initTableView()
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
    //model->setQuery("select * from t_sub_taskInfo;");
    //model->query();
    m_table_view->setModel(model);
    //model->select();
    //page ob 功能实现方式
    m_pagewg = new SpiltPagesByQSqlTableModel();
    m_pagewg->SetParam(model,"t_sub_taskInfo",25);
    m_pagewg->InitpagefunWg();
    m_pagewg->move(m_width/2-2*m_pagewg->width(),m_height-60);
    m_vbox->addWidget(m_pagewg);
    this->setLayout(m_vbox);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    LabelDelegateV1 *l1 = new LabelDelegateV1();
    m_table_view->setItemDelegateForColumn(2,l1);
    m_table_view->setItemDelegateForColumn(5,l1);

    QStringList header;
    header<<"ID"<<"任务号"<<"任务类型"<<"子任务类型"<<"顺序"<<"状态"<<"库位号"<<"穿梭车号"<<"发送内容"<<"创建时间"<<"完成时间";
    for(int i = 0 ;i <header.size();i++)
    {
        model->setHeaderData(i, Qt::Horizontal, header.at(i));
    }
    m_table_view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    SetTableviewsetColumnWidth();
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
}

void SubTask::SetTableviewsetColumnWidth()
{
    m_table_view->setColumnWidth(0,m_width/20*1);
    m_table_view->setColumnWidth(1,m_width/20*2);
    m_table_view->setColumnWidth(2,m_width/20*2);
    m_table_view->setColumnWidth(3,m_width/20*2);

    m_table_view->setColumnWidth(4,m_width/20*1);
    m_table_view->setColumnWidth(5,m_width/20*1);
    m_table_view->setColumnWidth(6,m_width/20*2);
    m_table_view->setColumnWidth(7,m_width/20*1);
    m_table_view->setColumnWidth(8,m_width/20*2);
    m_table_view->setColumnWidth(9,m_width/20*3);
     m_table_view->setColumnWidth(10,m_width/20*3);
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
}

void SubTask::selectTask()
{
    QString taskNum = m_task_edit->text();
    QString sql = QString("select * from t_sub_taskInfo where taskNum = %1;").arg(taskNum);
   // QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    model->setQuery(sql);
    model->query();
    //根据条件查询选择
    int total =  model->rowCount();
    sql.chop(1);
    if(taskNum == "")
    {
        sql = "";
        model->setQuery("select * from t_sub_taskInfo ORDER BY id DESC LIMIT 300;");
        model->query();
        total = model->rowCount();
    }
    //  qDebug()<<sql << total;
    m_pagewg->updateParam(total,sql);
    SetTableviewsetColumnWidth();
}

void SubTask::onRefresh()
{
    QString sql = "select * from t_sub_taskInfo ORDER BY id DESC LIMIT 300;";
    int total = 300;
    model->setQuery(sql);
    model->query();
    m_pagewg->updateParam(total,sql);
    SetTableviewsetColumnWidth();
}
