#include "dispatchcenter.h"

DispatchCenter::DispatchCenter()
{
    m_pSelectCar = new SelectCar();
    m_count_text = 1;
    m_in_cache_max = Myconfig::GetInstance()->m_runer.cache_in_max;
    m_out_cahce_max = Myconfig::GetInstance()->m_runer.cache_out_max;
    qDebug()<<"max"<<m_in_cache_max<<"out"<<m_out_cahce_max;
}

void DispatchCenter::dispatchTaskThread()
{
    while(Myconfig::GetInstance()->m_exit)
    {
        if(Myconfig::GetInstance()->m_flag == true && Myconfig::GetInstance()->m_exit)
        {
            //qDebug()<<"dispathch thread runing!";
            //扫描预定队列，如果里.面有小车空闲，则安排她完成接下来的任务
            for(auto it = Myconfig::GetInstance()->m_appointMap.begin();it != Myconfig::GetInstance()->m_appointMap.end();it++)
            {
                if(it.value().isEmpty() == true)
                {
                    Myconfig::GetInstance()->m_CarMap[it.key()].isLockByTask = false;
                    continue;
                }
                //QString ip = it.key();
                QString ip = "it.key()";
                int carId = it.key();
                if(Myconfig::GetInstance()->m_CarMap[it.key()].deveceStatus.isLocking == false&&Myconfig::GetInstance()->m_CarMap[it.key()].deveceStatus.statusinfodstru.berror==0)
                {
                    TaskInfoStru t = it.value().dequeue();
                    KPosition task_p;//根据料箱号返回料箱所在坐标
                    QString result = StoreInfo::BaseDataInfoOperate::GetWarehouselocationInfoForOut(t.boxNum,task_p);
                    if(result != "")
                    {
                        t.shelves = result;
                        lock_car(carId);
                        set_car_task_position(task_p,carId);
                        KDispatch *k = new KDispatch(task_p,ip,carId,t);//完成的状态，完成的结果，写入数据库的时间??
                        if(t.taskMainType == "入库")
                            //reduceCacheIn(stat);
                            m_writeData.WriteLoginfo(0,"Dispatch Info","将任务 "+t.taskNum +QString(" 分配给:%1").arg(carId));
                        QThreadPool::globalInstance()->start(k);
                        qDebug()<<"正在执行预约任务";
                    }
                }
                else
                {
                    delayMSec(50);
                }
            }
            m_car_ip = -999;
            if(Myconfig::GetInstance()->m_runer.cache_in_current < 3  && Myconfig::GetInstance()->m_taskQueue.isEmpty() == false)//&& Myconfig::GetInstance()->m_runer.cache_out_current < m_out_cahce_max
            {
                if(m_pSelectCar->hasUseCar())
                {
                    QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
                    m_task =  Myconfig::GetInstance()->m_taskQueue.at(0);
                    remove_task_from(m_task.taskNum);
                    handle_out_task(m_task);
                }
                else // no car use
                {
                    QThread::msleep(10);
                }
                QEventLoop loop;
                QTimer::singleShot(100,&loop,SLOT(quit()));
                loop.exec();
            }
            else // 分配入库
            {
                //&& Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[8] == 99 && Myconfig::GetInstance()->m_runer.cache_in_current > 0流道扫码信号 和有入库缓存信号
                if(m_pSelectCar->hasUseCar())
                {
                    scanCode();
                    if(false == Myconfig::GetInstance()->m_in_taskMap.isEmpty())
                    {
                        QString frist_in_boxNum = Myconfig::GetInstance()->m_boxNum_in;
                        if(Myconfig::GetInstance()->m_in_taskMap.contains(frist_in_boxNum))
                        {
                            TaskInfoStru t = Myconfig::GetInstance()->m_in_taskMap[frist_in_boxNum];
                            handle_in_task(t,frist_in_boxNum);
                            qDebug()<<"正在处理入库任务";
                        }
                        else
                        {
                            QThread::msleep(40);
                            continue;
                        }
                    }
                }
            }
            QThread::msleep(40);
            //box_getCache();
        }
        QThread::msleep(5);
    }
}
///
/// \brief DispatchCenter::dispatchTaskThreadV1
/// 2021/01/23  适配多个流道
void DispatchCenter::dispatchTaskThreadV1()
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
                {    //执行入库任务
                    if(Myconfig::GetInstance()->m_IntaskMap.contains(stationNum) && (Myconfig::GetInstance()->m_IntaskMap[stationNum].size() >= 4 || Myconfig::GetInstance()->m_OuttaskMap[stationNum].size() == 0))
                    {
                        if(m_pSelectCar->hasUseCar())
                        {
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
                                        handle_in_task(t,boxNum,stationNum);
                                        qDebug()<<"正在处理入库任务";
                                    }
                                }
                                else
                                {
                                    delayMSec(40);
                                    continue;
                                }
                            }
                        }
                        delayMSec(50);
                    }
                    else //执行出库任务
                    {
                        if(Myconfig::GetInstance()->m_OuttaskMap.contains(stationNum) && false == Myconfig::GetInstance()->m_OuttaskMap[stationNum].isEmpty())
                        {
                            if(m_pSelectCar->hasUseCar())
                            {
                                QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
                                m_task =  Myconfig::GetInstance()->m_OuttaskMap[stationNum].at(0);
                                handle_out_task(m_task,stationNum);
                            }
                            else // no car use
                            {
                                delayMSec(10);
                            }
                            delayMSec(100);
                        }
                    }
                }
            }
        }
        delayMSec(50);
    }
}

void DispatchCenter::lock_layer(int layer, int lock_car_id)
{
    if(Myconfig::GetInstance()->m_layerStatusMap.contains(layer))
    {
       // Myconfig::GetInstance()->m_layerStatusMap[layer].isLocked = true;
        Myconfig::GetInstance()->m_layerStatusMap[layer].layerstate = LayerState::InLayer;
        Myconfig::GetInstance()->m_layerStatusMap[layer].carId = lock_car_id;

    }
    else
    {
        LayerStru v;
        v.carId = lock_car_id;
       // v.isLocked = true;
        v.layerstate = LayerState::InLayer;
        Myconfig::GetInstance()->m_layerStatusMap.insert(layer,v);

    }
}

void DispatchCenter::set_car_task_position(const KPosition &p, const int &carId)
{
    Myconfig::GetInstance()->m_CarMap[carId].task_position = p;
}

void DispatchCenter::lock_car(int carId)
{
    Myconfig::GetInstance()->m_CarMap[carId].deveceStatus.isLocking = true;
}

void DispatchCenter::Task_execution_failed(const TaskInfoStru &t)
{
    //任务执行失败，没有找到目标库位
    //it.status = "执行失败";
    qDebug()<<"执行失败,没有查到料箱对应的库位";
    ALARMINFOSTRU arm;
    arm.alarminfo = "没有查到料箱对应的库位";
    arm.alarmlevel = 4;
    arm.boxnumber = t.boxNum;
    arm.carcoordx = Myconfig::GetInstance()->m_CarMap[m_car_ip].deveceStatus.carCurrentPosion.x;
    arm.carcoordy = Myconfig::GetInstance()->m_CarMap[m_car_ip].deveceStatus.carCurrentPosion.y;
    arm.carcoordz = Myconfig::GetInstance()->m_CarMap[m_car_ip].deveceStatus.carCurrentPosion.z;
    arm.deviceid = QString("%1").arg(m_car_ip);
    arm.errorcode = 004;//流程故障
    arm.errortype = 1;
    arm.Operatestate = 1;
    arm.wmsTaskid = t.taskNum;
    QString sql_err;
    if(false == m_writeData.WriteAlarmInfo(arm,sql_err))
        qDebug()<<"alarm insert failed!";
    //CRUDBaseOperation::getInstance()->saveCompletedTask(m_task);

}

void DispatchCenter::remove_task_from(QString task_num)
{
    if(Myconfig::GetInstance()->m_taskMap.contains(task_num))
    {
        QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_taskMap);
        Myconfig::GetInstance()->m_taskMap.remove(task_num);
    }
}

void DispatchCenter::handle_in_task(TaskInfoStru &t, QString frist_in_boxNum, int station)
{
    KPosition task_p;//根据料箱号返回料箱所在坐标
    QString shelves_name = t.shelves;
    //bool result = StoreInfo::BaseDataInfoOperate::GetWarehouselocationInfoForIn_V1(frist_in_boxNum,task_p,shelves_name);
    bool result =  StoreInfo::BaseDataInfoOperate::GetKposInfoByIDnbr(t.shelves,task_p);
    if(result && Myconfig::GetInstance()->m_storeinfoMap.contains(shelves_name))
    {
        //qDebug()<<"给料箱 "+t.taskNum+" 分配的货架号为"+shelves_name;
        Myconfig::GetInstance()->m_storeinfoMap[shelves_name].storestat = 2; //lock shelves 料想即将到达该库位

        //remove_task_from(t.taskNum);
        QString ip = Myconfig::GetInstance()->m_layerStatusMap[task_p.z].CarIP;
        //int car_num = Myconfig::GetInstance()->m_layerStatusMap[task_p.z].carId;
        //int car_task_z = Myconfig::GetInstance()->m_CarMap[car_num].task_position.z;
        //先判断目标层是否有车
        //        if(true == Myconfig::GetInstance()->m_layerStatusMap[task_p.z].isLocked)//&& Myconfig::GetInstance()->m_CarMap[ip].deveceStatus.isLocking
        //        {
        //            if(car_task_z == task_p.z)
        //            {
        //                Myconfig::GetInstance()->m_appointMap[car_num].enqueue(t);
        //                Myconfig::GetInstance()->m_CarMap[car_num].isLockByTask = true;
        //                qDebug()<<"正在执行入库任务,当前层有车，且忙碌，将入库任务提前分配给此小车";
        //                remove_in_task(frist_in_boxNum,station);//移除此入库任务
        //            }
        //            return;
        //        }
        //根据坐标返回分配小车的坐标
        m_car_ip = m_pSelectCar->getCarIp_out(task_p);
        if(m_car_ip == -999)
        {
            qDebug()<<"select car id = -999";
            return;
        }
        //锁定小车即将到达的层
        lock_layer(task_p.z,m_car_ip);
        //记录小车任务
        set_car_task_position(task_p,m_car_ip);
        // carP   boxP   elevatorP  in or out?
        t.shelves = shelves_name;
        KDispatch *k = new KDispatch(task_p,ip,m_car_ip,t);//完成的状态，完成的结果，写入数据库的时间??
        reduceCacheIn(station);
        m_writeData.WriteLoginfo(0,"Dispatch Info","将任务 "+t.taskNum +QString(" 分配给%1").arg(m_car_ip));
        QThreadPool::globalInstance()->start(k);
        qDebug()<<m_car_ip<<" runing in Task Number: "+QString::number(m_count_text);
        remove_in_task(frist_in_boxNum,station); //移除此任务
        //qDebug()<<"in size "<<Myconfig::GetInstance()->m_in_taskMap.size();
    }
    else
    {
        Task_execution_failed(t);//出现没有找到料箱，但是队列里面任务一直没有删掉，出现死循环
        //remove_task_from(t.taskNum);
        remove_in_task(t.boxNum,station);
    }

}

void DispatchCenter::handle_out_task(TaskInfoStru &t,int station)
{
    KPosition task_p;//根据料箱号返回料箱所在坐标
    //QString result = StoreInfo::BaseDataInfoOperate::GetWarehouselocationInfoForOut(t.boxNum,task_p);
    bool result = StoreInfo::BaseDataInfoOperate::GetKposInfoByIDnbr(t.fromLocationCode,task_p);
    qDebug()<<t.fromLocationCode<<" :the end position is ("<<task_p.x<<","<<task_p.y<<","<<task_p.z<<")"<<"shelves existence:"<<result;
    if(result)
    {
        QString ip = Myconfig::GetInstance()->m_layerStatusMap[task_p.z].CarIP;
        //int car_id = Myconfig::GetInstance()->m_layerStatusMap[task_p.z].carId;
        //int car_task_z = Myconfig::GetInstance()->m_CarMap[car_id].task_position.z;
        //先判断目标层是否有车
        //        if(true == Myconfig::GetInstance()->m_layerStatusMap[task_p.z].isLocked)//&& Myconfig::GetInstance()->m_CarMap[ip].deveceStatus.isLocking
        //        {
        //            //小车正在执行的任务的Z 是否和本次任务的z相等
        //            if(car_task_z == task_p.z)
        //            {
        //                Myconfig::GetInstance()->m_appointMap[car_id].enqueue(t);
        //                Myconfig::GetInstance()->m_CarMap[car_id].isLockByTask = true;
        //                //将此任务分配给这个小车
        //                //将这个车辆锁住（任务锁，意味着这辆车不可以被调到其他层）
        //                //然后将这个任务放在一个集合里面，然后在大循环里面扫描这个集合，分配任务 Map<ip,queue<task>>
        //                remove_out_task(station);
        //                qDebug()<<"正在执行出库任务,当前层有车，且忙碌，将此任务提前分配给此小车";
        //            }
        //            return;
        //        }
        //根据坐标返回分配小车的坐标
        m_car_ip = m_pSelectCar->getCarIp_out(task_p);
        if(m_car_ip == -999)
        {
            qDebug()<<"select car id = -999";
            return;
        }
        //锁定小车即将到达的层
        lock_layer(task_p.z,m_car_ip);
        set_car_task_position(task_p,m_car_ip);
        remove_out_task(station);
        // carP   boxP   elevatorP  in or out?
        t.shelves = result;
        KDispatch *k = new KDispatch(task_p,ip,m_car_ip,t);//完成的状态，完成的结果，写入数据库的时间??
        m_writeData.WriteLoginfo(0,"Dispatch Info",QString("将任务 %1 分配给 %2").arg(t.taskNum ).arg(m_car_ip));
        QThreadPool::globalInstance()->start(k);
        qDebug()<<m_car_ip<<" Task Number: "+QString::number(m_count_text);
        m_count_text++;
    }
    else
    {
        Task_execution_failed(t);
        Myconfig::GetInstance()->m_OuttaskMap[station].dequeue();
    }
}
//移除相机拍到的最新的那个料箱对应的任务
void DispatchCenter::remove_in_task(const QString &frist_in_boxNum, int station)
{
    //qDebug()<<" remove box_number<<"<<frist_in_boxNum<<"station "<<station;
    if(Myconfig::GetInstance()->m_IntaskMap[station].contains(frist_in_boxNum))
    {
        QMutexLocker locker(&Myconfig::GetInstance()->m_in_task_mutex);
        Myconfig::GetInstance()->m_IntaskMap[station].remove(frist_in_boxNum);
        qDebug()<<"remove it.key";
    }
}

void DispatchCenter::box_getCache()
{
    if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[6] == 99)
    {
        int cacheNum = getfreeCache();
        if(cacheNum != -1)
        {
            OrderStru o;
            o.value = 99;
            o.order = call_Runner_Putbox;
            o.startaddress = cacheNum;
            o.Datatype = 4;
            o.childtype = 2;
            AbstructInstruction *e = new RunnerInstruction();
            e->setParameter(o,2);
            e->runInstruction();
            QString resultMsg ="";
            e->getResult(resultMsg);
        }
    }
}

int DispatchCenter::getfreeCache()
{
    for(int i = 0; i < Myconfig::GetInstance()->m_cacheRunerMap.size();i++)
    {
        if(Myconfig::GetInstance()->m_cacheRunerMap[i] == false)
        {
            return i*2+62;
        }
    }
    return -1;
}

void DispatchCenter::scanCode()
{
    //scan and go
    AbstructInstruction *c = new ScanCodeInstruction();
    OrderStru o;
    o.strData = "scan_2";
    o.order = Scan_Code;
    c->setParameter(o,62);
    QString execStr = "";
    if(c->getResult(execStr) != 1)
    {
        //return;
    }
    if(Myconfig::GetInstance()->m_boxNum_in == "")
        return;

}

void DispatchCenter::delayMSec(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
///
/// \brief DispatchCenter::reduceCacheIn
/// \param station  站点[station]的入库缓存数减一
///
void DispatchCenter::reduceCacheIn(int station)
{
    if(!Myconfig::GetInstance()->m_RunnerMap.contains(station))
        return;
    if(Myconfig::GetInstance()->m_RunnerMap[station].cache_in_current != 0)
        Myconfig::GetInstance()->m_RunnerMap[station].cache_in_current--;
}


void DispatchCenter::remove_out_task(int station)
{
    //QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
    Myconfig::GetInstance()->m_OuttaskMap[station].dequeue();
}
