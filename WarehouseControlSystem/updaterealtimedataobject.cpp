#include "updaterealtimedataobject.h"
#include "UnitClass/c_systemlogmng.h"

UpdateRealtimeDataObject::UpdateRealtimeDataObject()
{
    mRstackerVar.clear();
    mhandShakevalue = 0;

}

UpdateRealtimeDataObject::~UpdateRealtimeDataObject()
{

}
///
/// \brief UpdateRealtimeDataObject::SaveStoreinfotoDatabase
///1s 更新货架信息数据到本地数据库
void UpdateRealtimeDataObject::SaveStoreinfotoDatabase()
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_rmutex);
    QMap<QString, StorePosInfoStru> updatemap;
    QVector<QVariant> keyvalue;
    auto it = Myconfig::GetInstance()->m_storeinfoMap.begin();
    for(;it !=Myconfig::GetInstance()->m_storeinfoMap.end(); ++it)
    {
        if(it.value().update)
        {
            updatemap.insert(it.key(),it.value());
            keyvalue.append(it.key());
        }
    }
    if(updatemap.size() > 0)
    {
        //写入数据库
        QString msg;
        if(!m_databaseopob.WriteUpdateInfoDataBase(updatemap,keyvalue,msg,false))
        {
            qDebug()<<"货架信息数据写入失败:"<< msg;
            GetSystemLogObj()->writeLog(msg,2);
        }
        else{ //更新成功
            QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
            for(int i = 0; i < keyvalue.size(); ++i)
            {
                QString key = keyvalue[i].toString();
                if(Myconfig::GetInstance()->m_storeinfoMap.contains(key))
                {
                    Myconfig::GetInstance()->m_storeinfoMap[key].update = false;
                }
            }
        }
    }
}
///
/// \brief UpdateRealtimeDataObject::RequestTimingupdateHWinfo
///  小车信息数据一直请求更新 发送详细数据类型
///  流道信息请求 定时请求任务更新
void UpdateRealtimeDataObject::RequestTimingupdateHWinfo()
{
    UpdateRunnerDataRequest();
    UpdateElevatorDataRequest();
}

void UpdateRealtimeDataObject::RequestTimingupdateCarHWinfo()
{
    UpdateCarDataRequest();
}
///
/// \brief UpdateRealtimeDataObject::UpdateCarDataRequest
///小车数据请求方式
void UpdateRealtimeDataObject::UpdateCarDataRequest()
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_rmutex);
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.begin(); it !=Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.end(); ++it  )
    {
        OrderStru carstru;
        carstru.childtype = 6;// 发送请求指令
        carstru.value = 5;//请求的数据指令，为详细类型
        if(it.value().hwtype == RGVCAR && it.value().protype == KTcpClient)
        {
            if(!Myconfig::GetInstance()->m_CarMap[it.key()].deveceStatus.isOnline /*||  Myconfig::GetInstance()->m_CarMap[it.key()].deveceStatus.status == 2 || Myconfig::GetInstance()->m_CarMap[it.key()].deveceStatus.status == 4*/ )
                continue;
            TCommtransceivermanager::GetInstance()->SendcommandByExtern(carstru,it.key());
            //carstru.value = 6;
            //TCommtransceivermanager::GetInstance()->SendcommandByExtern(carstru,it.key());
        }
    }
}
///
/// \brief UpdateRealtimeDataObject::UpdateRunnerDataRequest
///流道数据请求
void UpdateRealtimeDataObject::UpdateRunnerDataRequest()
{
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.begin(); \
        it !=Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.end(); ++it  )
    {
        if( it.value().protype == kMCMelsecTCPClient&& it.value().hwtype == RUNNER)
        {
            OrderStru runnerstru;
            runnerstru.childtype = 5;
            runnerstru.Datatype = 168; //寄存器 读写
            runnerstru.startaddress = 0;
            runnerstru.numberOfEntries = 23;
            TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,it.key());//d0-d20
            runnerstru.startaddress = 60;
            runnerstru.numberOfEntries = 15;
            //TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,it.key());//d60-d74
        }
        if( it.value().protype == KTcpClient&& it.value().hwtype == RUNNER)
        {
            OrderStru runnerstru;
            runnerstru.childtype = 5;
            TCommtransceivermanager::GetInstance()->SendcommandByExtern(runnerstru,it.key());
        }
    }
}

void UpdateRealtimeDataObject::UpdateElevatorDataRequest()
{
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwSerialPortMap.begin(); \
        it !=Myconfig::GetInstance()->m_hwcommstru.hwSerialPortMap.end(); ++it  )
    {
        if(it.value().hwtype == ELEVATOR_CAR )
        {
            if(!Myconfig::GetInstance()->m_elevatorMap[it.key()].status.isOnline)
                continue;
            OrderStru carelestru;
            carelestru.childtype = 5;
            carelestru.Datatype = 4; // 代表寄存器 读写
            carelestru.startaddress = 100;
            carelestru.numberOfEntries = 3;
            //TCommtransceivermanager::GetInstance()->SendcommandByExtern(carelestru,it.key());//d50-d52
            //carelestru.startaddress = 6;
            TCommtransceivermanager::GetInstance()->SendcommandByExtern(carelestru,it.key());//d100-d102
        }
    }
}
///
/// \brief UpdateRealtimeDataObject::UpdateStackerDataRequest
///
void UpdateRealtimeDataObject::UpdateStackerDataRequest()
{
    if(mRstackerVar.size() == 0)
    {
        GetRstackerVar();
    }
    //定时更新读数据寄存器的变量
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.begin(); it !=Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.end(); ++it  )
    {
        if(it.value().hwtype == STACKER && it.value().protype == KOPCUAClient)
        {
            //堆垛机 对象数据的更新
            OrderStru stackerstru;
            stackerstru.childtype = 5;
            stackerstru.Rcmdstackernames = mRstackerVar;
            TCommtransceivermanager::GetInstance()->SendcommandByExtern(stackerstru,it.key());
            if(mhandShakevalue == 0)
            {
                mhandShakevalue = 1;
            }
            else{
                 mhandShakevalue = 0;
            }
            SetStackerhandshake(mhandShakevalue,it.key());
        }
    }
}
///
/// \brief UpdateRealtimeDataObject::GetRstackerVar
///
void UpdateRealtimeDataObject::GetRstackerVar()
{
    mRstackerVar.clear();
    mRstackerVar.append("handShake");
    mRstackerVar.append("srmNo");
    mRstackerVar.append("mode");
    mRstackerVar.append("alarm");
    mRstackerVar.append("liftFull");
    mRstackerVar.append("posX");
    mRstackerVar.append("posY");
    mRstackerVar.append("posZ");
    mRstackerVar.append("posXmm");
    mRstackerVar.append("posYmm");
    mRstackerVar.append("posZmm");
    mRstackerVar.append("pickFinish");
    mRstackerVar.append("deliveryFinish");
    mRstackerVar.append("taskFinish");
    mRstackerVar.append("Alarm_number");
    mRstackerVar.append("state");
    mRstackerVar.append("ActualLane");
    mRstackerVar.append("TaskNo");
    mRstackerVar.append("ack");
    QString Rprefix = Myconfig::GetInstance()->m_SrmReadPrefix;
    for(int i = 0; i < mRstackerVar.size(); ++i)
    {
        mRstackerVar[i] =  "ns=3;s=\"T_WCS_DB\".\"" + mRstackerVar[i]+"\"";
    }
}
///
/// \brief UpdateRealtimeDataObject::SetStackerhandshake
/// \param value
/// \param id
///心跳交替发送
void UpdateRealtimeDataObject::SetStackerhandshake(int16_t value ,int id)
{
     QString wprefix = Myconfig::GetInstance()->m_SrmWritePrefix;
    OrderStru cmd;
    cmd.childtype = 1;
    cmd.Datatype = QOpcUa::Types::Int16;
    QMap<QString,QVariant> valuemap;//变量的名字
    valuemap.insert("ns=3;s=\"F_WCS_DB\".\"handShake\"",value); //"ns=3;s=\"F_WCS_DB\".\"handShake\""
    cmd.WcmdstackerValues = valuemap;
    TCommtransceivermanager::GetInstance()->SendcommandByExtern(cmd,id);
}
