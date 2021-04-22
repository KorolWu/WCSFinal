#include "carelevatorinstruction.h"
#define TIMEOUT 15000
CarElevatorInstruction::CarElevatorInstruction()
{
    m_result = 0;
}

void CarElevatorInstruction::setParameter(OrderStru o, int device_id)
{
    m_id = device_id;
    m_order = o;
    m_order.Datatype = 4;  //varint type
    m_order.childtype = 2; //write or read 5=read other is write
    QVector<int> value;
    value.append(o.z);
    m_z = o.z;
    m_order.values = value;

}

void CarElevatorInstruction::runInstruction()
{

    //date is not in layer fanwei
    if(m_order.z > 8 || m_order.z < 1)
    {
        m_result = -21;
        return;
    }
    //to check car status status 0--->1
    //    if(Myconfig::GetInstance()->m_elevatorMap[m_id].status.isLock == false && Myconfig::GetInstance()->m_elevatorMap[m_id].status.isOnline == true)
    //    {

    Myconfig::GetInstance()->m_elevatorMap[m_id].status.isLock = true;
    //emit order to interface
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(m_order,m_id);
    qDebug()<<"layer"<<m_z<<"id"<<m_id<<QDateTime::currentDateTime().toString();
    m_result = 0;
    return ;
    //    }
    //    else
    //        m_result = 99;
}

int CarElevatorInstruction::getResult(QString &exeMsg)
{
    if(m_result != 0)
    {
        exeMsg = " exec faild";
        Myconfig::GetInstance()->m_elevatorMap[m_id].status.isLock = true;
        qDebug()<<"elevatro instruction finished by excpetion";
        return m_result;
    }
    struct timeval tpStart,tpEnd;
    int64_t timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    while (timeUse < TIMEOUT)
    {
        if(!Myconfig::GetInstance()->m_exit)
        {
            m_result = -99;
            break ;
        }
        if(m_order.startaddress == 50)
        {
            if(Myconfig::GetInstance()->m_elevatorMap[m_id].status.curruntLayer == m_z)
            {
                m_result = 0;
                qDebug()<<"到达目标层"<<m_z;
                break;
            }
        }
        else if(m_order.startaddress == 52)
        {
            if(Myconfig::GetInstance()->m_elevatorMap[m_id].status.curachelayer == m_z)
            {
                m_result = 0;
                qDebug()<<"到达目标层"<<m_z;
                break;
            }
        }

        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= TIMEOUT)
        {
            m_result = -11;
            break;
        }
        QThread::msleep(5);
        QApplication::processEvents();
    }

    if(m_result == -99)
    {
        exeMsg = QString("%1  Terminate!").arg(m_order.type);
    }
    else if(m_result == -11)
    {
        exeMsg = " exec instruction Timeout!";
    }
    //qDebug()<<"elevatro instruct finished"<< m_result<<QDateTime::currentDateTime().toString()<<"elevator current layer:"<<Myconfig::GetInstance()->m_elevatorMap[m_id].status.curruntLayer;
    return m_result;
}

