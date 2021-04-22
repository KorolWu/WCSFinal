#include "updateelevatordata.h"

UpdateElevatorData::UpdateElevatorData()
{

}

UpdateElevatorData::~UpdateElevatorData()
{

}

int UpdateElevatorData::SetElevatorParamByFrame( int ID , QMap<int, int> Data)
{
    QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex);
    for(auto it = Data.begin(); it!= Data.end();++it )
    {
        if(Myconfig::GetInstance()->m_elevatorMap.contains(ID))
        {
            if(it.key() == 100) //读取楼层
            {

                if(Myconfig::GetInstance()->m_elevatorMap[ID].status.curruntLayer != it.value())
                {
                    Myconfig::GetInstance()->m_elevatorMap[ID].status.curruntLayer = it.value();
                }
            }
            else if(it.key() == 101 )//读取缓存
            {
                if(Myconfig::GetInstance()->m_elevatorMap[ID].status.curachelayer != it.value())
                {
                    Myconfig::GetInstance()->m_elevatorMap[ID].status.curachelayer = it.value();
                }
            }
            else if(it.key() == 102 )//读取缓存
            {
                if(Myconfig::GetInstance()->m_elevatorMap[ID].status.curachelayer != it.value())
                {
                    Myconfig::GetInstance()->m_elevatorMap[ID].status.curachelayer = it.value();
                }
            }
        }
    }
    return 1;
}

void UpdateElevatorData::Updateconnectstate(int ID, bool value)
{
    if(Myconfig::GetInstance()->m_elevatorMap.contains(ID))
    {
        if(Myconfig::GetInstance()->m_elevatorMap[ID].status.isOnline != value)
        {
            Myconfig::GetInstance()->m_elevatorMap[ID].status.isOnline = value;
        }
    }
}
