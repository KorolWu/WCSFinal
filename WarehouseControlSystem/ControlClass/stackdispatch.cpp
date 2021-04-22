#include "stackdispatch.h"

StackDispatch::StackDispatch(QObject *parent) : QObject(parent)
{

}
///
/// \brief StackDispatch::dispatchTaskThread
///优先出库的原则，一楼的优先级大于二楼的优先级，条件允许一直出库
/// 有两个站点，一楼可入可出，二楼只可以出
///  Myconfig::GetInstance()->m_RunnerMap.size() = 2;
///
void StackDispatch::dispatchTaskThread()
{
    int stationNum = 999;//站号
    while(Myconfig::GetInstance()->m_exit) //退出标志位
    {
        if(Myconfig::GetInstance()->m_flag == true && Myconfig::GetInstance()->m_exit)//暂停标志位
        {
            for (auto it = Myconfig::GetInstance()->m_RunnerMap.begin();it != Myconfig::GetInstance()->m_RunnerMap.end();it++)
            {
                stationNum = it.key();
                if(Myconfig::GetInstance()->m_IntaskMap.contains(stationNum) || Myconfig::GetInstance()->m_OuttaskMap.contains(stationNum))
                {    //执行入库任务 [入库任务数小于流道可存放的数量或者没有出库任务并且缓存区可取 的情况下]
                    if(Myconfig::GetInstance()->m_RunnerMap[stationNum].bitstate[0].bbit0 == 1 && Myconfig::GetInstance()->m_IntaskMap.contains(stationNum) && (Myconfig::GetInstance()->m_IntaskMap[stationNum].size() >= MAX_OUT_CAHCHE || Myconfig::GetInstance()->m_OuttaskMap[stationNum].size() == 0))
                    {
                        //这里不考虑堆垛机的状态是否可用，只有一台，任务没有完成前会阻塞指令里面
                        // scanCode();
                        if(false == Myconfig::GetInstance()->m_IntaskMap[stationNum].isEmpty())
                        {
                            QString boxNum = Myconfig::GetInstance()->m_RunnerMap[stationNum].Incodeinfo;
                            if(boxNum != "" && Myconfig::GetInstance()->m_IntaskMap[stationNum].contains(boxNum))
                            {
                                Myconfig::GetInstance()->m_RunnerMap[stationNum].Incodeinfo = "";
                                if(Myconfig::GetInstance()->m_IntaskMap[stationNum].contains(boxNum))
                                {
                                    TaskInfoStru t = Myconfig::GetInstance()->m_IntaskMap[stationNum][boxNum];
                                    //handle_in_task 确保在扫到码的情况下，正确取出任务，交给堆垛机
                                    handleTask(t,stationNum);
                                    qDebug()<<"正在处理入库任务";
                                }
                            }
                            else
                            {
                                delayMSec(40);
                                continue;
                            }
                        }

                        delayMSec(50);
                    }
                    else //执行出库任务
                    {
                        //流道可放 站点合法 出库任务不为空
                        if(Myconfig::GetInstance()->m_RunnerMap[stationNum].bitstate[0].bbit1 == 1 && Myconfig::GetInstance()->m_OuttaskMap.contains(stationNum) && false == Myconfig::GetInstance()->m_OuttaskMap[stationNum].isEmpty())
                        {

                            QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
                            m_task =  Myconfig::GetInstance()->m_OuttaskMap[stationNum].at(0);
                            //handle_out_task(m_task,stationNum);
                            handleTask(m_task,stationNum);
                            delayMSec(100);
                        }
                    }
                }
            }
        }
        delayMSec(50);
    }
}

void StackDispatch::delayMSec(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void StackDispatch::handleTask(TaskInfoStru &task,int station)
{
    OrderStru order;
    bool IsExist = false;
    //出库有两个站点 wms传过来的站点只能是INT类型
    if(task.taskMainType == "出库")
    {
        if(task.shelves == IN_OUT_STATION)//目标库位 = layer 1
        {
            order.destinationPos = LAYER_PUTDOWN_1;
        }
        else if(task.shelves == OUT_STATION) // layer = 2
        {
            order.destinationPos = LAYER_PUTDOWN_2;
        }
        remove_out_task(station);
        KPosition start;
        //由货架号得出该栈板的坐标
      IsExist = StoreInfo::BaseDataInfoOperate::GetKposInfoByIDnbr(task.fromLocationCode,start);//fromLocationCode 起始库位为货架号
      order.sourcePos = start;

    }
    else if(task.taskMainType == "入库")
    {
         order.sourcePos = LAYER_PICKUP_1;
         KPosition end;
         IsExist = StoreInfo::BaseDataInfoOperate::GetKposInfoByIDnbr(task.shelves,end);//fromLocationCode 目标库位为货架号
         order.destinationPos = end;
         remove_in_task(task.taskNum,station);
         //判断入库只能是由1号站点发送过来的
    }
    if(false == IsExist)
    {
        //false function();
        QString errorMsg = "仓库没有该货架号:" + task.shelves;
        writeFailedLog(errorMsg,1,task.boxNum,task.regCode);
        return;
    }
    StackerInstruction *instruction = new StackerInstruction();
    instruction->setParameter(order,M_ID);
    instruction->runInstruction();
    QString resultMsg = "";
    int v = instruction->getResult(resultMsg);
    if(v >= 0)
    {
        //正常执行完成 需要根据出库还是入库指令来区分是否要给流道指令
        task.status = "已完成";

    }
    else {
        //执行失败
         task.status = "执行失败";
        writeFailedLog(resultMsg,v,task.boxNum,task.regCode);
        Myconfig::GetInstance()->m_flag = false;//停止循环调度
    }
    QString err = "";
    if(!CRUDBaseOperation::getInstance()->saveCompletedTask(m_task,err))
        GetSystemLogObj()->writeLog("完成任务保存失败!-->"+err,2);

}

int StackDispatch::errorCodeChange(const int errCode)
{
    int result = -1;
    switch (errCode) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
    case 14:
    case 15:
        result = 1;
        break;
    case 5:
        result = 2;
        break;
    case 13:
        result = 3;
        break;
    default:
        result = 7;
        break;
    }
    return result;
}

void StackDispatch::remove_out_task(int station)
{
    Myconfig::GetInstance()->m_OuttaskMap[station].dequeue();
}

void StackDispatch::remove_in_task(const QString &frist_in_boxNum, int station)
{
    qDebug()<<" remove box_number<<"<<frist_in_boxNum<<"station "<<station;
    if(Myconfig::GetInstance()->m_IntaskMap[station].contains(frist_in_boxNum))
    {
        QMutexLocker locker(&Myconfig::GetInstance()->m_in_task_mutex);
        Myconfig::GetInstance()->m_IntaskMap[station].remove(frist_in_boxNum);
        qDebug()<<"remove it.key";
    }
}

void StackDispatch::writeFailedLog(const QString &message, const int errCode, const QString boxNum, QString taskId)
{
    ALARMINFOSTRU arm;
    arm.alarminfo = message;
    arm.alarmlevel = 4;
    arm.boxnumber = boxNum;
    arm.carcoordx = 0;
    arm.carcoordy = 0;
    arm.carcoordz = 0;
    arm.deviceid = QString("%1").arg(M_ID);
    arm.errorcode = errorCodeChange(errCode);
    arm.errortype = 1;
    arm.Operatestate = 1;
    arm.wmsTaskid = taskId;
    QString sql_err;
    if(false == m_writeData.WriteAlarmInfo(arm,sql_err))
        GetSystemLogObj()->writeLog("在执行子任务时保存报错信息失败！message->"+message,2);
}
