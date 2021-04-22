#include "commsetwidget.h"

CommSetWidget::CommSetWidget(int index, QString operation, QWidget *parent) : QWidget(parent)
{
    this->resize(550,410);
    m_index = index;
    m_operation = operation;
    m_pComContainer = new QWidget(this);
    QLabel *l = new QLabel("请选择通讯类型:",m_pComContainer);
    l->move(10,5);
    m_pSelectTypeCombox = new QComboBox(m_pComContainer);
    m_pSelectTypeCombox->move(130,5);
    m_pComContainer->setObjectName("CommWidgetContainer");//border-image:url(:/resouse/Image/grey2.png)
    m_pComContainer->setStyleSheet("#CommWidgetContainer{background-color:rgb(74, 112, 139);border-radius: 15px 15px}""QLabel{background:transparent;color:white}");
    QStringList comboxList;
    comboxList<<"Tcp/IP"<<"ModbusTcp"<<"ModbusRtu"<<"RS232";
    m_pSelectTypeCombox->addItems(comboxList);

    CommunicationFactory();
    if(m_pAbstrcutCommunication != nullptr)
        m_pAbstrcutCommunication->move(10,35);

    //index change signal
    connect(m_pSelectTypeCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [=](int index){
        if(m_pAbstrcutCommunication == nullptr)
            return;
        m_index = index;
        m_operation = "Insert";
        m_pAbstrcutCommunication->close();
        CommunicationFactory();
        m_pAbstrcutCommunication->move(10,35);
        m_pAbstrcutCommunication->show();
        connect(m_pAbstrcutCommunication,&DialogAbstractClass::OnWidgetClose,[=](){
            this->hide();
            this->deleteLater();
        });
        this->update();
    });
    connect(m_pAbstrcutCommunication,&DialogAbstractClass::OnWidgetClose,[=](){
        this->hide();
        this->deleteLater();
    });
}

DialogAbstractClass *CommSetWidget::CommunicationFactory()
{
    qDebug()<<"index change"<<m_index;
    QStringList list;
    list<<"编号"<<"类型"<<"通讯地址"<<"端口"<<"靠近点位置"<<"位置";
    QStringList labelList;
    labelList<<"设备编号"<<"设备IP"<<"端口"<<"设备类型"<<"说明";
    m_pAbstrcutCommunication = nullptr;
    switch (m_index) {
    case 0:
        m_pAbstrcutCommunication = new AddCar(labelList,m_operation,m_pComContainer);
        break;
    case 1:
        m_pAbstrcutCommunication = new AddElevatorForm(list,m_operation,m_pComContainer);
        qDebug()<<"init elevatorWidget";
        break;
    default:
        m_pAbstrcutCommunication = nullptr;
        break;
    }
    return m_pAbstrcutCommunication;
}

void CommSetWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_press){
        move(event->globalPos() - mousePoint);
    }
}

void CommSetWidget::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) ){
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();
        //        event->accept();
    }
    else if(event->button() == Qt::RightButton){
        //如果是右键
        this->close();

    }
}

void CommSetWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mouse_press = false;
}

