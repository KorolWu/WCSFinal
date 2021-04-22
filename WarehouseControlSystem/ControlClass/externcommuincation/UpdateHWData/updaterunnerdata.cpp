#include "updaterunnerdata.h"

UpdateRunnerData::UpdateRunnerData()
{
    m_actIndexlist.clear();
    m_stateIndexlist.clear();
    m_sendstateindex = 0;
    m_actiontindex = 0;
    D2 = 0;
}

UpdateRunnerData::~UpdateRunnerData()
{

}

int UpdateRunnerData::SetRunnerParamByFrame(int datatype, QMap<int, int> Data)
{
    QMutexLocker lockermyconfig(&Myconfig::GetInstance()->m_mutex);
    switch (datatype) {
    //    case 1: //DiscreteInputs
    //        break;
    //    case 2://Coils
    //        break;
    //    case 3://InputRegisters
    //        break;
    case 1://HoldingRegisters D0
    {
        for(auto it = Data.begin(); it!= Data.end();++it )
        {
            if(Myconfig::GetInstance()->m_runer.runneratastru.holdresMap.contains(it.key()))
            {
                Myconfig::GetInstance()->m_runer.runneratastru.holdresMap[it.key()] = it.value();
            }
            else{
                Myconfig::GetInstance()->m_runer.runneratastru.holdresMap.insert(it.key(),it.value());
            }
            if(it.key() == 2)//赋值map 地址 2
            {
                if(D2 != it.value())
                {
                    if(it.value() >= 1 && it.value() <= 8)
                    {
                        if(Myconfig::GetInstance()->m_cacheRunerMap.contains(it.value()))
                        {
                            if(!Myconfig::GetInstance()->m_cacheRunerMap[it.value()])
                            {
                                Myconfig::GetInstance()->m_cacheRunerMap[it.value()] = true;
                                qDebug()<<"cache["<<it.value()<<"]置为true";
                            }
                        }
                    }
                    D2 = it.value();
                }
                //qDebug()<<"runner:recedata"<<QString("adress:%1;value:%2").arg(QString::number(it.key())).arg(QString::number(it.value()));
            }
            //qDebug()<<"runner:recedata"<<QString("adress:%1;value:%2").arg(QString::number(it.key())).arg(QString::number(it.value()));
        }
          if(!Myconfig::GetInstance()->m_RunnerMap.contains(m_runnerID))
              return 0;
        for(auto it = Data.begin(); it!= Data.end();++it )
        {
            if(Myconfig::GetInstance()->m_RunnerMap[m_runnerID].runneratastru.holdresMap.contains(it.key()))
            {
                Myconfig::GetInstance()->m_RunnerMap[m_runnerID].runneratastru.holdresMap[it.key()] = it.value();
            }
            else{
                Myconfig::GetInstance()->m_RunnerMap[m_runnerID].runneratastru.holdresMap.insert(it.key(),it.value());
            }
//            if(it.key() == 2)//赋值map 地址 2
//            {
//                if(it.value() >= 1 && it.value() <= 8)
//                {
//                    if(Myconfig::GetInstance()->m_cacheRunerMap.contains(it.value()))
//                    {
//                        if(!Myconfig::GetInstance()->m_cacheRunerMap[it.value()])
//                        {
//                            Myconfig::GetInstance()->m_cacheRunerMap[it.value()] = true;
//                        }
//                    }
//                }
                //qDebug()<<"runner:recedata"<<QString("adress:%1;value:%2").arg(QString::number(it.key())).arg(QString::number(it.value()));
          //  }
            //qDebug()<<"runner:recedata"<<QString("adress:%1;value:%2").arg(QString::number(it.key())).arg(QString::number(it.value()));
        }
        break;
    }
    default:
        break;
    }
    return 1;
}

void UpdateRunnerData::Updateconnectstate(int id, bool value)
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
    //流道连接状态更新
    Q_UNUSED(id)
    //if(Myconfig::GetInstance()->m_runer.contains(id))
    {
        if(Myconfig::GetInstance()->m_runer.isOnline != value)
        {
            Myconfig::GetInstance()->m_runer.isOnline = value;
        }
        if(value)
        {
            if(Myconfig::GetInstance()->m_runer.runneratastru.connectresult !=1)
            {
                Myconfig::GetInstance()->m_runer.runneratastru.connectresult =1;
            }
        }
        else{
            if(Myconfig::GetInstance()->m_runer.runneratastru.connectresult != 0)
            {
                Myconfig::GetInstance()->m_runer.runneratastru.connectresult = 0;
            }
        }
    }
    if(Myconfig::GetInstance()->m_RunnerMap.contains(id))
    {
        m_runnerID = static_cast<uint16_t>(id);
        if(Myconfig::GetInstance()->m_RunnerMap[id].isOnline != value )
        {
            Myconfig::GetInstance()->m_RunnerMap[id].isOnline = value;
        }
        if(value)
        {
            if(Myconfig::GetInstance()->m_RunnerMap[id].runneratastru.connectresult != 1)
            {
                Myconfig::GetInstance()->m_RunnerMap[id].runneratastru.connectresult = 1;
            }
        }
        else{
            if(Myconfig::GetInstance()->m_RunnerMap[id].runneratastru.connectresult != 0)
            {
                Myconfig::GetInstance()->m_RunnerMap[id].runneratastru.connectresult = 0;
            }
        }
    }
}
///
/// \brief UpdateRunnerData::SetRunnParamByPiFrame
/// \param dataframe
/// \param ID
/// \return
///
int UpdateRunnerData::SetRunnParamByPiFrame(QByteArray dataframe, int ID)
{
   // m_runnerID = ID;
    for(int i = 0; i < dataframe.length()/20;++i)
    {
        //如果出现数据粘包则进行分包处理的方式
        QByteArray tempData;
        tempData.append(dataframe.data()+i*20,20);
        //处理成单包数据
        AnalysisFrameData(tempData,ID);
    }
}
///
/// \brief UpdateRunnerData::GetSendCmdIndex
/// \param ID
/// \param frametype
/// \return
///
bool UpdateRunnerData::GetSendCmdIndex(int ID, int frametype,uint16_t &index)
{
     QMutexLocker locker(&m_mutex);
    Q_UNUSED(ID)
    switch (frametype) {
    case 0://代表状态索引
    {
        //        if(m_stateIndexlist.size() > 20)
        //            return  false;
        m_sendstateindex++;
        index = m_sendstateindex;
        m_stateIndexlist.append(index);
        break;
    }
    case 1://代表动作索引编号
    {
        if(m_actIndexlist.size() > 20)
            return  false;
        m_actiontindex++;
        index = m_actiontindex;
        m_actIndexlist.append(index);
        break;
    }
    default:
        break;
    }
    return  true;
}
///
/// \brief UpdateRunnerData::AnalysisFrameData
/// \param frame
///
void UpdateRunnerData::AnalysisFrameData(QByteArray frame,int id)
{

     QMutexLocker locker(&m_mutex);
    if(frame.length() != 20)
        return;
    //如果是高字节序那么需要先转化字节序
     HighByteorderconversion(frame);
    if(frame[4] == 'S'&& frame[5] == 'D')//状态指令的回应
    {       
        WCSReceStatetRequestoRunnerStru statestru;//状态指令报文
        memcpy((char*)(&statestru),frame.data(),20);
        if(statestru.runnernbr == id && statestru.cmdname == 0x05)
        {
            if(m_stateIndexlist.contains(statestru.cmdnbr))//指令编号判断
            {
                UpdateRunnerDataByrole(statestru.runnernbr,cache_in_current,statestru.incache,"");
                UpdateRunnerDataByrole(statestru.runnernbr,cache_out_current,statestru.outcache,"");
                UpdateRunnerDataByrole(statestru.runnernbr,Runnerstate,statestru.state,"");
                UpdateRunnerDataByrole(statestru.runnernbr,statebitmap,statestru.statebitmap);
                int index = m_stateIndexlist.indexOf(statestru.cmdnbr);
                m_stateIndexlist.removeAt(index);
            }
            else{
                qDebug()<<"流道回复的状态索引不存在";
            }
        }
    }
    else if(frame[4] == 'S'&& frame[5] == 'A')//动作指令回应
    {       
        WCSReceRunnersendActInfoStru actionstru;
        memcpy((char*)(&actionstru),frame.data(),20);
        if(m_actIndexlist.contains(actionstru.cmdnbr))//指令编号判断
        {
            if(actionstru.checkinfo == 0x06 && actionstru.cmdname == 4) //回应入库扫码指令
            {
                // 判断指令是否是扫码指令，确定之后进行扫码赋值
                QString code = QString::fromUtf8(actionstru.codeInfo);
                UpdateRunnerDataByrole(actionstru.runnernbr,codeinfo,0,code);
            }
            else if(actionstru.checkinfo == 0x15)
            {
                qDebug()<<"动作指令请求反馈流道格式错误";
            }
            int index = m_actIndexlist.indexOf(actionstru.cmdnbr);
            m_actIndexlist.removeAt(index);
        }
        else{
            //
            qDebug()<<"流道回复的动作索引不存在";
        }
    }
}
///
/// \brief UpdateRunnerData::UpdateRunnerDataByrole
/// \param role
/// \param value
///
void UpdateRunnerData::UpdateRunnerDataByrole(uint16_t id,statebit role, uint8_t value,QString codeinfo)
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
    if(!Myconfig::GetInstance()->m_RunnerMap.contains(id))
    {
        return;
    }
    //更新缓存参数  //更新流道状态位图
    switch (role) {
    case statebit::cache_in_current:
    {
        if(Myconfig::GetInstance()->m_RunnerMap[id].cache_in_current != value)
        {
            Myconfig::GetInstance()->m_RunnerMap[id].cache_in_current = value;
            qDebug()<<"cache_in_current:"<<value;
        }

        break;
    }
    case statebit::cache_out_current:
    {
        if(Myconfig::GetInstance()->m_RunnerMap[id].cache_out_current != value)
        {
            Myconfig::GetInstance()->m_RunnerMap[id].cache_out_current = value;
             qDebug()<<"cache_out_current:"<<value;
        }
        break;
    }
    case statebit::Runnerstate:
    {
        //更新流道状态
        if(Myconfig::GetInstance()->m_RunnerMap[id].state != value)
        {
            Myconfig::GetInstance()->m_RunnerMap[id].state = value;
             qDebug()<<"Runnerstate:"<<value;
        }
        break;
    }
    case statebit::codeinfo:
    {
        //更新流道状态
        if(Myconfig::GetInstance()->m_RunnerMap[id].Incodeinfo != codeinfo)
        {
            Myconfig::GetInstance()->m_RunnerMap[id].Incodeinfo = codeinfo;
            qDebug()<<"codeinfo:"<< codeinfo;
        }
        break;
    }
    default:
        break;
    }
}
///
/// \brief UpdateRunnerData::UpdateRunnerDataByrole
/// \param id
/// \param role
/// \param value
///
void UpdateRunnerData::UpdateRunnerDataByrole(uint16_t id, statebit role, uint8_t *value)
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
    if(!Myconfig::GetInstance()->m_RunnerMap.contains(id))
    {
        return;
    }
    //更新流道状态位图
    switch (role) {
    case statebit::statebitmap:
    {
        for(int i = 0; i < 8; ++i )
        {
            //qDebug()<<"bit value:"<<value[i];
            if(Myconfig::GetInstance()->m_RunnerMap[id].bitstate[i].statebit != value[i])
            {
                Myconfig::GetInstance()->m_RunnerMap[id].bitstate[i].statebit = value[i];
                qDebug()<<"statebitmap:"<< i <<  value[i];
            }
        }
        break;
    }
    default:
        break;
    }

}

void UpdateRunnerData::HighByteorderconversion(QByteArray &frame)
{
    if(frame.length() != 20)
        return;
    //如果是高字节序那么需要先转化字节序
     char value = 0;
     value = frame[0];
     frame[0] = frame[1];
     frame[1] = value;
     value = frame[2];
     frame[2] = frame[3];
     frame[3] = value;
     value = frame[6];
     frame[6] = frame[7];
     frame[7] = value;
     if(frame[4] == 'S'&& frame[5] == 'A')//动作指令回应
     {
         value = frame[8];
         frame[8] = frame[9];
         frame[9] = value;
     }
}
