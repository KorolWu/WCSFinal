#include "excrunneroutcmd.h"
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"
#include <QMutexLocker>
#include <QApplication>
#include <ctime>
#include <time.h>
#include <sys/time.h>

ExcRunnerOutCmd::ExcRunnerOutCmd()
{
    IsStop = false;
    m_id = 9;
    m_changeD6 = false;
    m_changeD8 = false;
    m_scanid = -1;
    for(int i = 1; i<= 8;++i)
    {
        Myconfig::GetInstance()->m_cacheRunerMap.insert(i,false);
    }
}

void ExcRunnerOutCmd::SetExcRunnerParam(int id,int scanid)
{
    m_id = id;
    m_scanid = scanid;
}

void ExcRunnerOutCmd::ExcRun()
{
    while (!IsStop)
    {
        if(!Myconfig::GetInstance()->m_exit)
            break;
        ExcRunnerForOutNew();
        ExcRunnerForInNew();
        QApplication::processEvents();
        QThread::msleep(100);
    }
}

void ExcRunnerOutCmd::StopRun()
{
    IsStop = true;
}


void ExcRunnerOutCmd::ExcRunnerForOutNew()
{
    //QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex); //1/30没有涉及到写入操作，暂时屏蔽该锁
    if(Myconfig::GetInstance()->m_RunnerMap[m_id].runneratastru.holdresMap.contains(6)) //判断流道D6的值 ，如果D6==99 则执行扫码
    {
        if(Myconfig::GetInstance()->m_RunnerMap[m_id].runneratastru.holdresMap[6] == 99)
        {
            //1.WCS触发出库扫码
            if(!m_changeD6)
            {
                if(!ScancodeCheck())
                {
                    qDebug()<<"出库扫码失败 :"<< QDateTime::currentDateTime().toString();
                    return;
                }
                //2.检测流道缓存区位的状态，分配缓存位置发给流道 1-8个位置
                for(auto it = Myconfig::GetInstance()->m_cacheRunerMap.begin();it!= Myconfig::GetInstance()->m_cacheRunerMap.end();++it)
                {
                    if(it.key() == 8)
                        continue;
                    if(!it.value()) //无料箱即可分配
                    {
                        //发送给流道 D6赋值
                        OrderStru runnerstru;
                        runnerstru.childtype = 2;
                        runnerstru.Datatype = 168; //寄存器 读写
                        runnerstru.startaddress = 6;
                        runnerstru.numberOfEntries = 1;
                        runnerstru.values.append(it.key());
                        //qDebug()<<"查找到缓存区无料的位置值："<<it.key()<<"流道ID："<<m_id;
                        TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,m_id);//给出流道缓存区位置
                        UpdateTempPos(it.key(),Myconfig::GetInstance()->m_RunnerMap[m_id].Outcodeinfo);
                        m_changeD6 = true;
                        break;
                    }
                }
            }
        }
        else{
            if(m_changeD6)
            {
                m_changeD6 = false;
            }
        }
    }
}
///
/// \brief ExcRunnerOutCmd::ExcRunnerForInNew
///
void ExcRunnerOutCmd::ExcRunnerForInNew()
{
    //QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex);
    if(Myconfig::GetInstance()->m_RunnerMap[m_id].runneratastru.holdresMap.contains(8)) //判断流道D6的值 ，如果D8==99 则执行扫码
    {
        if(Myconfig::GetInstance()->m_RunnerMap[m_id].runneratastru.holdresMap[8] == 99 && \
                Myconfig::GetInstance()->m_RunnerMap[m_id].runneratastru.holdresMap[12] != 99)
        {
            if(m_time != 0) // not scan in 8s
            {
                if(QDateTime::currentDateTime().toMSecsSinceEpoch() - m_time < 10000)
                {
                    return;
                }
            }
            if(!m_changeD8)
            {

                 m_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
                 if(ScancodeCheck(false))
                 {
                     //发送给流道 D0赋值
                     ExcRunnerPass();
                 }
//                //1.WCS触发入库扫码
//                qDebug()<<"beging scan :"<<QDateTime::currentDateTime().toString();
//                ExcScanCmd();
//                //2.在2s之内等待扫码的结果
//                int iresult= CheckScanisOK();
//                qDebug()<<"end scan :"<<QDateTime::currentDateTime().toString();
//                if(iresult != 1)
//                {
//                    qDebug()<<"扫码失败："<<iresult;
//                }
//                else
//                {
//                    //发送给流道 D0赋值
//                    ExcRunnerPass();
//                }
                m_changeD8 = true;
            }
        }
        else{
            if(m_changeD8)
            {
                m_changeD8 = false;
            }
        }
    }
}

//void ExcRunnerOutCmd::ExcScanCmd()
//{
//    //1.WCS触发入库扫码
//    if(Myconfig::GetInstance()->m_ScanMap.contains(m_scanid))
//    {
//        OrderStru scanstru;
//        scanstru.order = Scan_Code;
//        TCommtransceivermanager::GetInstance()->SendcommandByExtern(scanstru,m_scanid);//扫码
//    }
//}

void ExcRunnerOutCmd::ExcScanInstruction(int device_id)
{
    //1.WCS触发扫码
    if(Myconfig::GetInstance()->m_ScanMap.contains(device_id))
    {
        OrderStru scanstru;
        scanstru.order = Scan_Code;
        TCommtransceivermanager::GetInstance()->SendcommandByExtern(scanstru,device_id);//扫码
        qDebug()<<"scan code id:"<<device_id;
    }
}
///
/// \brief ExcRunnerOutCmd::CheckScanisOK
/// \return
///
int ExcRunnerOutCmd::CheckScanisOK()
{
    int result= -1;
    struct timeval tpStart,tpEnd;
    double timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    while (true)
    {
        if(!Myconfig::GetInstance()->m_exit)
        {
            break;
        }
        if( Myconfig::GetInstance()->m_RunnerMap[m_id].Inscanok)
        {
            result = 1;
            qDebug()<<"入库扫码确认"<<Myconfig::GetInstance()->m_RunnerMap[m_id].Incodeinfo;
            QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
            Myconfig::GetInstance()->m_RunnerMap[m_id].Inscanok = false;
            break;
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= 1000)
        {
            result = -1;
            qDebug()<<"等待入库扫码信息超时1S"<<QThread::currentThreadId();
            break;
        }
        QThread::msleep(10);
        QApplication::processEvents();

    }
    return  result;
}

void ExcRunnerOutCmd::ExcRunnerPass()
{
    OrderStru runnerstru;
    runnerstru.childtype = 2;
    runnerstru.Datatype = 168; //寄存器 读写
    runnerstru.startaddress = 8;
    runnerstru.numberOfEntries = 1;
    runnerstru.values.append(0);
    if(!m_changeD8)
    {
        qDebug()<<"发送扫码完成的值 0"<<"流道ID："<<m_id;
        TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,m_id);//给出流道缓存区位置
        m_changeD8 = true;

    }
}

void ExcRunnerOutCmd::UpdateTempPos(int pos, QString codeinfo)
{
    QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex);
    //更新箱子码与暂存位置对应关系
    Myconfig::GetInstance()->m_RunnerMap[m_id].m_RunnerBoxCodeInfoMap.insert(pos,codeinfo);
    qDebug()<<"更新缓存位置:"<<pos <<"codeinfo："<<codeinfo;
}
///
/// \brief ExcRunnerOutCmd::CheckOutScanisOK
/// \return
///
int ExcRunnerOutCmd::CheckOutScanisOK()
{
    int result= -1;
    struct timeval tpStart,tpEnd;
    double timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    while (true)
    {
        if(!Myconfig::GetInstance()->m_exit)
        {
            break;
        }
        if( Myconfig::GetInstance()->m_RunnerMap[m_id].m_outscanok)
        {
            result = 1;
            qDebug()<<"扫码确认"<<Myconfig::GetInstance()->m_RunnerMap[m_id].Outcodeinfo;
            QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
            Myconfig::GetInstance()->m_RunnerMap[m_id].m_outscanok = false;
            break;
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= 1000)
        {
            result = -1;
            qDebug()<<"等待扫码信息超时1S"<<QThread::currentThreadId();
            break;
        }
        QThread::msleep(10);
        QApplication::processEvents();

    }
    return  result;
}

bool ExcRunnerOutCmd::ScancodeCheck(bool isout )
{
    bool isok = false;
    for(int i = 0; i < 5; ++i)
    {
        if(isout)
        {
             ExcScanInstruction(41);
            if(CheckOutScanisOK() == 1)
            {
                isok = true;
                break ;
            }
        }
        else{
            ExcScanInstruction(m_scanid);
            if(CheckScanisOK() == 1)
            {
                isok = true;
                break ;
            }
        }
    }
    return  isok;
}
