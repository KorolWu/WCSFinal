#include "elevatorfrom.h"
#include <QHeaderView>
#include <QIcon>
#include <QDebug>
#include <Forms/CarForms/labeldelegate.h>
#include "Forms/StorePosInfo/buttondelegate.h"
#include <QScreen>
ElevatorFrom::ElevatorFrom(int width, int height,QWidget *parent) : BaseFrom(parent)
{
    this->m_width = width;
    this->m_height = height;
    int height_fristLine = height/94;
    p_add_btn = new QPushButton("添加",this);
    p_add_btn->setIcon(QIcon(":/resouse/Image/add.png"));
    connect(p_add_btn,&QPushButton::clicked,this,&ElevatorFrom::onAddClicked);
    p_add_btn->move(m_width/158,height_fristLine);
    p_delete_btn= new QPushButton("删除",this);
    connect(p_delete_btn,&QPushButton::clicked,this,&ElevatorFrom::onDeleteClicked);
    p_delete_btn->setIcon(QIcon(":/resouse/Image/delete.png"));
    p_delete_btn->move(int(m_width/15.8),height_fristLine);
    p_export_btn= new QPushButton("导出",this);
    p_export_btn->move(int(m_width/7.9),height_fristLine);
    p_export_btn->setIcon(QIcon(":/resouse/Image/download.png"));

    QLabel *text_lable = new QLabel("设备编号:",this);
    text_lable->move(int(m_width/4.5),height_fristLine);
    text_lable->setAlignment(Qt::AlignBottom);
    p_input_text = new QLineEdit(this);
    p_input_text->resize(150,30);
    p_input_text->move(int(m_width/3.6),height_fristLine);
    p_query_btn = new QPushButton("查询",this);
    p_query_btn->move(int(m_width/2.6),height_fristLine);
    p_query_btn->setStyleSheet("background-color:rgb(0,170,255)");

    p_table_view = new QTableView(this);
    p_table_view->horizontalHeader()->setStyleSheet(headstlye);
    p_table_view->move(5,60);
    p_table_view->verticalHeader()->hide();
    p_table_view->resize(width,height);
    int row_count = Myconfig::GetInstance()->m_elevatorMap.size();
    model = new QStandardItemModel(row_count,8);
    p_table_view->setModel(model);

    p_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    p_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    LabelDelegate *delegate = new LabelDelegate();
    p_table_view->setItemDelegateForColumn(7,delegate);
    QStringList header;
    header<<"编号"<<"类型"<<"通讯地址"<<"端口"<<"靠近点位置"<<"位置"<<"状态"<<"编辑"<<"操作按钮";//<<"当前层"<<"目标层"
    for(int i = 0 ;i <header.size();i++)
    {
        model->setHeaderData(i, Qt::Horizontal, header.at(i));
    }
    p_table_view->setColumnWidth(0,width/20*2);
    p_table_view->setColumnWidth(1,width/20*2);
    p_table_view->setColumnWidth(2,width/20*3);
    p_table_view->setColumnWidth(3,width/20*2);

    p_table_view->setColumnWidth(4,width/20*2);
    p_table_view->setColumnWidth(5,width/20*2);
    p_table_view->setColumnWidth(6,width/20*2);
    p_table_view->horizontalHeader()->setMinimumHeight(40);
    // is online and status is a part of struct elevator
    for(int i = 0;i < 9; i++)
    {

    }
    p_table_view->setFont(QFont("宋体",12)); //设置字体
    setTableViewValue();
   //p_table_view->setFont(QFont("宋体",26)); //设置字体
    this->setStyleSheet("QPushButton{font: 14px;width:70px;height:25;background-color:rgb(150,150,150);}QPushButton:hover{background: rgb(220, 220, 220);}QPushButton:pressed{background-color:rgb(85, 170, 255);}QLabel{font: 16px}");

}

void ElevatorFrom::setTableViewValue()
{

//    for(auto au = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.begin(); au != Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.end();au++ )
//    {
//        if(au.value().protype == 2)
//        {
//            ElevatorInfoStru e;
//            e.elevatorIp = au.value().name;
//            e.elevatorPort = au.value().port;
//            e.elevatorType = "Elevator";
//            e.elevatorNum = QString("%1").arg(au.value().ID);
//            Myconfig::GetInstance()->m_elevatorMap.insert(au.value().ID,e);
//        }
//    }
    model->removeRows(0, model->rowCount());
    int row_count = Myconfig::GetInstance()->m_elevatorMap.size();
    for(int j = 0; j< 9 ;j++)
    {
        auto it = Myconfig::GetInstance()->m_elevatorMap.begin();
        for(int i = 0;i<row_count;i++)
        {
            if(j == 111)
                model->setItem(i,j,new QStandardItem(QString("%1").arg(i)));
            else if(j == 0)
                model->setItem(i,j,new QStandardItem(QString("%1").arg(it.value().elevatorId)));
            else if(j == 1)
                model->setItem(i,j,new QStandardItem(it.value().elevatorType));
            else if(j == 2)
                model->setItem(i,j,new QStandardItem(it.value().elevatorIp));
            else if(j == 3)
                model->setItem(i,j,new QStandardItem(QString("%1").arg(it.value().elevatorPort)));
            else if(j == 4)
                model->setItem(i,j,new QStandardItem(it.value().elevatorNearPos));
            else if(j == 5)
                model->setItem(i,j,new QStandardItem(it.value().elevatorPos));
//            else if(j == 8)
//                model->setItem(i,j,new QStandardItem("在线"));
            it ++;
        }
    }
    addControlForTableView();
}

void ElevatorFrom::onAddClicked()
{
    QStringList list;
    list<<"编号"<<"类型"<<"通讯地址"<<"端口"<<"靠近点位置"<<"位置";
//    AddElevatorForm *elevator = new AddElevatorForm(list,"Insert",this);
//    connect(elevator,&AddElevatorForm::insert_emit,this,&ElevatorFrom::refreshTable);
    CommSetWidget *elevator = new CommSetWidget(1,"Insert",this);
    QRect r = QGuiApplication::primaryScreen()->geometry();
    elevator->move(r.width()/2-elevator->width(),r.height()/2-elevator->height());
    elevator->show();
}

void ElevatorFrom::onOperationClicked()
{
     QPushButton *btn =static_cast<QPushButton*>(sender());
    int row_index = QString(btn->objectName().split("_")[1]).toInt();
    QModelIndex index = model->index(row_index,0);//选中行第一列的内容
    QVariant data = model->data(index);
    int elevator_id = data.toInt();
    ElevatorStatusWidget *s = new ElevatorStatusWidget(elevator_id);
    s->show();
    qDebug()<< elevator_id;

}

void ElevatorFrom::onEditClicked()
{
    QPushButton *btn =static_cast<QPushButton*>(sender());
    QStringList elevatorList;
    int current_row = getClickIndex(btn->objectName());
    QModelIndex modelIndex = model->index(current_row,0);
    QVariant value =  model->data(modelIndex);
    elevatorList << value.toString();
    modelIndex = model->index(current_row,1);
    elevatorList << model->data(modelIndex).toString();
    modelIndex = model->index(current_row,2);
    elevatorList << model->data(modelIndex).toString();
    modelIndex = model->index(current_row,3);
    elevatorList << model->data(modelIndex).toString();
    modelIndex = model->index(current_row,4);
    elevatorList << model->data(modelIndex).toString();
    modelIndex = model->index(current_row,5);
    elevatorList << model->data(modelIndex).toString();
    QStringList labelList;
    labelList<<"编号"<<"类型"<<"通讯地址"<<"端口"<<"靠近点位置"<<"位置";
    AddElevatorForm *elevator = new AddElevatorForm(labelList,"Update",this);
     connect(elevator,&AddElevatorForm::insert_emit,this,&ElevatorFrom::refreshTable);
    elevator->setContent(elevatorList);
    QRect r = QGuiApplication::primaryScreen()->geometry();
    elevator->move(r.width()/2-elevator->width(),r.height()/2-elevator->height());
    elevator->show();
}

void ElevatorFrom::refreshTable()
{
    setTableViewValue();
}

void ElevatorFrom::onDeleteClicked()
{
    int row_index = p_table_view->currentIndex().row();
    QModelIndex index = model->index(row_index,2);//选中行第一列的内容
    QVariant data = model->data(index);
    QString str_ip = data.toString();
    QString sql_str = QString("DELETE FROM t_elevator WHERE ip_address = '%1'").arg(str_ip);
    bool result = CRUDBaseOperation::getInstance()->queryUseStr(sql_str);
    QString key_ip = model->data(model->index(row_index,2)).toString();
    qDebug()<<key_ip;
    if(Myconfig::GetInstance()->m_elevatorMap.contains(key_ip.toInt())&&result)
    {
        Myconfig::GetInstance()->m_elevatorMap.remove(key_ip.toInt());
        model->removeRows(0,model->rowCount());
        setTableViewValue();
    }
    qDebug()<<"crrunt id is:"<<key_ip<< "query result :"<<result;
}

void ElevatorFrom::addControlForTableView()
{
    int row_count = model->rowCount();
    //table row count is up to elevator count
    for (int j = 0; j < row_count; j++)
    {
//        QSpinBox *spinBox = new QSpinBox();
//        spinBox->setMinimum(0);
//        spinBox->setMaximum(12);
//        spinBox->setValue(0);
//        p_table_view->setIndexWidget(model->index(j,4), spinBox);
        p_operation_btn = new QPushButton();
        p_operation_btn->setText("移动");
        p_operation_btn->setFont(QFont("宋体",11));
        p_operation_btn->setIcon(QIcon(":/resouse/Image/up_down_move.png"));
        p_operation_btn->setObjectName(QString("move_%1").arg(j));
        connect(p_operation_btn,&QPushButton::clicked,this,&ElevatorFrom::onOperationClicked);
        p_table_view->setIndexWidget(model->index(j,6), p_operation_btn);
        p_edit_btn = new QPushButton();
        p_edit_btn->setFont(QFont("宋体",11));
        p_edit_btn->setIcon(QIcon(":/resouse/Image/Edit.png"));
        p_edit_btn->setText("修改");
        p_edit_btn->setObjectName(QString("edit_%1").arg(j));
        connect(p_edit_btn,&QPushButton::clicked,this,&ElevatorFrom::onEditClicked);
        p_table_view->setIndexWidget(model->index(j,7),p_edit_btn);
    }
}
