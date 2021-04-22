#include "generatechargingtrajectory.h"

GenerateChargingTrajectory::GenerateChargingTrajectory()
{
    m_chargingposx = 0.0;
    m_chargingposy = 1423;
    m_roady = 0.0;
}

GenerateChargingTrajectory::~GenerateChargingTrajectory()
{

}

void GenerateChargingTrajectory::SetPathParam(KPosition task_P, KPosition curcarPosion)
{
    m_trapos = task_P;
    m_carPos = curcarPosion;
    m_chargingposy = task_P.y;
    m_chargingposx = task_P.x;
}

QQueue<OrderStru> GenerateChargingTrajectory::GetChargingTrajectoryOrders()
{
    m_taskQueue.clear();
    //小车先随小车电梯到达流道层位置点
    GetCarchargingPath();
//    for(int i = 0; i < m_taskQueue.size();++i)
//    {
//        qDebug()<<"充电轨迹指令名字:"<<GetorderNameByValue(m_taskQueue.at(i).order)<< "指令参数:"<<m_taskQueue.at(i).value;
//    }
    return m_taskQueue;
}

QString GenerateChargingTrajectory::GetorderNameByValue(Order value)
{
    QString name;
    switch (value) {
    case X:
        name = "X方向移动";
        break;
    case Y:
        name = "Y方向移动";
        break;
    case ChangeWhell:
        name = "ChangeWhell";
        break;
    case Left_Pickup:
        name = "取仓位左边的箱子状态为0";
        break;
    case Right_Pickup:
        name = "取仓位右边的箱子状态为1";
        break;
    case Call:
        name = "呼叫小车电梯到指定层数";
        break;
    case Elevator_Near:
        name = "到达小车电梯临近点";
        break;
    case Elevator_In:
        name = "呼叫小车进电梯";
        break;
    case Elevator_Out:
        name = "呼叫小车出电梯";
        break;
    case Left_WorkBin:
        name = "放入出库电梯的货物";
        break;
    case Right_WorkBin:
        name = "取入库电梯的货物";
        break;
    case  Call_WorkBin_Out:
        name = "呼叫出库电梯到达指定层数";
        break;
    case Call_WorkBin_In:
        name = "呼叫入库电梯到达指定层数";
        break;
    case call_Runner_Putbox:
        name = "呼叫流道把箱子放入电梯";
        break;
    case Left_Putinto:
        name = "把箱子放入左边仓位状态为0";
        break;
    case Right_Putinto:
        name = "把箱子放入右边的仓位状态为1";
        break;
    case In_elevator_call:
        name = "小车在电梯里面呼叫电梯";
        break;
    default:
        break;
    }
    return name;
}
///
/// \brief GenerateChargingTrajectory::GetCarchargingPath
///生成轨迹指令内容方式
void GenerateChargingTrajectory::GetCarchargingPath()
{
     OrderStru orderstru;
    if(!(m_carPos.x  ==  m_chargingposx && m_carPos.y == m_chargingposy)) //  x相同坐标
    {
        if(m_carPos.x  == m_chargingposx)
        {
            orderstru.order = Y;
            orderstru.value =  m_chargingposy - m_carPos.y;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.y = m_chargingposy;
        }
        else if(m_carPos.y == m_roady)
        {
            orderstru.order = X;
            orderstru.value =  m_chargingposx - m_carPos.x;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.x = m_chargingposx;
            orderstru.order = Y;
            orderstru.value = m_chargingposy - m_carPos.y;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.y = m_chargingposy;
        }
        else {
            orderstru.order = Y;
            orderstru.value =  m_roady-m_carPos.y;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.y = m_roady;
            orderstru.order = X;
            orderstru.value =  m_chargingposx - m_carPos.x;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.x = m_chargingposx;
            orderstru.order = Y;
            orderstru.value =  m_chargingposy - m_carPos.y;
            if(orderstru.value != 0)
            {
                m_taskQueue.append(orderstru);
            }
            m_carPos.y = m_chargingposy;
        }
    }
}
