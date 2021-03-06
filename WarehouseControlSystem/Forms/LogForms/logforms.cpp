#include "logforms.h"
#include <QDebug>

LogForms::LogForms(int width, int height,QWidget *parent):BaseFrom(parent)
{
    this->m_width = width;
    this->m_height = height;
    qDebug()<<m_width <<";"<<m_height;
    QLabel *q = new QLabel("来源",this);
    int height_fristLine = m_height/94;
    q->move(m_width/100,height_fristLine);
    m_com_box = new QComboBox(this);
    m_com_box->setToolTip("--select--");
    m_com_box->addItems(QStringList("--select--"));

    m_com_box->move(m_width/14,height_fristLine);
    m_all_btn= new QPushButton("查看全部",this);
    m_all_btn->move(m_width/5,height_fristLine);
    m_all_btn->setIcon(QIcon(":/resouse/Image/all.png"));

    m_start_date = new QDateEdit(this);
    m_start_date->setDateTime(QDateTime::currentDateTime());
    m_start_date->setCalendarPopup(true);
    m_start_date->move(int(m_width/3.5),height_fristLine);
    QLabel *l = new QLabel(this);
    l->setText("-");
    l->move(int(m_width/2.7),height_fristLine);
    m_end_date = new QDateEdit(this);
    m_end_date->setDateTime(QDateTime::currentDateTime());
    m_end_date->setCalendarPopup(true);
    m_end_date->move(int(m_width/2.6),height_fristLine);

    m_query_btn = new QPushButton("查询",this);
    m_query_btn->move(int(m_width/2.1),height_fristLine);
    m_query_btn->setStyleSheet("background-color:rgb(0,170,255)");
    this->setStyleSheet("QPushButton{font: 14px;width:100px;height:25;}QLabel{font: 16px}QDateEdit{width:100px;height:25px}QComboBox{width:120px;height:25px}");
    initTableView();
}

void LogForms::initTableView()
{
    m_table_view = new QTableView(this);
    m_table_view->horizontalHeader()->setStyleSheet(headstlye);
    m_table_view->move(5,60);
    m_table_view->verticalHeader()->hide();

    m_table_view->resize(m_width,int(m_height*0.9));

    model = new QSqlTableModel();
    model->setTable("t_log");
    m_table_view->setModel(model);
    model->select();
    //page ob 功能实现方式
    m_pagewg = new SpiltPagesByQSqlTableModel(this);
    m_pagewg->SetParam(model,"t_log",25);
    m_pagewg->InitpagefunWg();
    m_pagewg->move(580,900);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);


    QStringList header;
    header<<"ID"<<"等级"<<"来自"<<"信息"<<"时间";
    for(int i = 0 ;i <header.size();i++)
    {
        model->setHeaderData(i, Qt::Horizontal, header.at(i));
    }
    m_table_view->setColumnWidth(0,m_width/20*2);
    m_table_view->setColumnWidth(1,m_width/20*2);
    m_table_view->setColumnWidth(2,m_width/20*3);
    m_table_view->setColumnWidth(3,m_width/20*8);
    m_table_view->setColumnWidth(4,m_width/20*4);
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
   // m_table_view->
}
