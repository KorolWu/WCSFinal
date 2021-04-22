#include "kdispatch.h"

KDispatch::KDispatch(KPosition task_P, QString &ip, int carId, const TaskInfoStru task)
{
    qRegisterMetaType<OrderStru>("OrderStru");
    this->m_task_p = task_P;
    this->m_ip = ip;
    this->m_task = task;
    this->m_carId = carId;
    //test
    m_pAbstructInstruction = nullptr;
    initRedis();
}

KDispatch::~KDispatch()
{

}

/// \brief KDispatch::saveSubTaskInfo
/// \return
/// 将轨迹产生的子任务保存到数据库里面
bool KDispatch::saveSubTaskInfo(QQueue<OrderStru> taskQueue)
{
    QQueue<OrderStru> sub_task;
    sub_task = taskQueue;
    QStringList field;

    field<<"taskNum"<<"taskType"<<"subTaskNum"<<"sequence" << "state"<<"storeNum"<<"deviceIp"<<"commandData";
    QString taskType = "";
    int sequence = 1;
    if(m_task.taskMainType == "出库")
        taskType = "出库";
    else if(m_task.taskMainType == "入库")
        taskType = "入库";
    else
        taskType = "充电";

    QList<QVariantList> values;
    while(sub_task.isEmpty() == false)
    {
        OrderStru o = sub_task.dequeue();
        QVariantList value;
        value<<m_task.taskNum<<taskType<<transformationOrder(o.order)<<QString::number(sequence, 10)<<"未执行"<<m_task.boxNum<<QString("%1").arg(m_carId)<<"commandDate";
        values.append(value);
        sequence ++;
    }
    QString errorInfo;
    if(CRUDBaseOperation::getInstance()->ExcBatchInsertDb("t_sub_taskInfo",field,values,errorInfo))
    {
        return true;
    }
    else{
        qDebug()<<"errorinfo:"<<errorInfo<<endl;
        return false;
    }

}
///
/// \brief KDispatch::reply2WMS  任务完成后向wms反馈任务执行结果
/// \param task 当前正在执行的任务
/// \param result 任务是否执行成功
///
void KDispatch::reply2WMS(const TaskInfoStru &task,const bool &result)
{
    int interfaceType = 0;
    QString nowLocation = "";
    QString errLocation = "";//异常发生库位
    QString errMsg = "";
    if(result)
    {
        if(task.taskMainType == "出库")
        {
            interfaceType = 500;
            nowLocation = "9";
            errMsg = "OK";
        }
        else if(task.taskMainType == "入库")
        {
            interfaceType = 900;
            nowLocation = task.shelves;
            errMsg = "OK";
        }
    }
    else {
        interfaceType = 300;//异常统一接口代码
        errLocation = task.shelves;
        errMsg = "WCS Execution exception!";
    }
    QByteArray reply = makeBody(task,interfaceType,nowLocation,result,errMsg);
    HttpClien_k *t = new HttpClien_k();
    OrderStru postParameter;
    postParameter.http.body = reply;
    if(m_task.taskMainType == "出库")
        postParameter.http.url = Myconfig::GetInstance()->outOfWarehouseUrl;
    else
        postParameter.http.url = Myconfig::GetInstance()->inOfWareHouseUrl;
    t->setParameter(postParameter,0);
    t->runInstruction();
    QString errmsg;
    t->getResult(errmsg);
}
//how to know devicesId
bool KDispatch::runSubTask()
{
    //qDebug()<<QThread::currentThreadId();
    int sequnce = 1;
    QString msg = "";
    QString sqlerr = "";
    bool result = false;
    while(false == m_taskQueue.isEmpty() )
    {
        if(!Myconfig::GetInstance()->m_flag)
        {
            break; // 检测到急停信号后跳出执行队列，然后返回false；记录该条任务是由急停结束的
        }
        OrderStru o = m_taskQueue.dequeue();
        //        if(o.order == 9 || o.order ==10)//为了测试，取货时屏蔽放料想的动作
        //            continue;
        result= runInstrucation(o);
        if(result == false || o.order == Order::Right_Putinto || o.order == Order::Left_Putinto)//取料箱执行失败
        {
            QByteArray reply("empty");
            if(o.order == Order::Left_Pickup || o.order == Order::Right_Pickup)
            {
                reply = makeBody(m_task,300,m_task.shelves,result,"取料箱执行失败");
            }
            else if(o.order == Order::Left_Putinto || o.order == Order::Right_Putinto)
            {
                if(result == false)
                    reply = makeBody(m_task,800,"rgv",result,"放料箱执行失败");
                else
                    reply = makeBody(m_task,900,m_task.shelves,result,"入库成功");
            }
            else if(o.order == Order::Right_WorkBin)//把料箱从入库口电梯取出
            {
                reply = makeBody(m_task,700,"流道入库终止位",result,"取料箱执行失败");
            }
            else if(o.order == Order::Left_WorkBin) //把料箱放在出货口电梯
            {
                reply = makeBody(m_task,400,"rgv",result,"放料箱执行失败");
            }
            if(reply != "empty")
            {
                //                HttpClien_k *t = new HttpClien_k();
                //                OrderStru postParameter;
                //                postParameter.http.body = reply;
                //                if(m_task.taskMainType == "出库")
                //                    postParameter.http.url = Myconfig::GetInstance()->outOfWarehouseUrl;
                //                else
                //                    postParameter.http.url = Myconfig::GetInstance()->inOfWareHouseUrl;
                //                t->setParameter(postParameter,0);
                //                t->runInstruction();
                //                QString errmsg;
                //                t->getResult(errmsg);
            }
        }
        //        for(int i = 0; i < 10 && Myconfig::GetInstance()->m_flag; ++i)
        //        {
        //             QThread::msleep(200);
        //        }
        result?msg = "执行成功":msg = "执行失败";
        if(!CRUDBaseOperation::getInstance()->changeSubtaskStatus(m_task.taskNum,msg,QString("%1").arg(o.value),sequnce,sqlerr))
            GetSystemLogObj()->writeLog("change substatus to dbbase failed! ->"+sqlerr,2);
        sequnce++;
        if(result == false) //一条失败整个任务退出,跳出队列，释放小车资源
            break;
        else
        {
            if(o.order == 9 || o.order == 10)
            {
                OrderStru r;
                r.order = call_Runner_Putbox;
                r.Datatype = 168;//A8 D型寄存器的的代号
                r.childtype = 2;
                AbstructInstruction *e = new RunnerInstruction();
                r.value = 0;   //value
                if(o.order == 9)
                    r.startaddress = 10;  //start address
                else
                    r.startaddress = 12;  //start address
                e->setParameter(r,9);
                e->runInstruction();
                QString resultMsg ="";
                e->getResult(resultMsg);
                qDebug()<<"流道取放货复位指令"<<resultMsg;
            }
        }
        Delay_MSec(800);
    }
    QMutexLocker locker(&Myconfig::GetInstance()->m_carMap_mutex);
    //    Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.isLocking = false;//让小车和它正在执行的任务解绑，让小车可以接受下一个任务
    qDebug()<<m_task.taskNum<<"任务执行结束"<<"result : "<<result;
    //delete crrunt task
    if(result)
    {
        Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.status = 3; //3=执行完成
        return true;
    }
    else
    {
        Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.status = 2; //2=异常 此异常反馈到界面，当出现异常时需要手动解除
        return false;
    }
}

void KDispatch::initRedis()
{
    m_redis = new RedisBaseModule(Myconfig::GetInstance()->m_redisInfo.ip.toStdString().c_str(),Myconfig::GetInstance()->m_redisInfo.port);
    m_redisIsConnect = m_redis->getConnect();
}

bool KDispatch::removeOldTask(QString regCode)
{
    if(m_redis)
    {
        QString commd = QString("DEL %1").arg(regCode);
        redisReply *reply = m_redis->execommand(commd);
        if(reply != nullptr && reply->type == REDIS_REPLY_INTEGER)
        {
            if(reply->integer == 1)
            {
                qDebug()<<regCode<<"removed!";
                return true;
            }
            else {
                qDebug()<<"task "<<regCode<<"not exist!";
            }
        }
    }
    qDebug()<<regCode<<"remove failed!";
    return false;
}
void KDispatch::Delay_MSec(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
QString KDispatch::transformationOrder(int i)
{
    if(i == 0)
        return "Y方向横移车辆";
    else if(i == 1)
        return "X方向竖移车辆";
    else if(i == 3)
        return "左取货";
    else if(i == 4)
        return " 右取货";
    else if(i == 5)
        return "呼叫电梯";
    else if(i == 7)
        return "Y方向进电梯";
    else if(i == 8)
        return "Y方向出电梯";
    else if(i == 9)
        return "把料箱放在出货口电梯";
    else if(i == 10)
        return "把料箱从入库口电梯取出";
    else if(i == 14)
        return "将箱子放到左料架上";
    else if(i == 15)
        return "将箱子放到右料架上";
    else if(i == 16)
        return "将小车锁定的层解锁";
    else if(i == 26)
        return "在电梯里面地调度电梯";
    return "unknow Order";
}

bool KDispatch::runInstrucation(OrderStru o)//id can`t write here
{
    o.box_num = m_task.boxNum;
    if(m_task.taskMainType == "出库")
    {
        o.shelves = m_task.fromLocationCode;
    }
    else if(m_task.taskMainType == "入库")
    {
        o.shelves  = m_task.shelves;
    }
    if(!(o.order == Call || o.order == In_elevator_call))
        o.z = m_task_p.z;
    int id = -1;
    if(m_pAbstructInstruction !=nullptr )
    {
        delete m_pAbstructInstruction;
        m_pAbstructInstruction = nullptr;
    }
    if(o.order == Y||o.order == X||o.order == Left_Pickup ||o.order == Right_Pickup ||o.order == Left_WorkBin ||o.order == Right_WorkBin||o.order == Left_Putinto ||o.order == Right_Putinto)
    {
        m_pAbstructInstruction = new CarInstruction();
        id = m_carId;
    }
    else if(o.order == Call)
    {
        //Myconfig::GetInstance()->m_elevatorMutex.lock();
        //Myconfig::GetInstance()->m_elevator_mutex.lock();
        m_pAbstructInstruction = new CarElevatorInstruction();
        id = 68;
    }
    else if(o.order == In_elevator_call)
    {
        m_pAbstructInstruction = new CarElevatorInstruction();
        id = 68;
    }
    else if(o.order == Elevator_In) //car in elevator
    {
        m_pAbstructInstruction = new CarInstruction();
        id = m_carId;
    }
    else if(o.order == Elevator_Out) //car out elevator
    {
        m_pAbstructInstruction = new CarInstruction();
        id = m_carId;
    }
    if(m_pAbstructInstruction != nullptr)
    {
        m_pAbstructInstruction->setParameter(o,id);
        m_pAbstructInstruction->runInstruction();
        QString exeResult = "";
        int returnResult = m_pAbstructInstruction->getResult(exeResult);
        if( returnResult < 0)
        {
            saveErrMassage(exeResult,o.order);
            qDebug()<<o.order<<" execResult: "<<returnResult<<QDateTime::currentDateTime().toString();
            return false;
        }
    }
    return true;
}

void KDispatch::saveErrMassage(const QString &message , const int errCode)
{
    ALARMINFOSTRU arm;
    arm.alarminfo = message;
    arm.alarmlevel = 4;
    arm.boxnumber = m_task.boxNum;
    arm.carcoordx = Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.carCurrentPosion.x;
    arm.carcoordy = Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.carCurrentPosion.y;
    arm.carcoordz = Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.carCurrentPosion.z;
    arm.deviceid = QString("%1").arg(m_carId);
    arm.errorcode = errorCodeChange(errCode);
    arm.errortype = 1;
    arm.Operatestate = 1;
    arm.wmsTaskid = m_task.taskNum;
    QString sql_err;
    if(false == m_writeData.WriteAlarmInfo(arm,sql_err))
        GetSystemLogObj()->writeLog("在执行子任务时保存报错信息失败！message->"+message,2);
}

int KDispatch::errorCodeChange(const int errCode)
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
///
/// \brief KDispatch::makeBody 产生返回体给wms
/// \param t 当前的任务
/// \param interfaceType 接口类型
/// \return
///
QByteArray KDispatch::makeBody(const TaskInfoStru &t,const int interfaceType,QString nowLocation,bool result,QString errMsg= "")
{
    QVariantHash data;
    data.insert("regCode",t.regCode);
    data.insert("interfaceType",QString("%1").arg(interfaceType));
    data.insert("from",t.end);
    data.insert("to",t.from);
    data.insert("taskId",t.taskNum);
    data.insert("taskMainType",t.taskMainType);
    data.insert("taskSonType",t.taskSonType);
    data.insert("status","执行中");
    data.insert("receiptNo",t.receiptNo);//wms number
    data.insert("receiptNoType",t.receiptNoType);//wms number type
    data.insert("containerCode",t.boxNum);
    data.insert("fromLocationCode",t.fromLocationCode);
    data.insert("toLocationCode",t.shelves);//tager position
    data.insert("taskResult",result? "否":"是"); //是否异常
    data.insert("nowLocationCode",nowLocation);
    if(result == true)
        data.insert("abnormalLocationCode",nowLocation);
    else
        data.insert("abnormalLocationCode","");
    data.insert("inbToLocationCode","");
    data.insert("responseMessage",errMsg);
    data.insert("createTime",t.creatTime.toString("yyyy-MM-dd hh:mm:ss"));
    data.insert("requestTimes",t.requestTimes);
    QJsonObject rootObj = QJsonObject::fromVariantHash(data);
    QJsonDocument document;
    document.setObject(rootObj);
    return document.toJson(QJsonDocument::Compact);
}
///
/// \brief KDispatch::whereCarShouldbe 小车完成当前任务后，接下来该怎么走
/// i = 1 正常的判断逻辑，=0不去判断小车的电量和小车预约任务的关系
///
void KDispatch::whereCarShouldbe(int i)
{
    bool result = true;
    QString sqlerr = "";
    if(i == 1)
    {
        if((Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.statusinfodstru.belectricity==0)&&(false == Myconfig::GetInstance()->m_appointMap[m_carId].isEmpty())) //electricticy low !
        {
            return;
        }
    }
    if(Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.status == 3)
    {
        auto Charginglogic = [&](){
            KPosition chargPosition{0,1423,0,0};
            GenerateChargingTrajectory *g = new GenerateChargingTrajectory();
            g->SetPathParam(chargPosition,Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.carCurrentPosion);
            QQueue<OrderStru> orderQueue = g->GetChargingTrajectoryOrders();
            m_task.taskMainType = "充电";
            saveSubTaskInfo(orderQueue);
            QString msg ="";
            int sequnce = 1;
            while(false == orderQueue.isEmpty())
            {
                if(!Myconfig::GetInstance()->m_flag && i == 1)
                {
                    break;
                }
                OrderStru o = orderQueue.dequeue();
                result= runInstrucation(o);
                result?msg = "执行成功" : msg = "执行失败";
                if(!CRUDBaseOperation::getInstance()->changeSubtaskStatus(m_task.taskNum,msg,QString("%1").arg(o.value),sequnce,sqlerr))
                    GetSystemLogObj()->writeLog("change substatus to dbbase failed! ->"+sqlerr,2);
                sequnce++;
                if(result == false) //一条失败整个任务退出,跳出队列，释放小车资源
                {
                    break;
                }
            }
        };
        Charginglogic();
    }

    QMutexLocker locker(&Myconfig::GetInstance()->m_carMap_mutex);
    result ? Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.status = 3:Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.status = 2;
}

void KDispatch::execRunnerInstruction(int startAddress, int value)
{
    OrderStru o;
    o.value = value;   //value
    o.order = call_Runner_Putbox;
    o.startaddress = startAddress; //(i-1)*2 + 60;  //start address
    o.Datatype = 168;//A8 D型寄存器的的代号
    o.childtype = 2;
    AbstructInstruction *e = new RunnerInstruction();
    e->setParameter(o,9);
    e->runInstruction();
    QString resultMsg ="";
    e->getResult(resultMsg);
    qDebug()<<resultMsg;
}

void KDispatch::run()
{
    if(m_task.taskMainType == "出库")// out
    {
        GetOutTrajectory *t = new GetOutTrajectory(m_task_p,m_carId,m_task);
        m_taskQueue = t->getTrajectory();
        qDebug()<<"正在执行出库任务";
    }
    else if(m_task.taskMainType == "入库")
    {
        GenerateInputWarehousingOrders *t = new GenerateInputWarehousingOrders();
        t->SetPathParam(m_task_p,Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.carCurrentPosion);
        m_taskQueue = t->GetInputWarehousingOrders();
        qDebug()<<"正在执行入库任务";
    }
    else if(m_task.taskMainType == "充电")
    {
        whereCarShouldbe(0);
        return;
    }
    saveSubTaskInfo(m_taskQueue);
    //并在此时判断流道是否开启
    if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[14] == 0)
    {
        execRunnerInstruction(22,99);
    }
    bool result = runSubTask();
    if(result)
        m_task.status = "已完成";
    else
        m_task.status = "执行失败";
    reply2WMS(m_task,result);// reply wms the task state
    //保存当前任务完成的状态，完成 未完成，或者报警日志

    QString err = "";
    if(!CRUDBaseOperation::getInstance()->saveCompletedTask(m_task,err))
        GetSystemLogObj()->writeLog("完成任务保存失败!-->"+err,2);
    err = "";
    //    if(!CRUDBaseOperation::getInstance()->removeCrruntTask(m_task,err)) 任务不在由从数据库里面删除 而是由redis里面删除
    //        GetSystemLogObj()->writeLog("移除执行完成任务失败!-->"+err,2);
    removeOldTask(m_task.regCode);
    //1,在完成任务后，退出此线程之前，完成对小车电量和位置的处理
    //2,并在此时判断流道是否开启，如果开启了就关闭流道
    if(result)
    {
        whereCarShouldbe(1);
        if(m_task.taskMainType == "入库" && Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[14] == 99)
        {
            execRunnerInstruction(22,0);
        }
    }
    QMutexLocker locker(&Myconfig::GetInstance()->m_carMap_mutex);
    Myconfig::GetInstance()->m_CarMap[m_carId].deveceStatus.isLocking = false;//让小车和它正在执行的任务解绑，让小车可以接受下一个任务
}
