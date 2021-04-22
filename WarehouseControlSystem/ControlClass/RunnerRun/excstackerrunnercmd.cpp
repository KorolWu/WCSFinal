#include "excstackerrunnercmd.h"
#include <QMutexLocker>
#include <QApplication>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <QThread>
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"

ExcStackerRunnerCmd::ExcStackerRunnerCmd()
{
  m_changed = false;
  IsStop = false;
}
///
/// \brief ExcStackerRunnerCmd::SetExcRunnerParam
/// \param runnerid
/// \param scanid
///
void ExcStackerRunnerCmd::SetExcRunnerParam(int runnerid, int scanid)
{
    m_runnerid = runnerid;
    m_scanid = scanid;
}

void ExcStackerRunnerCmd::ExcRunning()
{
    while (!IsStop)
    {
        if(!Myconfig::GetInstance()->m_exit)
            break;
        ExcInRequest();
        QApplication::processEvents();
        QThread::msleep(100);
    }
}
///
/// \brief ExcStackerRunnerCmd::StopRun
///
void ExcStackerRunnerCmd::StopRun()
{
     IsStop = true;
}
///
/// \brief ExcStackerRunnerCmd::ExcInRequest
///
void ExcStackerRunnerCmd::ExcInRequest()
{
    if(Myconfig::GetInstance()->m_RunnerMap.contains(m_runnerid))
    {
          //1.判断是否为扫码请求的状态
        if(Myconfig::GetInstance()->m_RunnerMap[m_runnerid].bitstate[0].bbit2 != 0)
        {
            if(m_changed)
                return;
            //2.给扫码枪发送入库扫码请求并判断是否在指定时间之内收到扫码指令
            if(ScancodeCheck(m_scanid))
            {
                 //3.发送入库扫码完成指令给流道
                ExcRunnerCmd(m_runnerid,Runner_Scancode2_In);
                m_changed = true;
            }
        }
        else{
            if(m_changed)
            {
                m_changed = false;
            }
        }
    }
}
///
/// \brief ExcStackerRunnerCmd::ExcScanInstruction
/// \param device_id
///
void ExcStackerRunnerCmd::ExcScanInstruction(int device_id)
{
    //1.WCS触发扫码
    if(Myconfig::GetInstance()->m_ScanMap.contains(device_id))
    {
        OrderStru scanstru;
        scanstru.order = Scan_Code;
        TCommtransceivermanager::GetInstance()->SendcommandByExtern(scanstru,device_id);//扫码
    }
}
///
/// \brief ExcStackerRunnerCmd::CheckScanIsok
/// \param scanid
/// \param inflag
/// \return
///
int ExcStackerRunnerCmd::CheckScanIsok(int scanid, bool inflag)
{
    Q_UNUSED(inflag)
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
        //检查流道上的扫码枪是否收到码
        if(Myconfig::GetInstance()->m_RunnerMap.contains(scanid))
        {
            if(Myconfig::GetInstance()->m_RunnerMap[scanid].Inscanok)
            {
                result = 1;
                QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
                Myconfig::GetInstance()->m_RunnerMap[scanid].Inscanok = false;
                qDebug()<<" 接收到扫码id["<<scanid<<"]:"<<Myconfig::GetInstance()->m_RunnerMap[scanid].Inscanok;
                break;
            }
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= 1000)
        {
            result = -1;
            break;
        }
        QThread::msleep(10);
        QApplication::processEvents();

    }
    return  result;
}
///
/// \brief ExcStackerRunnerCmd::ScancodeCheck
/// \param scanid
/// \return
///
bool ExcStackerRunnerCmd::ScancodeCheck(int scanid)
{
    bool isok = false;
    for(int i = 0; i < 5; ++i)
    {
        ExcScanInstruction(scanid);
        if(CheckScanIsok(m_runnerid))
        {
            isok = true;
            break ;
        }
    }
    return  isok;
}

void ExcStackerRunnerCmd::ExcRunnerCmd(int device_id, int cmdname)
{
    OrderStru runnerstru;
    runnerstru.childtype = 6;
    runnerstru.order = static_cast<Order>(cmdname);
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,device_id);
}
