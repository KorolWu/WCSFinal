
#include "carinstruction.h"
#define TIMEOUT 90000
#include "ControlClass/basedatainfooperate.h"
using namespace  StoreInfo;
CarInstruction::CarInstruction()
{  
    m_result = -4;
    m_instructMsg = "";
}

void CarInstruction::setParameter(OrderStru o, int device_id)
{
    m_id = device_id;
    m_order = o;
}
void CarInstruction::runInstruction()
{
    //小车可以接受指令(无故障，就绪，可以接受指令)
   //if((Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline) && (Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror == false) && (s.statusinfodstru.bunready == true) && (Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready == false))
    struct timeval readytpStart,readytpEnd;
    float readytimeUse = 0;
    gettimeofday(&readytpStart,nullptr);
    while(true)
    {
        if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.isOnline && Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready != 0)
        {
            break;
        }
        gettimeofday(&readytpEnd,nullptr);
        readytimeUse = 1000 *(readytpEnd.tv_sec - readytpStart.tv_sec) + 0.001*(readytpEnd.tv_usec - readytpStart.tv_usec);
        if(readytimeUse >= 20000 || Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror != 0)
        {
            qDebug()<<"wait car ready faild or There is a problem";
            m_result = -5;
            return;
        }
        Delay_MSec(30);
    }
    if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror == 0)
    {
        // get box on elevator,moust be wait elevator ready
        if(m_order.order == 7 || m_order.order == 8 || m_order.order == 9 || m_order.order == 10)
        {
            struct timeval tpStart,tpEnd;
            float timeUse = 0;
            gettimeofday(&tpStart,nullptr);
            while (timeUse < TIMEOUT)
            {
                if(!Myconfig::GetInstance()->m_flag&&Myconfig::GetInstance()->m_flag)
                {
                    m_result = -99;
                    return ;
                }
                //判断电梯是否到位
                //qDebug()<< "order is"<<m_order.order<<"elevator current is:"<<Myconfig::GetInstance()->m_elevatorMap[68].status.curruntLayer<<"判断层"<<m_order.z;
                if((m_order.order == 7 && Myconfig::GetInstance()->m_elevatorMap[68].status.curruntLayer == Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.z)\
                  || (m_order.order == 8 && Myconfig::GetInstance()->m_elevatorMap[68].status.curruntLayer == m_order.z))
                {

                        m_tempValue = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y;
                        m_tempValue += m_order.value;
                        if(false == CRUDBaseOperation::getInstance()->updateCarPosition(m_id,"y",m_tempValue,m_instructMsg))
                        {
                            m_result = 3;

                        }
                        //发送指令
                        //TCommtransceivermanager::GetInstance()->SendcommandByExtern(m_order,m_id);
                        break;
                }
                //暂时屏蔽，先不处理指令 9和10 的问题
                else // =9 or =10 从电梯口取箱子，或者从电梯口放箱子 需要等待流道的可放，可取信号
                {
                    int layer = 31;
                    if(m_order.order == 9)// 放货
                    {
                        layer = 1;
                    }
                    else //取货
                        layer = 2;
                    if(Myconfig::GetInstance()->m_elevatorMap[68].status.curachelayer == layer)//and signal about runner can get box signal
                    {
                        //发送指令
                        //TCommtransceivermanager::GetInstance()->SendcommandByExtern(m_order,m_id);
                        if(m_order.order == 9)
                        {
                            if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[10] == 99)
                            {
                                m_result = 9;
                                //qDebug()<<QString ("放货指令执行中，楼层= %1，可放寄存器D10 = %2").arg(layer).arg(99);
                                break;
                            }
                        }
                        if(m_order.order == 10)
                        {
                            if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[12] == 99)
                            {
                                m_result = 10;
                                //qDebug()<<QString ("取货指令执行中，楼层= %1，可货寄存器D10 = %2").arg(layer).arg(99);
                                break;
                            }
                        }
                    }
                }
                gettimeofday(&tpEnd,nullptr);
                timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
                if(timeUse >= TIMEOUT)
                {
                    m_result = -23;
                    return ;
                }
                QThread::msleep(5);
                QApplication::processEvents();
            }
        }
        else if(m_order.order == 1)
        {
            m_tempValue = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.x;
            m_tempValue += m_order.value;
            m_order.value = m_order.value; //* -1// x direction is contrary so value * -1
            if(false == CRUDBaseOperation::getInstance()->updateCarPosition(m_id,"x",m_tempValue,m_instructMsg))
            {
                m_result = 2;
            }
        }
        else if(m_order.order == 0 )//|| m_order.order == 7 ||m_order.order == 8)//7 8 is test
        {
            m_tempValue = Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y;
            m_tempValue += m_order.value;
            if(false == CRUDBaseOperation::getInstance()->updateCarPosition(m_id,"y",m_tempValue,m_instructMsg))
            {
                m_result = 3;
            }
        }
//        else if(m_order.order == 8) //car out of elevator y change z change too
//        {
//            m_tempValue = m_order.z;
//            if(false == CRUDBaseOperation::getInstance()->updateCarPosition(m_id,"z",m_tempValue,m_instructMsg))
//                m_result = 3;
//        }
        //normal move or get box on store
        m_result = 8;// in instruction order = 7 or =8;取货指令不需要等待其他信号
       TCommtransceivermanager::GetInstance()->SendcommandByExtern(m_order,m_id);
    }
    else
    {
        qDebug()<<"car resive instruction in not ready or There is a problem ";
        m_result = -1;
    }
}

int CarInstruction::getResult(QString &exeMsg)
{
    // qDebug()<<"car instruction is begin!";
    if(m_result == -4)
    {
        exeMsg = QString("%1  Terminate!").arg(m_order.type);
        return m_result;
    }
    else if(m_result == -1)
    {
        exeMsg = QString("%1  The car fails when it is running!").arg(m_order.type);
        return m_result;
    }
    else if(m_result == -5)
    {
        exeMsg = QString("%1 Car execution status not satisfied!").arg(m_order.type);
        return m_result;
    }
    else if(m_result == -23)
    {
        exeMsg = QString("%1 电梯没有到小车呼叫层，或者在流道取放货超时失败!").arg(m_order.type);
        return m_result;
    }
    struct timeval tpStart,tpEnd;
    float timeUse = 0;
    gettimeofday(&tpStart,nullptr);
    struct timeval ws,we;
    float timeUseWait = 0;
    gettimeofday(&ws,nullptr);

    while(true)
    {
        if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bruning!=0)
        {
            break;
        }
        gettimeofday(&we,nullptr);
        timeUseWait = 1000 *(we.tv_sec - ws.tv_sec) + 0.001*(we.tv_usec - ws.tv_usec);
        if(timeUseWait >= 20000 || Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror!=0)
        {
            exeMsg = "等待小车执行信号超时或小车故障"+QString::number(timeUseWait);
            m_result = -21;
            qDebug()<<"Wait for begin single(ready -> not ready) time out";
            return m_result;
        }
        QThread::msleep(10);
        QApplication::processEvents();
    }
    while (timeUse < TIMEOUT)
    {
        if(isTerminate)
        {
            m_result = -99;
            exeMsg = QString("%1  Terminate!").arg(m_order.type);
            return m_result;
        }
        if(m_order.order == Left_Putinto || m_order.order == Right_Putinto || m_order.order == Right_Pickup ||  m_order.order == Left_Pickup||m_order.order ==Left_WorkBin||m_order.order ==Right_WorkBin)
        {
            if(Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror != 0)
            {
                m_result = -22;
                exeMsg = "小车在执行指令时，小车发生故障。";
                return m_result;
            }
        }
       if(Myconfig::GetInstance()->m_exit == false)
       {
           m_result = -24;
           exeMsg = "小车在运行过程中wcs应用退出";
           return m_result;
       }
        //to check car status status 0--->1
       // qDebug()<<Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.berror<<"-"<<Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bunready <<"-"<< Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready;
       if( Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bunready == 0 && Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.readonlystatus == 3 &&Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.statusinfodstru.bready != 0)
        {
            if(m_order.order == X)
            {
                Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.x = m_tempValue;
                //qDebug()<<"---------------------X"<<m_tempValue;
            }
            else if(m_order.order == Y)
            {
                 Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y = m_tempValue;
            }
            else if(m_order.order == Elevator_In)
            {
                Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y = m_tempValue;
                //进电梯的时候将小车所在层解锁
                //Myconfig::GetInstance()->m_layerStatusMap[Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.z].isLocked = false;
            }
            else if(m_order.order == Elevator_Out)//out of elevator update car.z
            {   //update z is up to elevator current z,not task.z
                 Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.z = Myconfig::GetInstance()->m_elevatorMap[68].status.curruntLayer;
                 Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.y = m_tempValue;
                 for(auto it = Myconfig::GetInstance()->m_layerStatusMap.begin();it != Myconfig::GetInstance()->m_layerStatusMap.end();it ++ )
                 {
                     if(it.value().carId == m_id)
                     {
                        // it.value().isLocked = false;
                         it.value().layerstate = LayerState::Free;
                     }
                 }
                 Myconfig::GetInstance()->m_elevatorMap[68].status.isLock = false;// 小车出电梯的时候将电梯从使用中切换到可以使用
                // Myconfig::GetInstance()->m_layerStatusMap[Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.z].isLocked = true;
                 Myconfig::GetInstance()->m_layerStatusMap[Myconfig::GetInstance()->m_CarMap[m_id].deveceStatus.carCurrentPosion.z].layerstate = LayerState::InLayer;

                 Myconfig::GetInstance()->m_elevatorMutex.unlock();//在小车出电梯的瞬间将电梯置为空闲状态
                 Myconfig::GetInstance()->m_elevator_mutex.unlock();//电梯锁解锁
            }
            else if(m_order.order == Right_Pickup ||  m_order.order == Left_Pickup) //pick up (shelves,box_num)
            {
               Myconfig::GetInstance()->m_storeinfoMap[m_order.shelves].storestat = 0;
               Myconfig::GetInstance()->m_storeinfoMap[m_order.shelves].update = true;
                //更新数据状态
//                if(!BaseDataInfoOperate::UpdateWarehousBoxState(m_order.shelves,""))
//                {
//                    qDebug()<<QDateTime::currentDateTime().toString()<<"---取货完成更新货架编号:"<<m_order.shelves<<";result:失败";
//                }
//                else{
//                   qDebug()<<QDateTime::currentDateTime().toString()<<"---取货完成更新货架编号:"<<m_order.shelves<<";result:成功";
//                }
            }
            else if(m_order.order == Left_Putinto || m_order.order == Right_Putinto) //putinto
            {
               //Myconfig::GetInstance()->m_storeinfoMap[m_order.shelves].storestat = 1;
               //Myconfig::GetInstance()->m_storeinfoMap[m_order.shelves].update = true;
                if(!BaseDataInfoOperate::UpdateWarehousBoxState(m_order.shelves,m_order.box_num))
                {
                     qDebug()<<QDateTime::currentDateTime().toString()<<"---放货完成更新货架编号:"<<m_order.shelves<< "箱子编号["<<m_order.box_num<<"]result:失败";
                }
                else{
                     qDebug()<<QDateTime::currentDateTime().toString()<<"---放货完成更新货架编号:"<<m_order.shelves<< "箱子编号["<<m_order.box_num<<"]result:成功";
                }
            }
            m_result = 0;
            exeMsg = QString("%1  Exec successful!").arg(m_order.type);
            break;
        }
        gettimeofday(&tpEnd,nullptr);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= TIMEOUT)
        {
            exeMsg = "等待小车完成信号超时";
            qDebug()<<"car instruction is finished! time out";
            m_result = -41;
            break;
        }
        QThread::msleep(5);
        QApplication::processEvents();
    }
    return m_result;

}

