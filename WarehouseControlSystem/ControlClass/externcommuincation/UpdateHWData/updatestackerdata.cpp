#include "updatestackerdata.h"
#include <QDebug>

UpdateStackerData::UpdateStackerData()
{
  m_sendcarryindex = -1;
  mRprefix = Myconfig::GetInstance()->m_SrmReadPrefix;

}
///
/// \brief UpdateStackerData::Updateconnectstate
/// \param id
/// \param value
///
void UpdateStackerData::Updateconnectstate(int id, bool value)
{
    if(Myconfig::GetInstance()->m_stackerMap.contains(id))
    {
        if(value != Myconfig::GetInstance()->m_stackerMap[id].online)
        {
            Myconfig::GetInstance()->m_stackerMap[id].online = value;
        }
    }
}
///
/// \brief UpdateStackerData::SetStackerParam
/// \param id
/// \param valueData
///
void UpdateStackerData::SetStackerParam(int id, QMap<QString, QVariant> valueData)
{
    if(Myconfig::GetInstance()->m_stackerMap.contains(id))
    {
        QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex);
        for(auto it = valueData.begin(); it != valueData.end();++it )
        {
            int value = it.value().toInt();
            QString nodename = it.key();
//            if(nodename.size() > mRprefix.size())
//            {
//               nodename = nodename.mid(mRprefix.size());
//            }
           // qDebug()<<"SetStackerParam read data result:"<<nodename << value;
            if(nodename.contains("handShake"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.handShake)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.handShake = value;
                }
            }
            else if(nodename.contains("srmNo"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.srmNo)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.srmNo = value;
                }
            }
            else if(nodename.contains("mode"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.mode)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.mode = value;
                }
            }
            else if(nodename.contains("alarm"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.alarm)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.alarm =value;
                }
            }
            else if(nodename.contains("liftFull"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.liftFull)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.liftFull =value;
                }
            }
            else if(nodename.contains("posX")&&(!nodename.contains("posXmm")))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posX)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posX=value;
                }
            }
            else if(nodename.contains("posY")&&(!nodename.contains("posYmm")))
            {
                      //  qDebug()<<"resive posY ="<< value;
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posY)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posY=value;
                }
            }
            else if(nodename.contains("posZ")&&(!nodename.contains("posZmm")))
            {
                       // qDebug()<<"resive posZ ="<< value;
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posZ)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posZ = value;
                }
            }
            else if(nodename.contains("posXmm"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posXmm)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posXmm = value;
                }
            }
            else if(nodename.contains("posYmm"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posYmm)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posYmm = value;
                }
            }
            else if(nodename.contains("posZmm"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posZmm)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.posZmm = value;
                }
            }
            else if(nodename.contains("pickFinish"))
            { if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.pickFinish)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.pickFinish = value;
                }
            }
            else if(nodename.contains("deliveryFinish"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.deliveryFinish)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.deliveryFinish = value;
                }
            }
            else if(nodename.contains("taskFinish"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.taskFinish)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.taskFinish = value;
                }
            }
            else if(nodename.contains("Alarm_number"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.Alarm_number)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.Alarm_number = value;
                }
            }
            else if(nodename.contains("state"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.state)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.state = value;
                }
            }
            else if(nodename.contains("ActualLane"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.ActualLine)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.ActualLine = value;
                }
            }
            else if(nodename.contains("TaskNo"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.TaskNo)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.TaskNo = value;
                }
            }
            else if(nodename.contains("ack"))
            {
                if(value !=  Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.ack)
                {
                    Myconfig::GetInstance()->m_stackerMap[id].Stacker_r.ack = value;
                }
            }
        }
    }

}
///
/// \brief UpdateStackerData::GetTaskIndex
/// \param role
/// \return
///
qint32 UpdateStackerData::GetTaskIndex(int role)
{
    qint32 index = 0;
    switch (role) {
    case 0:
    {
        m_sendcarryindex++;
        index = m_sendcarryindex;
        break;
    }
    case 1:
    {
        break;
    }
    default:
        break;
    }
    return  index;
}
