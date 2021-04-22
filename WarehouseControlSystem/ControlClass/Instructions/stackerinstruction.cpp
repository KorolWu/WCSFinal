#include "stackerinstruction.h"
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include "../externcommuincation/tcommtransceivermanager.h"
#define FinishedTIMEOUT 150000
#define ACKTIMEOUT 5000
StackerInstruction::StackerInstruction()
{
    m_result = 0;
    m_id = -1;
    mprefix = Myconfig::GetInstance()->m_SrmWritePrefix;

}
///
/// \brief StackerInstruction::setParameter
/// \param o
/// \param device_id
///
void StackerInstruction::setParameter(OrderStru o, int device_id)
{
    m_order = o;
    m_id = device_id;
    isTerminate = false;
    m_result = 0;
   //qDebug()<<"sourece pos x y z:"<<o.sourcePos.x<<o.sourcePos.y<<o.sourcePos.z << "desnation pos x y z:"<<o.destinationPos.x << o.destinationPos.y<<o.destinationPos.z;
    //根据箱子号查找 源箱子号 和 目标箱子号 两个参数得到对应的数据
}
///
/// \brief StackerInstruction::runInstruction
///
void StackerInstruction::runInstruction()
{
    if(!Myconfig::GetInstance()->m_stackerMap.contains(m_id)) //堆垛机编号判断
    {
        m_result = -1; //参数配置错误
        return ;
    }
    if(!CheckStackerState())
    {
        m_result = -2; //发指令之前的检查
        return ;
    }
    SendCmdTask();//发送数据包指令
    QThread::msleep(5);
    //再写 STB = 1并确认数据收到执行
    int iresult = 0;
    iresult = SendWriteACK();
    if(iresult != 1)
    {
        m_result = iresult; //发送执行任务的数据请求 并判断对方是否收到确认的信息并发送
        return ;
    }
    QThread::msleep(5);
    //执行判断任务是否执行完成
    iresult = CheckTaskisFinished();
    qDebug()<<"iresult = "<<iresult;
    if(iresult != 1)
    {
        m_result = -4; //在任务执行过程中发现任务超时或者出现故障
        return ;
    }
    else{
        qDebug()<<"write Task_finish_ACK = 1;";
        // QThread::msleep(5);
        OrderStru cmd;
        cmd.childtype = 1;
        cmd.Datatype = QOpcUa::Types::Int16;
        QMap<QString,QVariant> valuemap;//变量的名字
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"Task_Finish_ACK\"",1); ////"ns=3;s=\"F_WCS_DB\".\"Task_Finish_ACK\""
        cmd.WcmdstackerValues = valuemap;
        TCommtransceivermanager::GetInstance()->SendcommandByExtern(cmd,m_id);
        m_result = 1;
    }
}

///
/// \brief StackerInstruction::getResult
/// \param exeMsg
/// \return
///

int StackerInstruction::getResult(QString &exeMsg)
{
    if(m_result < 0)
    {
        switch (m_result)
        {
        case -1:
        {
            exeMsg = QString("堆垛机的参数配置错误,硬件的ID查到不到错误的ID:%1").arg(QString::number(m_id));
            return m_result;
        }
        case -2:
        {
            exeMsg = QString("堆垛机运行之前的状态检查，不在线或者有故障或模式不对或状态不对:%1").arg(QString::number(m_id));
            return m_result;
        }
        case -3:
        {
            exeMsg = QString("堆垛机发送任务确认堆垛机接收任务失败:%1").arg(QString::number(m_id));
            return m_result;
        }
        case -4:
        {
            exeMsg = QString("堆垛机执行任务完成信号出现超时或者有故障:%1").arg(QString::number(m_id));
            return m_result;
        }
        case -5:
        {
            exeMsg = QString("堆垛机执行任务完成信号出现超时或者有故障:%1").arg(QString::number(m_id));
            return m_result;
        }
        case -6:
        {
            exeMsg = QString("堆垛机等待发送确认ACK信号中断").arg(QString::number(m_id));
            return m_result;
        }
        default:
        {
            exeMsg = QString("堆垛机未定义设备状态").arg(QString::number(m_id));
            break;
        }
        }
    }
    else {
        exeMsg = QString("堆垛机执行搬运任务成功").arg(QString::number(m_id));
    }
    return  m_result;
}

void StackerInstruction::Run()
{
    QString msg = "";
    runInstruction();
    getResult(msg);
}
///
/// \brief StackerInstruction::CheckStackerState
/// \return
///
bool StackerInstruction::CheckStackerState(qint16 state)
{
    //检查通讯状态
    //检查模式 自动
    //检查状态 空闲
    //检查故障 代码0
    if(Myconfig::GetInstance()->m_stackerMap[m_id].online && \
            Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.mode == 1 &&\
            Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.state == state && \
            Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.Alarm_number == 0 )
    {
        return  true;
    }
    return  false;
}

void StackerInstruction::SendCmdTask()
{
    //组任务指令数据包
    OrderStru cmd;
    cmd.order = Order::Stacker_Carry;
    cmd.childtype = 1;
    cmd.Datatype = QOpcUa::Types::Int16;
    QMap<QString,QVariant> valuemap;//变量的名字
    int16_t srmNo = static_cast<int16_t>(1);//堆垛机编号
    int16_t palletType = 0;//托盘类型
    valuemap.insert("ns=3;s=\"F_WCS_DB\".\"srmNO\"" ,srmNo); //编号     "ns=3;s=\"T_WCS_DB\".\"" + mRstackerVar[i]+"\"";
    valuemap.insert("ns=3;s=\"F_WCS_DB\".\"PalletType\"",palletType);
    switch (m_order.order) {
    case Stacker_Carry:
    {
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"sourcePosX\"",m_order.sourcePos.x);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"sourcePosY\"",m_order.sourcePos.y);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"sourcePosZ\"",m_order.sourcePos.z);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"destinationPosX\"",m_order.destinationPos.x);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"destinationPosY\"",m_order.destinationPos.y);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"destinationPosZ\"",m_order.destinationPos.z);
        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"command\"",StackerCommd::Carray);
        qDebug()<<"#sourece pos x y z:"<<m_order.sourcePos.x<<m_order.sourcePos.y<<m_order.sourcePos.z << "desnation pos x y z:"<<m_order.destinationPos.x << m_order.destinationPos.y<<m_order.destinationPos.z;

        break;
    }
    default:
        break;
    }
    cmd.WcmdstackerValues = valuemap;
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(cmd,m_id);
}
///
/// \brief StackerInstruction::SendWriteACK
/// \return
///
int StackerInstruction::SendWriteACK()
{
    int writeack = 0;
    //组任务指令数据包
    OrderStru cmd;
    cmd.childtype = 1;
    cmd.Datatype = QOpcUa::Types::Int16;
    QMap<QString,QVariant> valuemap;//变量的名字
    valuemap.insert("ns=3;s=\"F_WCS_DB\".\"stb\"",1); //"ns=3;s=\"F_WCS_DB\".\"stb\""
    cmd.WcmdstackerValues = valuemap;
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(cmd,m_id);
    QThread::msleep(5);
    //等待ACK变为1
    int checkresult = CheckACkSignal(1, CHECKVAR::ACK);
    if(checkresult == 1)
    {
        qDebug()<<"check ack = 1";
        //发送任务已经确认接收
        cmd.WcmdstackerValues["ns=3;s=\"F_WCS_DB\".\"stb\""] = 0;
        TCommtransceivermanager::GetInstance()->SendcommandByExtern(cmd,m_id);
        writeack = 1;
//        checkresult = CheckACkSignal(0, CHECKVAR::ACK);
//        if(checkresult == 1)
//        {
//            writeack  = 1;
//        }
//        else{
//            writeack = -2;//超时或者退出了
//        }
    }
    else{
        writeack = -6;//超时或者退出了
    }
    //等待ACK变为false
    return  writeack;
}
///
/// \brief StackerInstruction::CheckTaskisFinished
/// \return
///
int StackerInstruction::CheckTaskisFinished()
{
    int reslut = 0;
    struct timeval tpStart,tpEnd;
    int64_t timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    while (true)
    {
        if(!Myconfig::GetInstance()->m_exit || isTerminate)
        {
            reslut = -99;
            break ;
        }
        if(Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.state == 99)
        {
            reslut = -10;
            break ;
        }
        if(Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.taskFinish == 1)
        {
            reslut = 1;
            qDebug()<<"taskFinish signal checked!!";
            break;
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= FinishedTIMEOUT)
        {
            break;
        }
        QThread::msleep(5);
        QApplication::processEvents();
    }
    qDebug()<<"wait taskFinish result ="<<reslut;
    return  reslut;
}
///
/// \brief StackerInstruction::CheckACkSignal
/// \param expectvalue
/// \param role
/// \return
///
int  StackerInstruction::CheckACkSignal(qint16  expectvalue, int role )
{
    int reslut = 0;
    struct timeval tpStart,tpEnd;
    int64_t timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    while (true)
    {
        if(!Myconfig::GetInstance()->m_exit|| isTerminate)
        {
            reslut = -99;
            break ;
        }
        switch (role) {
        case CHECKVAR::ACK:
        {
            if(Myconfig::GetInstance()->m_stackerMap[m_id].Stacker_r.ack == expectvalue)
            {
                reslut = 1;
                return  reslut;
            }
            break;
        }
        default:
            break;
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= ACKTIMEOUT)
        {
            break;
        }
        QThread::msleep(5);
        QApplication::processEvents();
    }
    return  reslut;
}
