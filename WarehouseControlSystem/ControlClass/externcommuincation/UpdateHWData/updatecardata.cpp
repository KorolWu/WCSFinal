#include "updatecardata.h"
#include "UnitClass/c_systemlogmng.h"
#include <QDebug>

UpdateCarData::UpdateCarData()
{
    m_senddetailindexlist.clear();
    m_sendactionindexlist.clear();
    m_RequeErrorflag = false;
    m_senddetailindex = 2001;
    m_actiontindex = 2001;
    m_ErrorNbr.clear();
    QMap<int,int> biterrcode;
    biterrcode.insert(0,1002);
    biterrcode.insert(1,1022);
    biterrcode.insert(2,1003);
    biterrcode.insert(3,1023);
    biterrcode.insert(4,1004);
    biterrcode.insert(5,1024);
    biterrcode.insert(6,1005);
    biterrcode.insert(7,1025);
    m_ErrorNbr.append(biterrcode);
    biterrcode.clear();
    biterrcode.insert(0,1004);
    biterrcode.insert(1,1022);
    biterrcode.insert(2,1003);
    biterrcode.insert(3,1023);
    biterrcode.insert(4,1004);
    m_ErrorNbr.append(biterrcode);
    biterrcode.clear();
    biterrcode.insert(0,1062);
    biterrcode.insert(1,1064);
    biterrcode.insert(2,1068);
    biterrcode.insert(3,1063);
    biterrcode.insert(4,1065);
    biterrcode.insert(5,1067);
    m_ErrorNbr.append(biterrcode);
}

UpdateCarData::~UpdateCarData()
{

}
///
/// \brief UpdateCarData::SetCarParamByFrame
/// \param tempData
/// \param ID
/// \return
///
int UpdateCarData::SetCarParamByFrame(QByteArray dataframe, int ID,bool high)
{
    int count  =  0;
    for(int i = 0; i < dataframe.length();)
    {
        QByteArray tempData;
        if(i+10 > dataframe.length()) //数据帧长度至少10
        {
            return -1;
        }
        if(dataframe[i+4] == 'R' && dataframe[i+5] == 'B')//RB报文
        {
            ReceCarRBFrame RBstru;//复位报文
            memcpy((char*)&RBstru,dataframe.data()+i,10);
            ///qDebug()<<"收到RB报文:"<< RBstru.carnbr << RBstru.posinfo;
            tempData.append(dataframe.data()+i,sizeof(ReceCarcmdActionFrame));
            i = i + 10;
        }
        else if(dataframe[i+4] == 'S' && dataframe[i+5] == 'D')
        {
            if((i+74  >  dataframe.length())) //不正常的帧 不完整
            {
                return -1;
            }
            ReceCarDetailFrame detailstru;//详细数据报文
            memcpy((char*)&detailstru,dataframe.data()+i,74);
            //qDebug()<<"收到详细数据报文:"<< detailstru.carnbr << detailstru.state;
            tempData.append(dataframe.data()+i,sizeof(ReceCarDetailFrame));
            i = i + 74;
        }
        else if(dataframe[i+4] == 'S' && dataframe[i+5] == 'T')
        {
            //简易数据报文
            ReceCarcmdsimFrame simstru;
            memcpy((char*)&simstru,dataframe.data()+i,10);
            tempData.append(dataframe.data()+i,sizeof(ReceCarcmdsimFrame));
            i = i + 10;
        }
        else //默认是动作指令报文
        {
            ReceCarcmdActionFrame actionstru;//动作指令报文
            memcpy((char*)&actionstru,dataframe.data()+i,10);
            tempData.append(dataframe.data()+i,sizeof(ReceCarcmdActionFrame));
            i = i + 10;
        }
        if(high)
        {
            ReadSingleFrame(tempData,ID);
        }
        else
        {
            ReadSingleFrameVtest(tempData,ID);
        }
        count++;
    }
    if(count > 1/*|| (dataframe.length() != 74 && dataframe.length() != 10 */) //出现了粘包
    {
        QString str  = QString("小车ID：%1,发生时间：%2,出现了粘包次数 count : %3, length：%4").arg(ID).arg(QDateTime::currentDateTime().toString()).arg(count).arg(dataframe.length());
        //qDebug()<<str;
        GetSystemLogObj()->writeLog(str,2);
    }
    return 0;
}
///
/// \brief UpdateCarData::Updateconnectstate
/// \param id
/// \param value
///
void UpdateCarData::Updateconnectstate(int ID, int value)
{
    if(Myconfig::GetInstance()->m_CarMap[ID].deveceStatus.isOnline != value)
    {
        Myconfig::GetInstance()->m_CarMap[ID].deveceStatus.isOnline = value;
        QString online = QString("小车通讯状态发生变化：ID：%1,时间：%2，状态 %3").arg(ID).arg(QDateTime::currentDateTime().toString()).arg(value);
        GetSystemLogObj()->writeLog(online,2);
    }
}
///
/// \brief UpdateCarData::GetSendFrameIndex
/// \param ID
/// \param frametype
/// \return
///
int16_t UpdateCarData::GetSendFrameIndex(int ID, int frametype)
{
    Q_UNUSED(ID)
    int16_t index = 2001;
    switch (frametype) {
    case detailframe:
    {
        m_senddetailindex++;
        if(m_senddetailindex > 32700)//不能超过65535
        {
            m_senddetailindex = 2001;
        }
        if(m_senddetailindexlist.size() > 0)
        {

            //-----------------被迫不做校验---------------------
//            if(m_senddetailindexlist.size() > 30)
//            {
//                UpdateCarFunData(ID,requestError,4);

//                QString listindex;
//                for(int i = 0; i <m_senddetailindexlist.size(); ++i )
//                {
//                    listindex += QString::number(m_senddetailindexlist[i])+",";
//                }
//                listindex.chop(1);
//                QString errorinfostr = QString("ERROR:WCS发送给小车数据ID[%1],发生时间：%2，信息：数据已经有10条以上未回复，未回复的索引号:%3;").arg(ID).arg(QDateTime::currentDateTime().toString()).arg(listindex);
//                qDebug() << errorinfostr;
//                GetSystemLogObj()->writeLog(errorinfostr,2);
//                index = -1;
//                return index;
//            }
            //-----------------------------------------------


            // index = m_senddetailindexlist[m_senddetailindexlist.size() -1];
            //            if(m_senddetailindex > 65532)//不能超过65535
            //            {
            //                m_senddetailindex = 2001;
            //            }
            //            if(index == 1000) //特殊索引号不要使用
            //            {
            //                index = 1001;
            //            }
            //            if(index == 2000)//特殊索引号不要使用
            //            {
            //                index = 2001;
            //            }
            m_senddetailindexlist.append(static_cast<int16_t>(m_senddetailindex));
        }
        else{
            m_senddetailindexlist.append(static_cast<int16_t>(m_senddetailindex));
        }
        index = static_cast<int16_t>(m_senddetailindex);
    }
        break;
    case actionframe:
    {
        m_actiontindex++;
        if(m_actiontindex > 32700)//不能超过65535
        {
            m_actiontindex = 2001;
        }
        // int16_t index = 0;
        //        if(!m_Wcstocaractionframeindex.contains(hwId))
        //        {
        //            int wcsindex = 0;
        //            QList<int16_t> indexlsit;
        //            indexlsit.append(wcsindex);
        //            m_Wcstocaractionframeindex.insert(hwId,indexlsit);
        //        }
        //        else{
        //            QList<int16_t> list = m_Wcstocaractionframeindex[hwId];
        //            if(list.size() > 0)
        //            {
        //                index = list[list.size() -1];
        //                // return index;
        //            }
        //            else{
        //                int16_t endvalue  = 0;
        //                list.append(endvalue);
        //                index = endvalue;
        //            }
        //            m_Wcstocarframeindex[hwId] = list;
        //        }
        //qDebug() << "得到动作指令的索引号："<<index << hwId<< QDateTime::currentDateTime();
        index =static_cast<int16_t>(m_actiontindex);
    }
        break;
    default:
        break;
    }
    return index;
}
///
/// \brief UpdateCarData::ReSetIndex
/// \param ID
/// \param frametype
///
void UpdateCarData::ReSetIndex(int ID, int frametype)
{
    Q_UNUSED(ID)
    switch (frametype) {
    case detailframe:
    {
        m_senddetailindexlist.clear();
    }
        break;
    case actionframe:
        m_sendactionindexlist.clear();
        break;
    default:
        break;
    }
}

void UpdateCarData::ReadSingleFrame(QByteArray Data, int ID)
{
    QByteArray  tempData;
    tempData.append(Data);
    if(tempData.length() >= 10)
    {
        //分析报文类型
        int16_t nbr;//指令编号
        int16_t carnbr;//小车编号
        atoi16 nbrvalue;
        memcpy(nbrvalue.a,tempData.data(),2);
        nbr = nbrvalue.x;
        atoi16 carrvalue;
        memcpy(carrvalue.a,tempData.data()+2,2);
        carnbr = carrvalue.x;
        if(ID == carnbr)
        {
            if(nbr == 2000 && tempData[4] == 'R' && tempData[5] == 'B')//RB报文
            {
                ReceCarRBFrame RBstru;//复位报文
                memcpy((char*)&RBstru,tempData.data(),10);
            }
            else if(tempData[4] == 'S' && tempData[5] == 'D'&&(tempData.size() >= 74))//详细报文数据
            {
                if(CheckIndexIsOK(ID,detailframe,nbr))//正确报文 之前请求的数据已经返回了结果了
                {
                    //解析详细数据内容
                    ReceCarDetailFrame detailstru;//详细数据报文
                    memcpy((char*)&detailstru,tempData.data(),74);
                    UpdateCarFunData(ID,Opermode,detailstru.state);//自动 / 手动
                    UpdateCarFunData(ID,exestatus,detailstru.info.carinfo);// 电量 校准  就绪 等状态变化
                    UpdateCarFunData(ID,sensorstat,detailstru.statepic.goodsstate.carsensorstat);//货物状态变化情况
                    QByteArray errorstr;
                    errorstr.append(reinterpret_cast<char*>(&detailstru.errorpic),4); //插入字节信息
                    UpdateCarFunData(ID,Errorcode,GetErrorcodeByByte(detailstru.errorpic));//故障代码值更新 用前4个字节的故障位图表示
                    GetErrorcode(errorstr);//故障代码值更新 用前4个字节的故障位图表示
                }
                else{
                    qDebug()<<"详细数据报文回复索引号不对:"<< ID << nbr << QDateTime::currentDateTime();
                    QString str = "详细数据报文 回复索引号不对:"+ QString::number(ID)  + QString::number(nbr) + QDateTime::currentDateTime().toString();
                    GetSystemLogObj()->writeLog(str,2);
                }
            }
            else if(tempData[4] == 'S' && tempData[5] == 'T')
            {
                //简易数据报文
                ReceCarcmdsimFrame simstru;
                memcpy((char*)&simstru,tempData.data(),10);
                //if(CheckIndexIsOK(ID,detailframe,nbr))//正确报文 之前请求的数据已经返回了结果了
                {
                    UpdateCarFunData(ID,Opermode,simstru.carstate);//自动 / 手动
                    UpdateCarFunData(ID,exestatus,simstru.info.carinfo);// 电量 校准  就绪 等状态变化
                }
               // else
                {
                    if(nbr == 1000)
                    {
                        //小车自动得到指令动作回复
                        UpdateCarFunData(ID,Opermode,simstru.carstate);//自动 / 手动
                        UpdateCarFunData(ID,exestatus,simstru.info.carinfo);// 电量 校准  就绪 等状态变化
                    }
                }
            }
            else
            {
                ReceCarcmdActionFrame actionstru;//动作指令报文
                memcpy((char*)&actionstru,tempData.data(),10);
                //  if(actionstru.cmdstate != 1 )
                {
                    //if(ModifyCarReceFrameIndex(ID,nbr)) //回应发出去编号内容  执行中 异常 完成 等指令
                    {
                        UpdateCarFunData(ID,actioninfo,actionstru.cmdstate);
                        //qDebug()<<"动作指令报文:"<< actionstru.carnbr <<actionstru.cmdname<<  actionstru.cmdstate<<actionstru.cmdnbr;
                    }
                    //                            else{
                    //                                QString str = "回复指令 异常 完成，索引号不在 错误:"+ QString::number(ID)  + QString::number(nbr) + QString::number(actionstru.cmdstate)+ QDateTime::currentDateTime().toString();
                    //                                GetSystemLogObj()->writeLog(str,2);
                    //                                qDebug()<<"回复指令 异常 完成，索引号不在 错误:"<< ID << nbr << QDateTime::currentDateTime()<<actionstru.cmdstate;
                    //                            }
                }
                //                        else{
                //                            if(actionstru.cmdstate == 1)
                //                            {
                //                                UpdateCarStatus(ID,actioninfo,actionstru.cmdstate);
                //                            }
                //                            if(m_Wcstocarframeindex.contains(ID))
                //                            {
                //                                int index  = m_Wcstocarframeindex[ID].indexOf(nbr);
                //                                if(index < 0)
                //                                {
                //                                    qDebug()<<"actionstru.cmdstate执行中 索引号不在 错误指令:"<< ID << nbr <<QDateTime::currentDateTime();
                //                                    QString str = "actionstru.cmdstate执行中 索引号不在 错误指令:"+ QString::number(ID)  + QString::number(nbr) + QString::number(actionstru.cmdstate)+ QDateTime::currentDateTime().toString();
                //                                    GetSystemLogObj()->writeLog(str,2);
                //                                }
                //                            }
                //                        }
            }
        }
    }
}

void UpdateCarData::ReadSingleFrameVtest(QByteArray Data, int ID)
{
    QByteArray  tempData;
    tempData.append(Data);
    if(tempData.length() >= 10)
    {
        //分析报文类型
        int16_t nbr;//指令编号
        int16_t carnbr;//小车编号
        atoi16 nbrvalue;
        // memcpy(nbrvalue.a,tempData.data(),2);
        nbrvalue.a[0] = tempData[1];
        nbrvalue.a[1] = tempData[0];
        tempData[0] =  nbrvalue.a[0];
        tempData[1] =  nbrvalue.a[1];
        nbr = nbrvalue.x;
        atoi16 carrvalue;
        // memcpy(carrvalue.a,tempData.data()+2,2);
        carrvalue.a[0]= tempData[3];
        carrvalue.a[1]= tempData[2];
        tempData[2] =  carrvalue.a[0];
        tempData[3] =  carrvalue.a[1];
        carnbr = carrvalue.x;
        if(ID == carnbr)
        {
            if(nbr == 2000 && tempData[4] == 'R' && tempData[5] == 'B')//RB报文
            {
                ReceCarRBFrame RBstru;//复位报文
                char state1 = tempData[6];
                char pos1 = tempData[7];
                tempData[6] = tempData[9];
                tempData[7] = tempData[8];
                tempData[8] = pos1;
                tempData[9] = state1;
                 memcpy((char*)&RBstru,tempData.data(),10);
                qDebug()<<"car id:"<< ID << "rec [RB] frame:"<<tempData.toHex() << "rb value:"<< RBstru.posinfo;
                UpdateCarFunData(ID,Updateposinfo,RBstru.posinfo);
            }
            else if(tempData[4] == 'S' && tempData[5] == 'D'&&(tempData.size() >= 74))//详细报文数据
            {
                if(CheckIndexIsOK(ID,detailframe,nbr))//正确报文 之前请求的数据已经返回了结果了
                {
                    //解析详细数据内容
                    ReceCarDetailFrame detailstru;//详细数据报文
                    char state = tempData[6];
                    tempData[6] = tempData[7];
                    tempData[7] = state;
                    //                    char carinfo = tempData[8];
                    //                    tempData[8] = tempData[9];
                    //                    tempData[9] = carinfo;
                    memcpy((char*)&detailstru,tempData.data(),74);
                    //change byte
                    UpdateCarFunData(ID,Opermode,detailstru.state);//自动 / 手动
                    UpdateCarFunData(ID,exestatus,detailstru.info.carinfo);// 电量 校准  就绪 等状态变化
                    UpdateCarFunData(ID,sensorstat,detailstru.statepic.goodsstate.carsensorstat);//货物状态变化情况
                    for(int i = 0; i < 37;++i)
                    {
                        if(detailstru.errorpic[i].errcodestate != 0)
                        {
                            QString str = "小车ID："+QString::number(ID)+"时间---"+QDateTime::currentDateTime().toString()+\
                                    "收到小车的故障字节位置是："+QString::number(i)+"字节的值是："+QString::number(detailstru.errorpic[i].errcodestate);
                            GetSystemLogObj()->writeLog(str,2);

                        }
                    }

                }
                else{
                    qDebug()<<"详细数据报文 回复索引 号不对:"<< ID << nbr << QDateTime::currentDateTime();
                    QString str = "详细数据报文 回复索引 号不对:"+ QString::number(ID)  + QString::number(nbr) + QDateTime::currentDateTime().toString();
                    GetSystemLogObj()->writeLog(str,2);
                }
            }
            else if(tempData[4] == 'S' && tempData[5] == 'T')
            {
                //简易数据报文
                ReceCarcmdsimFrame simstru;
                char state = tempData[6];
                tempData[6] = tempData[7];
                tempData[7] = state;
                //                char carinfo = tempData[8];
                //                tempData[8] = tempData[9];
                //                tempData[9] = carinfo;
                memcpy((char*)&simstru,tempData.data(),10);
                //if(CheckIndexIsOK(ID,detailframe,nbr))//正确报文 之前请求的数据已经返回了结果了
                {
                    UpdateCarFunData(ID,Opermode,simstru.carstate);//自动 / 手动
                    UpdateCarFunData(ID,exestatus,simstru.info.carinfo);// 电量 校准  就绪 等状态变化
                }
                //  else
                {
                    if(nbr == 1000)
                    {
                        //小车自动得到指令动作回复
                        UpdateCarFunData(ID,Opermode,simstru.carstate);//自动 / 手动
                        UpdateCarFunData(ID,exestatus,simstru.info.carinfo);// 电量 校准  就绪 等状态变化
                    }
                }
            }
            else
            {
                ReceCarcmdActionFrame actionstru;//动作指令报文
                char  cmd =  tempData[4];
                tempData[4] =  tempData[5];
                tempData[5] = cmd;
                char state = tempData[6];
                tempData[6] = tempData[7];
                tempData[7] = state;
                char carinfo = tempData[8];
                tempData[8] = tempData[9];
                tempData[9] = carinfo;
                memcpy((char*)&actionstru,tempData.data(),10);
                //  if(actionstru.cmdstate != 1 )
                {
                    //if(ModifyCarReceFrameIndex(ID,nbr)) //回应发出去编号内容  执行中 异常 完成 等指令
                    {
                        UpdateCarFunData(ID,actioninfo,actionstru.cmdstate);
                        //qDebug()<<"动作指令报文:"<< actionstru.carnbr <<actionstru.cmdname<<  actionstru.cmdstate<<actionstru.cmdnbr;
                    }
                    //                            else{
                    //                                QString str = "回复指令 异常 完成，索引号不在 错误:"+ QString::number(ID)  + QString::number(nbr) + QString::number(actionstru.cmdstate)+ QDateTime::currentDateTime().toString();
                    //                                GetSystemLogObj()->writeLog(str,2);
                    //                                qDebug()<<"回复指令 异常 完成，索引号不在 错误:"<< ID << nbr << QDateTime::currentDateTime()<<actionstru.cmdstate;
                    //                            }
                }
                //                        else{
                //                            if(actionstru.cmdstate == 1)
                //                            {
                //                                UpdateCarStatus(ID,actioninfo,actionstru.cmdstate);
                //                            }
                //                            if(m_Wcstocarframeindex.contains(ID))
                //                            {
                //                                int index  = m_Wcstocarframeindex[ID].indexOf(nbr);
                //                                if(index < 0)
                //                                {
                //                                    qDebug()<<"actionstru.cmdstate执行中 索引号不在 错误指令:"<< ID << nbr <<QDateTime::currentDateTime();
                //                                    QString str = "actionstru.cmdstate执行中 索引号不在 错误指令:"+ QString::number(ID)  + QString::number(nbr) + QString::number(actionstru.cmdstate)+ QDateTime::currentDateTime().toString();
                //                                    GetSystemLogObj()->writeLog(str,2);
                //                                }
                //                            }
                //                        }
            }
        }
    }
}
///
/// \brief UpdateCarData::UpdateCarFunData
/// \param ID
/// \param role
/// \param Value
/// \param type
///
void UpdateCarData::UpdateCarFunData(int carID, int role, int value, int type )
{
    Q_UNUSED(type)
    QMutexLocker locker(&Myconfig::GetInstance()->m_carMap_mutex);
    if(Myconfig::GetInstance()->m_CarMap.contains(carID))
    {
        switch (role) {
        case UpdateCarStatusrole::RBposinfo:
            break;
        case UpdateCarStatusrole::Opermode:
        {
            //1:手动 2：自动
            if(Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.model != value)
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.model = value;
            }
            break;
        }
        case UpdateCarStatusrole::sensorstat://传感器sensor状态
        {
            int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.senorgoodsstru.carsensorstat;
            if(curvalue != value)
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.senorgoodsstru.carsensorstat = static_cast<int8_t>(value);
            }
            break;
        }
        case UpdateCarStatusrole::exestatus: //包含了状态信息 故障等信息
        {
            int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.statusinfodstru.carstatusinfo;
            if(curvalue != value )
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.statusinfodstru.carstatusinfo = static_cast<int8_t>(value);
                if(Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.statusinfodstru.berror != 0)
                {
                    QString str = "检测到故障位了 id:"+ QString::number(carID)  + QDateTime::currentDateTime().toString() ;
                    GetSystemLogObj()->writeLog(str,2);
                }

            }
            break;
        }
        case UpdateCarStatusrole::actioninfo://指令完成动作赋值
        {
            int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.readonlystatus;
            if(curvalue != value)
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.readonlystatus = value;
                //qDebug()<<"小车ID："<<carID <<",小车动作状态改变信息:"<<QDateTime::currentDateTime()<<"value:"<< value;
            }
            break;
        }
        case UpdateCarStatusrole::requestError:// 指令请求异常
        {
            int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.status;
            if(curvalue != value)
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.status = value;
                QString str = QString("小车ID:[%1],发生时间:%2,通信协议出现请求异常,更改小车状态的值：%3;").arg(carID).arg(QDateTime::currentDateTime().toString()).arg(value);
                qDebug()<<str;
                GetSystemLogObj()->writeLog(str,2);
            }
            break;
        }
        case UpdateCarStatusrole::Errorcode:// 故障代码
        {
            int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.err_code;
            if(curvalue != value)
            {
                Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.err_code = value;
                qDebug()<<"car id:"<<carID<<"err_code:"<< value;
                QString str = "小车故障了 id:"+ QString::number(carID)  + QDateTime::currentDateTime().toString()+ "errcode:"+QString::number(value) ;
                GetSystemLogObj()->writeLog(str,2);
            }
            break;
        }
        case UpdateCarStatusrole::Updateposinfo:// RB info updatepos
        {

            if(Myconfig::GetInstance()->m_codeResiveMap.contains(value))
            {
                  int curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.x;
                  int setbvalue = Myconfig::GetInstance()->m_codeResiveMap[value].x;
                  if(curvalue != setbvalue)
                  {
                      Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.x = setbvalue;
                      qDebug()<<"car id:"<<carID<<"RBPOS[X]:"<< setbvalue;
                  }
                   curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.y;
                   setbvalue = Myconfig::GetInstance()->m_codeResiveMap[value].y;
                   if(curvalue != setbvalue)
                   {
                       Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.y = setbvalue;
                       qDebug()<<"car id:"<<carID<<"RBPOS[Y]:"<< setbvalue;
                   }
                   curvalue = Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.z;
                   setbvalue = Myconfig::GetInstance()->m_codeResiveMap[value].z;
                   if(curvalue != setbvalue)
                   {
                       Myconfig::GetInstance()->m_CarMap[carID].deveceStatus.carCurrentPosion.z = setbvalue;
                       qDebug()<<"car id:"<<carID<<"RBPOS[Z]:"<< setbvalue;
                   }
            }
            break;
        }
        default:
            break;
        }
    }
}
///
/// \brief UpdateCarData::CheckIndexIsOK
/// \param ID
/// \param frametype
/// \param nbr
/// \return
///检查编号是否存在
bool UpdateCarData::CheckIndexIsOK(int ID, int frametype, int16_t nbr)
{
    Q_UNUSED(ID)
    bool isok = false;
    switch (frametype) {
    case detailframe:
    {
        int index  = m_senddetailindexlist.indexOf(nbr);
        if(index >= 0)
        {
            m_senddetailindexlist.removeAt(index);
            isok = true;
        }
    }
        break;
    case actionframe:
        break;
    default:
        break;
    }
    return isok;
}
///
/// \brief UpdateCarData::GetErrorcode
/// \param array
/// \return
/// 字节数组与 故障编码转化 故障编码位图与编号的
int UpdateCarData::GetErrorcode(QByteArray array)
{
    union  autoi{
        int8_t a[4];
        int value ;
    };
    autoi bytearray;
    memcpy(bytearray.a,array.data(),4);
    if(bytearray.value != 0)//故障状态 显示
    {
        qDebug()<<"故障位图显示异常，当前转化成int之后的值是："<<bytearray.value;
    }
    return  bytearray.value;
}
///
/// \brief UpdateCarData::GetErrorcodeByByte
/// \param errorpic
/// \return
///根据故障位图 得到对应的故障编号,暂时处理前3个字节信息的值
int UpdateCarData::GetErrorcodeByByte(ERRORCODESTATESTRU *errorpic)
{
    int code = 0;
    //提取前三个字节信息进行显示
    ERRORCODESTATESTRU array[3];
    memcpy(array,errorpic,3);
    for(int8_t i = 0; i < 3;++i)
    {
        if(array[i].errcodestate != 0)
        {
            //判断是哪一个bit位有变化
            for(int8_t  j = 0; j < 8;++j)
            {
                if((array[i].errcodestate&(1<j)) != 0)
                {
                    code = GetDefineNbr(i,j);
                    qDebug()<<"小车故障位图变化了:"<< "字节位置："<< i <<"bit pos:"<< j;
                    break;
                }
            }
             break;
        }
    }
    return code;
}
///
/// \brief UpdateCarData::GetDefineNbr
/// \param bytepos
/// \param bitpos
/// \return
///根据存储的故障字典查到编号显示在最新界面上
int UpdateCarData::GetDefineNbr(char bytepos, char bitpos)
{
    int nbr = 0;
    if(m_ErrorNbr.size() < bytepos)
    {
        if(m_ErrorNbr[bytepos].contains(bitpos))
        {
            nbr = m_ErrorNbr[bytepos][bitpos];
        }
    }
    return nbr;
}
