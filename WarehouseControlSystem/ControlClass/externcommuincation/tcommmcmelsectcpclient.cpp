#include "tcommmcmelsectcpclient.h"
#include <QDateTime>

TCommMCMelsecTCPclient::TCommMCMelsecTCPclient()
{
    socket = nullptr;
    m_connectStatus = false;
    m_enbled = true;
    m_senddatatype = UnknownSendDatatype;
    m_opsofttype = UnknownSofttype;
    m_requestaddress = -1;
    m_requestcount = 0;
   // memset(m_RevData,0,128);
    qRegisterMetaType<QMap<int,int>>("QMap<int,int>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    connect(this,&TCommMCMelsecTCPclient::signalReadData,this,&TCommMCMelsecTCPclient::readDataRequest);
    connect(this,&TCommMCMelsecTCPclient::signalWriteData,this,&TCommMCMelsecTCPclient::on_writeData_request);
    connect(this,&TCommMCMelsecTCPclient::signalcreateclient,this,&TCommMCMelsecTCPclient::creadTcpClient);
}
///
/// \brief TCommMCMelsecTCPclient::SetCommParam
/// \param paramstru
///
void TCommMCMelsecTCPclient::SetCommParam(ComConfigStru paramstru)
{
    m_config = paramstru.hwTcpstru;
    //creadTcpClient();
}
///
/// \brief TCommMCMelsecTCPclient::GetNameID
/// \return
///得到名字ID
int TCommMCMelsecTCPclient::GetNameID()
{
    return m_config.ID;
}
///
/// \brief TCommMCMelsecTCPclient::GetHWtype
/// \return
///
int TCommMCMelsecTCPclient::GetHWtype()
{
    return m_config.hwtype;
}
///
/// \brief TCommMCMelsecTCPclient::GetHWprotype
/// \return
///
int TCommMCMelsecTCPclient::GetHWprotype()
{
    return kMCMelsecTCPClient;
}
///
/// \brief TCommMCMelsecTCPclient::CloseComm
///
void TCommMCMelsecTCPclient::CloseComm()
{
    if(socket == nullptr)
        return;
    socket->disconnectFromHost();
    socket->close();
    m_connectStatus = false;
    socket = nullptr;
}
///
/// \brief TCommMCMelsecTCPclient::creadTcpClient
/// \return
///
bool TCommMCMelsecTCPclient::creadTcpClient()
{
    socket = new QTcpSocket(this);
    //    connect(socket,&QTcpSocket::disconnected,this,&TCommMCMelsecTCPclient::onDisconnected);
    //    connect(socket,&QTcpSocket::connected,
    //            [=]()
    //    {
    //        m_connectStatus = true;
    //        m_connectstate = 1;

    //    } );
    connect(socket, &QTcpSocket::stateChanged,
            this, &TCommMCMelsecTCPclient::onStateChanged);
    connect(socket,&QTcpSocket::readyRead,
            [=]()
    {
        QMutexLocker locker(&m_Mutex);
        QByteArray array = socket->readAll();
       //if(array.length() <= 128)
        {
           // memcpy(m_RevData,array.data(),array.length());
            // emit signalReadHWdeviceData(m_config.ID,m_config.hwtype,array);
           /* if(*/CheckReceData(array,array.length());/* == 1)*/
          //  {
               // qDebug()<<"数据读写成功："<<m_config.ID;
         //   }
            m_enbled = true;
        }

    });
    bool connect =  connectServer(m_config.name,m_config.port);
    // emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
    return connect;
}

bool TCommMCMelsecTCPclient::connectServer(QString ip, qint16 port)
{
    if(socket == nullptr)
        return false;
    socket->connectToHost(QHostAddress(ip),port);
    if(socket->waitForConnected(1000))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TCommMCMelsecTCPclient::reConnection()
{
    m_connectStatus = connectServer(m_config.name,m_config.port);
    //    if(m_connectStatus)//连接成功发出成功状态
    //    {
    //        emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
    //         m_connectstate  = 1;
    //    }
    //    else{
    //          m_connectstate  = 0;
    //    }
    return m_connectStatus;
}
///
/// \brief TCommMCMelsecTCPclient::write
/// \param array
/// \return
///
qint64 TCommMCMelsecTCPclient::write(QByteArray array)
{

    if(!m_connectStatus)//连接成功发出成功状态
        return 1;
    if(socket == nullptr)
        return 0;
    return socket->write(array);
}
///
/// \brief TCommMCMelsecTCPclient::GetWaitreplyresult
/// \return
///等待回复的过程数据
int TCommMCMelsecTCPclient::GetWaitreplyresult()
{

    int result= -1;
    struct timeval tpStart,tpEnd;
    float timeUse = 0;
    gettimeofday(&tpStart,NULL);//设置500ms
    while (timeUse < 1000)
    {
        if(m_enbled)
        {
            result = 1;
            break ;
        }
        gettimeofday(&tpEnd,NULL);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= 1000)
            break;
        QEventLoop loop;//定义一个新的事件循环
        QTimer::singleShot(10, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
        loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    }
    if(result == 1)
    {
        qDebug()<<"1s之内请求回复成功";
    }
    return result;
}
///
/// \brief TCommMCMelsecTCPclient::CheckReceData
/// \param array
/// \return
///分析收到的数据可靠性
int TCommMCMelsecTCPclient::CheckReceData(QByteArray array,int length)
{
    int iresult = -1;
    QByteArray dataTemp;
    dataTemp.append(array);
    int size = sizeof(MCFrameHeadstru);
    //解析收到数据帧
    MCFrameHeadstru headstru;
    if(size >= length)
        return iresult;
    memcpy((char*)&headstru,dataTemp.data(),size);
    if(!(headstru.header == 208&&headstru.netindex == 0\
            &&headstru.plcindex == 255 &&headstru.targetIOindex == 1023))
    {
        //qDebug()<<"接收的数据帧头格式错误: 索引号。。。";
    }

    switch (m_senddatatype) {
    case BatchRead:
    {
         // qDebug()<<"接收到读请求的结果："<<dataTemp.toHex();
        int count = m_requestcount;
        if(count < 1)
            return -2;
        if(headstru.length == (count*2+2))
        {
            //校正读数据字节长度
            if(length == (headstru.length +size))
            {
                QMap<int,int> Datamap;
               // dataTemp.data()+i*count+2+size;
                for(int i = 0; i < count;++i)
                {
                  // qDebug()<<"pos:"<<i*count+2+size;
                    atoi16 a16;
                    a16.x = 0;
                    memcpy(a16.a,&dataTemp.data()[i*2+2+size],2);
                    int value = a16.x;
                    Datamap.insert(m_requestaddress+i,value);
                  //  qDebug()<<"address:"<<m_requestaddress+i <<"value:"<<value;
                }
                if(Datamap.size() > 0)
                {
                    emit signalReceModbusHWdeviceData(m_config.ID,m_config.hwtype,SoftCodeD,Datamap);
                    iresult =1;
                }
            }
        }
        //校验返回批量读的报文格式
        break;
    }
    case BatchWrite:
    {
        //校验返回批量写的报文格式
       // ComframeReceWriteStru recstru;
          if(headstru.length == 2 &&(length == (size+2))&&((int)dataTemp[length-1] == 0) )
          {
              //数据写成功
             // qDebug()<<"数据写成功:"<<m_requestaddress << m_requestcount;
              iresult =1;
          }
        break;
    }
    default:
        break;
    }
    return iresult;
}
///
/// \brief TCommMCMelsecTCPclient::readDataRequest
/// \param type
/// \param startAddress
/// \param numberOfEntries
/// 发送读数据的请求
void TCommMCMelsecTCPclient::readDataRequest(int type, int startAddress, int numberOfEntries)
{   
    if(m_connectstate <= 0)
        return;
    //检查是否可以执行发送请求
    if(!m_enbled)
    {
         qDebug()<<"进入请求读数据函数：发现还没有回应：时间，"<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:zzz")<<"上一个发送类型是->"<<m_senddatatype;
        if(GetWaitreplyresult() != 1)
        {
            m_connectstate = -2;
            qDebug()<<"上一个请求未回复超时1s";
            return;
        }
    }
     QMutexLocker locker(&m_Mutex);
    ComFrameSendReadMCStru stru;
    stru.headstru.length = 12;
    stru.defineData.cmdtype = 1025;
    stru.defineData.softcount = numberOfEntries;
    stru.defineData.softcode = type;
    union strtovalue{
        int8_t valueby[3];
        int value;
    };
    strtovalue a;
    a.value = startAddress;
    memcpy(stru.defineData.startaddress,a.valueby,3);
    //得到的16进制的字节数组
    QByteArray tempData;
    tempData.append(reinterpret_cast<char*>(&stru),sizeof(ComFrameSendReadMCStru));
    //qDebug()<<"readDataRequest:"<<tempData.toHex() << sizeof(ComFrameSendReadMCStru);
    if(write(tempData) > 0)
    {
        m_senddatatype = BatchRead;
        m_requestaddress = startAddress;
        m_requestcount = numberOfEntries;
        m_enbled = false;
    }
}
///
/// \brief TCommMCMelsecTCPclient::on_writeData_request
/// \param type
/// \param startAddress
/// \param values
///
void TCommMCMelsecTCPclient::on_writeData_request(int type, int startAddress, QVector<int> values)
{  
    if(m_connectstate <= 0)
        return;
    //检查是否可以执行发送请求
    if(!m_enbled)
    {
        if(GetWaitreplyresult() != 1)
        {
            m_connectstate = -2;
            qDebug()<<"上一个请求未回复超时1s";
            return;
        }
    }
    QMutexLocker locker(&m_Mutex);
    ComFrameSendWriteMCstru writestru;
    writestru.headstru.length = 12 + values.size()*2;
    writestru.defineData.softcount = values.size();
    writestru.defineData.cmdtype = 5121;
    writestru.defineData.softcode = type;
    union strtovalue{
        int8_t valueby[3];
        int value;
    };
    strtovalue a;
    a.value = startAddress;
    memcpy(writestru.defineData.startaddress,a.valueby,3);
    QByteArray tempData;
    tempData.append(reinterpret_cast<char*>(&writestru),sizeof(ComFrameSendWriteMCstru));
    for(int i = 0; i < values.size();++i)
    {
        int16_t datavalue = 0;
        datavalue = values[i];
        tempData.append(reinterpret_cast<char*>(&datavalue),sizeof(int16_t));
    }
    qDebug()<<"on_writeData_request:"<<tempData.toHex()<<endl<<"startAddress:"<<startAddress<<"value: "<<values;
    if(write(tempData) > 0)
    {
        m_senddatatype = BatchWrite;
        m_requestaddress = startAddress;
        m_requestcount = values.size();
        m_enbled = false;
    }
}
///
/// \brief TCommMCMelsecTCPclient::onStateChanged
/// \param state
///当连接状态发生改变
void TCommMCMelsecTCPclient::onStateChanged(int state)
{
    //连接状态发生了改变
    //emit signalconnectstate(state);
    bool connected = (state != QTcpSocket::UnconnectedState);
    Q_UNUSED(connected);
    if (state == QTcpSocket::UnconnectedState)
    {

        m_connectStatus = false;
        m_connectstate = -1;
    }

    else if (state == QTcpSocket::ConnectedState)
    {
        m_connectStatus = true;
        m_connectstate = 1;
        m_senddatatype = UnknownSendDatatype;
    }
    emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
    qDebug()<<"设备ID："<<m_config.ID <<",设备类型："<<m_config.hwtype<< ",TCommMCMelsecTCPclient连接状态发生："<<"值："<<state;
}
