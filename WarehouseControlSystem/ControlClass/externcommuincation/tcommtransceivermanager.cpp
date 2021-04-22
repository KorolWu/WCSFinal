
#include "tcommtransceivermanager.h"
#include "UnitClass/c_systemlogmng.h"

TCommtransceivermanager::TCommtransceivermanager()
{
    m_updatecarMap.clear();
    m_updateRunnerMap.clear();
    m_updateElevatorMap.clear();
    m_updateScanBarCodeMap.clear();
    m_high = true;
}

TCommtransceivermanager::~TCommtransceivermanager()
{
    m_HWdeviceMap.clear();
    m_updatecarMap.clear();
    m_updateRunnerMap.clear();
    m_updateElevatorMap.clear();
    m_updateScanBarCodeMap.clear();
}///
/// \brief TCommtransceivermanager::InitHWcommob
///根据数据库内容创建所有通讯的对象，并存储到内存中
void TCommtransceivermanager::InitHWcommob()
{
    //读取内存的数据进行 创建通讯对象
    m_HWdeviceMap.clear();
    if(Myconfig::GetInstance()->m_byteorder != "high")
    {
        m_high = false;
    }
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.begin(); it !=Myconfig::GetInstance()->m_hwcommstru.hwTcpMap.end(); ++it  )
    {
        if(it.value().hwtype == RGVCAR && it.value().protype == KTcpClient)
        {
            UpdateCarData *carupdateob = new UpdateCarData();
            m_updatecarMap.insert(it.key(),carupdateob);
        }
        if(it.value().hwtype == RUNNER)
        {
            UpdateRunnerData *runnerupdateob = new UpdateRunnerData();
            m_updateRunnerMap.insert(it.key(),runnerupdateob);
        }
        if(it.value().hwtype == STACKER && it.value().protype == KOPCUAClient)
        {
            //堆垛机 对象数据的更新
            UpdateStackerData *stackerob = new UpdateStackerData();
            m_updatestackerMap.insert(it.key(),stackerob);
        }
    }
    for(auto it = Myconfig::GetInstance()->m_hwcommstru.hwSerialPortMap.begin(); \
        it !=Myconfig::GetInstance()->m_hwcommstru.hwSerialPortMap.end(); ++it  )
    {
        if(it.value().hwtype == ELEVATOR_CAR)
        {
            UpdateElevatorData *Elevatorupdateob = new UpdateElevatorData();
            m_updateElevatorMap.insert(it.key(),Elevatorupdateob);
        }
        if(it.value().hwtype == BARCODE)
        {
            ScanBarCode *scanbarupdateob = new ScanBarCode();
            m_updateScanBarCodeMap.insert(it.key(),scanbarupdateob);
        }
    }
    //    for(auto it = Myconfig::GetInstance()->m_ScanMap.begin();
    //        it !=Myconfig::GetInstance()->m_ScanMap.end(); ++it  )
    //    {
    //        //if(it.value()->GetHWtype() == BARCODE)
    //        {
    //            ScanBarCode *scanbarupdateob = new ScanBarCode();
    //            m_updateScanBarCodeMap.insert(it.key(),scanbarupdateob);
    //        }
    //    }
    CreatObbyHWconfigData(Myconfig::GetInstance()->m_hwcommstru.hwTcpMap,KTcpClient);
    CreatObbyHWconfigData(Myconfig::GetInstance()->m_hwcommstru.hwmodbustcpcliMap,KModbusTcpClient);
    CreatObbyHWconfigData(Myconfig::GetInstance()->m_hwcommstru.hwSerialPortMap,KSerialPort);
}
///
/// \brief TCommtransceivermanager::ResetHWcommob
/// \param Id
///请求通讯异常回复复位的指令针对
void TCommtransceivermanager::ResetHWcommob(int Id)
{
    QMutexLocker locker(&m_TCommMutex);
    //Q_UNUSED(Id)
    if(!m_updatecarMap.contains(Id))
    {
        return;
    }
    if(m_HWdeviceMap[Id]->m_connectstate == -999) ///针对小车
    {
        if(m_updatecarMap.contains(Id))
        {
            m_updatecarMap[Id]->ReSetIndex(Id,detailframe); //累积的索引号进行clear
            m_HWdeviceMap[Id]->m_connectstate = 1;//回复之前的连接状态
        }
    }
}
///
/// \brief TCommtransceivermanager::SendDatatoCarHW
/// \param cmd
/// \param hwId
///
void TCommtransceivermanager::SendDatatoCarHW(OrderStru cmd, int hwId)
{
    int16_t wcsindex = 0;
    QByteArray frameData;
    int16_t childtype = 0;
    childtype = cmd.childtype;
    int protype = m_HWdeviceMap[hwId]->GetHWprotype();
    switch (childtype) {
    case 5: // 发送动作指令请求
    {
        if(!m_updatecarMap.contains(hwId))
        {
            return;
        }
        wcsindex = m_updatecarMap[hwId]->GetSendFrameIndex(hwId,1);
        SendCarCmdFrame wcssendframestru;
        wcssendframestru.cmdnbr = wcsindex;//指令编号:识别不同报文的唯一编号,该序号由WCS提供。

        wcssendframestru.carnbr = static_cast<int16_t>(hwId);
        if(cmd.order == Left_Pickup)
        {
            wcssendframestru.pickputdirection = 1;//左取货
            wcssendframestru.cmdname = 3;//左取货
        }
        else if(cmd.order == Right_Pickup || cmd.order == Right_WorkBin)
        {
            wcssendframestru.pickputdirection = 2;//右取货
            wcssendframestru.cmdname = 3;//取货
        }
        else if(cmd.order == Left_Putinto|| cmd.order == Left_WorkBin)
        {
            wcssendframestru.pickputdirection = 3;//左放货
            wcssendframestru.cmdname = 4;//放货
        }
        else if(cmd.order == Right_Putinto)
        {
            wcssendframestru.pickputdirection = 4;//右放货
            wcssendframestru.cmdname = 4;//放货
        }
        else if(cmd.order == Y||cmd.order == Elevator_In || cmd.order == Elevator_Out)
        {
            wcssendframestru.cmdname = 1; //直行距离
            wcssendframestru.Strdistance =  static_cast<int64_t>(cmd.value);
        }
        else if(cmd.order == X )
        {
            wcssendframestru.cmdname = 2;//横行距离
            wcssendframestru.Tradistance = static_cast<int64_t>(cmd.value);
        }
        frameData.append(reinterpret_cast<char*>(&wcssendframestru),sizeof(SendCarCmdFrame));
        break;
    }
    case 6://定时请求读小车数据指令
    {
        if(!m_updatecarMap.contains(hwId))
        {
            return;
        }
        wcsindex = m_updatecarMap[hwId]->GetSendFrameIndex(hwId,0);
        if(wcsindex < 0)
        {
            if( m_HWdeviceMap[hwId]->m_connectstate != -999)
            {
                QString errorstr = QString("小车ID：%1,已经超过10条以上未回复了，时间%2;").arg(hwId).arg(QDateTime::currentDateTime().toString());
                frameData.append(errorstr);
                qDebug()<<"小车ID："<< hwId<<",索引号-1改变:之前的状态值："<< m_HWdeviceMap[hwId]->m_connectstate <<"发生时间"<<QDateTime::currentDateTime().toString();
                m_HWdeviceMap[hwId]->m_connectstate = -999;
                // emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(frameData);//发送报文
            }
            return;
        }
        //wcsindex = GetWCStocarFrameIndex(hwId);
        SendCarCmdReqestFrame simplestru;
        simplestru.carnbr = static_cast<int16_t>(hwId);
        simplestru.cmdnbr = wcsindex;
        simplestru.cmdname =  static_cast<int16_t>(cmd.value); // 请求指令数据 5 或者 6
        frameData.append(reinterpret_cast<char*>(&simplestru),sizeof(SendCarCmdReqestFrame));
        break;
    }
    default:
        break;
    }
    //WCS发送数据报文到小车
    if(frameData.size() > 0 && protype == KTcpClient )
    {
        emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(frameData);//发送报文
    }
}

void TCommtransceivermanager::SendDatatoCarV1test(OrderStru cmd, int hwId)
{
    int16_t wcsindex = 0;
    QByteArray frameData;
    int16_t childtype = 0;
    childtype = cmd.childtype;
    int protype = m_HWdeviceMap[hwId]->GetHWprotype();
    switch (childtype) {
    case 5: // 发送动作指令请求
    {
        if(!m_updatecarMap.contains(hwId))
        {
            return;
        }
        wcsindex = m_updatecarMap[hwId]->GetSendFrameIndex(hwId,1);
        SendCarCmdFrame wcssendframestru;
        wcssendframestru.cmdnbr = wcsindex;//指令编号:识别不同报文的唯一编号,该序号由WCS提供。

        wcssendframestru.carnbr =static_cast<int16_t>(hwId);
        if(cmd.order == Left_Pickup)
        {
            wcssendframestru.pickputdirection = 1;//左取货
            wcssendframestru.cmdname = 3;//左取货
        }
        else if(cmd.order == Right_Pickup || cmd.order == Right_WorkBin)  // get box same direction
        {
            wcssendframestru.pickputdirection = 2;//右取货
            wcssendframestru.cmdname = 3;//取货
        }
        else if(cmd.order == Left_Putinto)
        {
            wcssendframestru.pickputdirection = 3;//左放m_HWdeviceMap货
            wcssendframestru.cmdname = 4;//放货
        }
        else if(cmd.order == Right_Putinto || cmd.order == Left_WorkBin)
        {
            wcssendframestru.pickputdirection = 4;//右放货
            wcssendframestru.cmdname = 4;//放货
        }
        else if(cmd.order == Y||cmd.order == Elevator_In || cmd.order == Elevator_Out)
        {
            wcssendframestru.cmdname = 1; //直行距离
            wcssendframestru.Strdistance = static_cast<int16_t>(cmd.value);
        }
        else if(cmd.order == X )
        {
            wcssendframestru.cmdname = 2;//横行距离
            wcssendframestru.Tradistance = static_cast<int16_t>(cmd.value);
        }
        frameData.append(reinterpret_cast<char*>(&wcssendframestru),sizeof(SendCarCmdFrame));
        char nbr = frameData[0];
        frameData[0] = frameData[1];
        frameData[1] = nbr;
        char carnbr = frameData[2];
        frameData[2] = frameData[3];
        frameData[3] = carnbr;
        char cmdname = frameData[4];
        frameData[4] = frameData[5];
        frameData[5] = cmdname;
        char value[8] = {0};
        memcpy(value,frameData.data()+12,8);
        int j = 0;
        for(int i = 7 ;i >= 0;--i)
        {
            frameData[12+j] = value[i];
            j++;
        }
        memcpy(value,frameData.data()+20,8);
        j = 0;
        for(int i = 7 ;i >= 0;--i)
        {
            frameData[20+j] = value[i];
            j++;
        }
        char direction = frameData[32];
        frameData[32] = frameData[33];
        frameData[33] = direction;
        //WCS发送数据报文到小车
        if(frameData.size() > 0 && protype == KTcpClient )
        {
            qDebug()<<"send action cmd :"<<frameData.toHex() <<QDateTime::currentDateTime().toString();
            emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(frameData);//发送报文
        }
        break;
    }
    case 6://定时请求读小车数据指令
    {
        if(!m_updatecarMap.contains(hwId))
        {
            return;
        }
        wcsindex = m_updatecarMap[hwId]->GetSendFrameIndex(hwId,0);
        if(wcsindex < 0)
        {
            m_HWdeviceMap[hwId]->m_connectstate = -999;
            QString errorstr = QString("小车ID：%1,已经超过10条以上未回复了，时间%2;").arg(hwId).arg(QDateTime::currentDateTime().toString());
            frameData.append(errorstr);
            // emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(frameData);//发送报文
            return;
        }
        //wcsindex = GetWCStocarFrameIndex(hwId);
        SendCarCmdReqestFrame simplestru;
        simplestru.carnbr = static_cast<int16_t>(hwId);
        simplestru.cmdnbr = wcsindex;
        simplestru.cmdname = static_cast<int16_t>(cmd.value); // 请求指令数据 5 或者 6
        frameData.append(reinterpret_cast<char*>(&simplestru),sizeof(SendCarCmdReqestFrame));
        QByteArray temframeData;
        temframeData.append(frameData[1]);
        temframeData.append(frameData[0]);
        temframeData.append(frameData[3]);
        temframeData.append(frameData[2]);
        temframeData.append(frameData[5]);
        temframeData.append(frameData[4]);
        for(int i = 0; i < 34;++i)
        {
            char a = 0;
            temframeData.append(a);
        }
        //WCS发送数据报文到小车
        if(frameData.size() > 0 && protype == KTcpClient )
        {
            emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(temframeData);//发送报文
        }
        break;
    }
    default:
        break;
    }
}
///
/// \brief TCommtransceivermanager::SendDatatoRunnerHW
/// \param cmd
/// \param hwId
///
void TCommtransceivermanager::SendDatatoRunnerHW(OrderStru cmd, int hwId)
{
    int protype = m_HWdeviceMap[hwId]->GetHWprotype();
    if(protype == kMCMelsecTCPClient) //KModbusTcpClient 暂时替换为MC协议测试 OLD 的方式
    {
        TCommMCMelsecTCPclient *MCTcpob = dynamic_cast<TCommMCMelsecTCPclient *>(m_HWdeviceMap[hwId]);
        // if(cmd.order.call_Runner_Putbox)
        if(cmd.childtype == 5) //请求读数据
        {
            emit  MCTcpob->signalReadData(cmd.Datatype,cmd.startaddress,cmd.numberOfEntries);
        }
        else{ //请求写数据
            emit  MCTcpob->signalWriteData(cmd.Datatype,cmd.startaddress,cmd.values);
        }
    }
    //新增TCPclient方式发送数据pi
    if(protype == KTcpClient)
    {
        if(!m_updateRunnerMap.contains(hwId))
        {
            return;
        }
        QByteArray frameData;
        if(cmd.childtype == 5) //发送批量读指令的报文数据内容
        {
            WCSsendStatetRequestoRunnerStru sendstatestru;
            sendstatestru.cmdname = 0x05;
            sendstatestru.runnernbr = static_cast<uint16_t>(hwId);
            sendstatestru.cmdtype[0] = 'S';
            sendstatestru.cmdtype[1] = 'D';

            if(m_updateRunnerMap[hwId]->GetSendCmdIndex(hwId,0,sendstatestru.cmdnbr))
            {
                frameData.append(reinterpret_cast<char*>(&sendstatestru),sizeof(WCSsendStatetRequestoRunnerStru));

            }
        }
        else
        {
            //发送动作指令数据的赋值
            WCSsendActiontoRunnerStru sendacstru;
            sendacstru.runnernbr = static_cast<uint16_t>(hwId);
            sendacstru.cmdtype[0] = 'S';
            sendacstru.cmdtype[1] = 'A';
            size_t size = sizeof(cmd.box_num);
            if(size <= 10 && size > 0)
            {
                memcpy(sendacstru.boxinfo,cmd.box_num.toStdString().c_str(),size);
            }
            uint16_t cmdvalue = GetRunnerCmdValue(cmd) ;
            if( cmdvalue >  0)
            {
                sendacstru.cmdname = cmdvalue;
            }

            if(m_updateRunnerMap[hwId]->GetSendCmdIndex(hwId,1,sendacstru.cmdnbr))
            {
                frameData.append(reinterpret_cast<char*>(&sendacstru),sizeof(WCSsendActiontoRunnerStru));
            }
        }
        if(frameData.size() > 0)
        {
            //高字节转化函数
            RunnerHighByteConversion(frameData);
            emit m_HWdeviceMap[hwId]->signalSendHWdeviceData(frameData);//发送报文给流道
        }
    }
}

uint16_t TCommtransceivermanager::GetRunnerCmdValue(OrderStru cmd)
{
    uint16_t value = 0;
    switch (cmd.order)
    {
    case Runner_Start:
    {
        value = 1;
        break;
    }
    case Runner_Stop:
    {
        value = 2;
        break;
    }
    case Runner_Scancode1_Out:
    {
        value = 3;
        break;
    }
    case Runner_Scancode2_In:
    {
        value = 4;
        break;
    }
    case Runner_CarPickFinished:
    {
        value = 5;
        break;
    }
    case Runner_CarputFinished:
    {
        value = 6;
        break;
    }
    case Runner_CarputcheckFinished:
    {
        value = 7;
        break;
    }
    case Runner_InPushBox:
    {
        value = 10;
        break;
    }
    default:
        break;
    }
    return  value;
}

void TCommtransceivermanager::RunnerHighByteConversion(QByteArray &frame)
{
    if(frame.length() < 8)
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
    if(frame.length() == 40)
    {
        value = frame[8];
        frame[8] = frame[11];
        frame[11] = value;
        value = frame[9];
        frame[9] = frame[10];
        frame[10] = value;
    }
}
///
/// \brief TCommtransceivermanager::SendcommandByExtern
/// \param cmd
/// \param Id
///外部传入参数进行处理 小车 流道 电梯指令 等
//uint Sendcommand= 0;
void TCommtransceivermanager::SendcommandByExtern(OrderStru cmd, int hwId)
{
    QMutexLocker locker(&m_TCommMutex);
    //先解析小车部分数据 发送帧格式内容
    if(m_HWdeviceMap.contains(hwId))
    {
        if(m_HWdeviceMap[hwId]->m_connectstate  <= 0 )//10条以上未回复，会改变这个状态
        {
            //qDebug()<<"通讯异常中断 或者指令异常:"<< hwId << m_HWdeviceMap[hwId]->m_connectstate ;
            return;
        }
        int hwtype = m_HWdeviceMap[hwId]->GetHWtype();
        int protype = m_HWdeviceMap[hwId]->GetHWprotype();
        switch (hwtype) {
        case HWDEVICETYPE::RGVCAR://需要发送的是AGV小车的内容
        {
            if(m_high)
            {
                SendDatatoCarHW(cmd,hwId);
            }
            else//false
            {
                SendDatatoCarV1test(cmd,hwId);
            }
            break;
        }
        case HWDEVICETYPE::ELEVATOR_OUT:
            break;
        case HWDEVICETYPE::ELEVATOR_IN:
            break;
        case HWDEVICETYPE::ELEVATOR_CAR://小车电梯
        {
            if (protype == KFx3uSerialport)
            {
                TCommFx3uSerialport * modbusSerialob = dynamic_cast<TCommFx3uSerialport *>(m_HWdeviceMap[hwId]);
                if(cmd.childtype == 5) //请求读数据
                {
                    emit  modbusSerialob->signalReadData(cmd.Datatype,cmd.startaddress,cmd.numberOfEntries);
                }
                else{ //请求写数据
                    emit  modbusSerialob->signalWriteData(cmd.Datatype,cmd.startaddress,cmd.values);
                }
            }
            break;
        }
        case HWDEVICETYPE::RUNNER://流道指令调用
        {
            SendDatatoRunnerHW(cmd,hwId);
            break;
        }
        case HWDEVICETYPE::BARCODE://串口协议
        {
            //扫码协议需要发送数据内容
            if(protype == KSerialPort)
            {
                TcommSerialport * Serialob = dynamic_cast<TcommSerialport *>(m_HWdeviceMap[hwId]);
                QByteArray requestScanarray;
                requestScanarray.append(0x1B);
                requestScanarray.append(0x31);
                emit Serialob->signalSendHWdeviceData(requestScanarray);
            }
            break;
        }
        case HWDEVICETYPE::STACKER://opc协议
        {
            //扫码协议需要发送数据内容
            if(protype == KOPCUAClient)
            {
                TCommOPCUAClient * opcua = dynamic_cast<TCommOPCUAClient *>(m_HWdeviceMap[hwId]);
                if(!opcua)
                    return;
                if(cmd.childtype == 5)//批量读数据
                {
                    emit opcua->SignalBatchRead(cmd.Rcmdstackernames);
                }
                else{
                    //批量写数据
                    if(cmd.order == Stacker_Carry)
                    {
                        //插入执行任务索引号
                        OrderStru stru;
                        stru.childtype = 1;
                        stru.Datatype = QOpcUa::Types::Int32;;
                        QMap<QString,QVariant> valuemap;
                        valuemap.insert("ns=3;s=\"F_WCS_DB\".\"TaskNO\"",m_updatestackerMap[hwId]->GetTaskIndex());
                        stru.WcmdstackerValues = valuemap;
                        emit opcua->SiganlBatchWrite(stru.WcmdstackerValues,stru.Datatype);
                    }
                    emit opcua->SiganlBatchWrite(cmd.WcmdstackerValues,cmd.Datatype);
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
/// \brief TCommtransceivermanager::GetCommOb
/// \param Id
/// \return
///
HWdeviceabstractInterface *TCommtransceivermanager::GetCommOb(int Id)
{
    HWdeviceabstractInterface *ob = nullptr;
    if(m_HWdeviceMap.contains(Id))
    {
        ob  = m_HWdeviceMap[Id];
    }
    return ob;
}
///
/// \brief TCommtransceivermanager::CloseHWCommOb
///断开对象通讯连接
void TCommtransceivermanager::CloseHWCommOb()
{
    // m_heartTimer->stop();
    for(auto it = m_HWdeviceMap.begin(); it!= m_HWdeviceMap.end();++it)
    {
        if(it.value())
        {
            it.value()->CloseComm();
        }
    }
    // delete m_heartTimer;
    m_HWdeviceMap.clear();
}
///
/// \brief TCommtransceivermanager::ReconnectComm
/// \param carid
/// 开放重新连接机制
void TCommtransceivermanager::ReconnectComm(int id)
{
    for(auto it = m_HWdeviceMap.begin();it!= m_HWdeviceMap.end();++it)
    {
        if(it.key() == id)
        {
            switch (it.value()->GetHWprotype())
            {
            case HWDEVICEPROTYPE::KTcpClient:
            {
                TCommTCPclient *tob = dynamic_cast<TCommTCPclient *>(it.value());
                tob->reConnection();
                break;
            }
            default:
                break;
            }
            break;
        }
    }
}
///
/// \brief TCommtransceivermanager::sendDataToHWob
/// \param data
/// 发送数据到对应的硬件对象中,根据ID 找到对应的对象实施发送
void TCommtransceivermanager::sendDataToHWob(QByteArray datavalue,int id)
{
    for(auto it = m_HWdeviceMap.begin();it!= m_HWdeviceMap.end();++it)
    {
        if(it.key() == id)
        {
            switch (it.value()->GetHWprotype())
            {
            case HWDEVICEPROTYPE::KTcpClient:
            {
                TCommTCPclient *tob = dynamic_cast<TCommTCPclient *>(it.value());
                emit tob->signalSendHWdeviceData(datavalue);
                break;
            }
            case HWDEVICEPROTYPE::Tcpserver:
            {
                TCommTCPserver *tob = dynamic_cast<TCommTCPserver *>(it.value());
                emit tob->signalSendHWdeviceData(datavalue);
                break;
            }
            case HWDEVICEPROTYPE::KModbusTcpClient:// 地址 类型 字节 值
            {
                //字节结构转换
                TCommModbusTcpClient *tob = dynamic_cast<TCommModbusTcpClient *>(it.value());
                emit tob->signalSendHWdeviceData(datavalue);
                break;
            }
            default:
                break;
            }
            break;
        }
    }
}
///
/// \brief TCommtransceivermanager::AnalysisDataFrame
/// \param dataframe
///测试使用 在使用
///
void TCommtransceivermanager::AnalysisDataFrame(QByteArray dataframe, int ID,bool high )
{
    QByteArray  tempData;
    tempData.append(dataframe);
    if(m_updatecarMap.contains(ID)&&m_HWdeviceMap.contains(ID))
    {
        m_updatecarMap[ID]->SetCarParamByFrame(tempData,ID,high);
    }
}
///
/// \brief TCommtransceivermanager::ReceDataFromHWob
/// \param ID
/// \param hwtype
/// \param data
///接收小车 扫码枪等硬件发过来的数据进行处理
void TCommtransceivermanager::ReceDataFromHWob(int ID, int hwtype, QByteArray data)
{
    QMutexLocker locker(&m_TCommMutex);
    //根据各个对象进行包解析 更新数据内容
    QByteArray  tempData;
    tempData.append(data);
    switch (hwtype) {
    case HWDEVICETYPE::RGVCAR:
    {
        AnalysisDataFrame(tempData,ID,m_high);
        break;
    }
    case HWDEVICETYPE::BARCODE://自动扫码枪数据更新
    {
        //扫码枪连接创建 串口部分
        if(m_updateScanBarCodeMap.contains(ID))
        {
            m_updateScanBarCodeMap[ID]->SetScanBarParamByFrame(tempData,ID);
        }
        break;
    }
    case HWDEVICETYPE::RUNNER:
    {
        if(m_updateRunnerMap.contains(ID))
        {
            m_updateRunnerMap[ID]->SetRunnParamByPiFrame(tempData,ID);
        }

        break;
    }
    default:
        break;
    }
}
///
/// \brief TCommtransceivermanager::ReceModbusDataFromHWob
/// \param ID 硬件唯一编号
/// \param hwtype 流道 电梯 等硬件
/// \param datatype
/// \param Data
///modbus 协议接收的内容
void TCommtransceivermanager::ReceModbusDataFromHWob(int ID, int hwtype, int datatype, QMap<int, int> Data)
{
    QMutexLocker locker(&m_TCommMutex);
    if(!m_HWdeviceMap.contains(ID))
        return ;
    switch (hwtype) {
    case HWDEVICETYPE::RUNNER: //流道 //使用三菱Q系列的MC协议
    {
        if(m_updateRunnerMap.contains(ID))
        {
            m_updateRunnerMap[ID]->SetRunnerParamByFrame(datatype,Data);
        }
        break;
    }
    case HWDEVICETYPE::ELEVATOR_CAR: //小车电梯的值 fx3u系列 编程口协议
    {
        if(m_updateElevatorMap.contains(ID))
        {
            m_updateElevatorMap[ID]->SetElevatorParamByFrame(ID,Data);
        }
        break;
    }
    default:
        break;
    }
}

void TCommtransceivermanager::ReceOPCDataFromHWob(int ID, int hwtype, QMap<QString, QVariant> DataMap)
{
    QMutexLocker locker(&m_TCommMutex);
    if(!m_HWdeviceMap.contains(ID))
        return ;
    switch (hwtype) {
    case HWDEVICETYPE::STACKER: //流道 //使用三菱Q系列的MC协议
    {
        if(m_updatestackerMap.contains(ID))
        {
            m_updatestackerMap[ID]->SetStackerParam(ID,DataMap);
        }
        break;
    }
    default:
        break;
    }

}

///
/// \brief TCommtransceivermanager::Slotdisconnect
/// \param ID
/// \param type
///通讯断开的信号
void TCommtransceivermanager::Slotconnectstate(int ID, int type,bool state)
{
    //接收到掉线信号自动重新连接 设备状态更新
    int iresult = 0;
    if(state)
    {
        iresult = 1;
    }
    if(!m_HWdeviceMap.contains(ID))
    {
        return;
    }
    switch (type)
    {
    case HWDEVICETYPE::RGVCAR:
    {
        TCommTCPclient *tob = dynamic_cast<TCommTCPclient *>(m_HWdeviceMap[ID]);
        if(tob->GetHWprotype() == KTcpClient && m_updatecarMap.contains(ID))
        {
            m_updatecarMap[ID]->Updateconnectstate(ID,iresult);
        }
        break;
    }
    case HWDEVICETYPE::RUNNER:
    {
        if(m_updateRunnerMap.contains(ID))
        {
            m_updateRunnerMap[ID]->Updateconnectstate(ID,iresult);
        }
        break;
    }
    case HWDEVICETYPE::ELEVATOR_CAR:
    {
        if(m_updateElevatorMap.contains(ID))
        {
            m_updateElevatorMap[ID]->Updateconnectstate(ID,state);
        }
        break;
    }
    case HWDEVICETYPE::BARCODE:
    {
        //扫码枪连接创建 串口部分
        if(m_updateScanBarCodeMap.contains(ID))
        {
            m_updateScanBarCodeMap[ID]->Updateconnectstate(ID,state);
        }
        break;
    }
    case HWDEVICETYPE::STACKER:
    {
        //堆垛机部分
        if(m_updatestackerMap.contains(ID))
        {
            m_updatestackerMap[ID]->Updateconnectstate(ID,state);
        }
        break;
    }
    default:
        break;
    }
}

///
/// \brief TCommtransceivermanager::SlotErrinfo
/// \param ID
/// \param type
/// \param info
///
void TCommtransceivermanager::SlotErrinfo(int ID, int type, QString info)
{
    QString errorinfo = QString("通讯协议错误 硬件ID：%1,硬件类型：%2,info：%3").arg(ID).arg(type).arg(info);
    emit signalError(errorinfo);
}


