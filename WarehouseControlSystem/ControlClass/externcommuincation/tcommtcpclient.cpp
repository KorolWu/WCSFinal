#include "tcommtcpclient.h"
#include <QDateTime>
#include "tcommtransceivermanager.h"
#include "UnitClass/c_systemlogmng.h"
TCommTCPclient::TCommTCPclient()
{
    socket = nullptr;
    m_connectStatus = false;
    connect(this,&TCommTCPclient::signalSendHWdeviceData,this,&TCommTCPclient::write);
    connect(this,&TCommTCPclient::signalClientconnectserver,this,&TCommTCPclient::reConnection);
}

TCommTCPclient::~TCommTCPclient()
{
    if(socket == nullptr)
        return;
    socket->disconnectFromHost();
    socket->close();
    socket = nullptr;
}

void TCommTCPclient::SetCommParam(ComConfigStru paramstru)
{
    m_config = paramstru.hwTcpstru;
    creadTcpClient();
}

int TCommTCPclient::GetNameID()
{
    return m_config.ID;
}

int TCommTCPclient::GetHWtype()
{
    return m_config.hwtype;
}

int TCommTCPclient::GetHWprotype()
{
    return KTcpClient;
}

void TCommTCPclient::CloseComm()
{
    if(socket == nullptr)
        return;
    socket->disconnectFromHost();
    socket->close();
}
uint recindex = 0;
bool TCommTCPclient::creadTcpClient()
{
    this->m_ip = m_config.name;
    this->m_port = m_config.port ;
//    if( m_config.ID == 0)
//    {
//        this->m_ip = "10.0.1.68";
//        this->m_port =  1883;
//    }
   // qDebug()<<"ip "<< m_config.name << m_config.port;
    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::disconnected,this,&TCommTCPclient::onDisconnected);
    connect(socket,&QTcpSocket::connected,
            [=]()
    {
        m_connectStatus = true;
       // qDebug()<<"ip "<< m_config.name << m_config.port << m_connectStatus ;
        m_connectstate = 1;

    } );
    connect(socket,&QTcpSocket::readyRead,
            [=]()
    {
        QByteArray array = socket->readAll();
        //  qDebug()<<"小车收到的数据包情况："<<QDateTime::currentDateTime() << m_config.ID << m_config.port << array.toHex()<< array.length();
        //测试部分收到的数据-----------------2020/11/07
        if( m_config.ID == 1)
        {
            if(!(array.length() == 10 || array.length() == 74))
            {
           //   qDebug()<< "!!!!小车收到的数据长度异常：" <<QDateTime::currentDateTime()<< array.length();
            }
            //分析报文类型
            if(array.length() >= 10)
            {
//                int16_t nbr;//指令编号
//                int16_t carnbr;//小车编号
//                atoi16 nbrvalue;
//                memcpy(nbrvalue.a,array.data(),2);
               // nbr = nbrvalue.x;
                atoi16 carrvalue;
                memcpy(carrvalue.a,array.data()+2,2);
               // carnbr = carrvalue.x;
                recindex++;
                if(array[4] == 'S' && array[5] == 'D'&&(array.size() >= 74))//详细报文数据
                {
                    //qDebug()<<"收到:WCS收到小车发送详细数据报文：" << QDateTime::currentDateTime()<< array.length() << array.toHex() <<nbr << recindex ;
                    //解析详细数据内容
                    ReceCarDetailFrame detailstru;//详细数据报文
                    memcpy((char*)&detailstru,array.data(),74);
                  // qDebug()<<"收到:WCS收到小车发送详细数据报文解析内容准备状态：" << QDateTime::currentDateTime()<<";长度:" <<array.length() << "ready:"<<detailstru.info.bready <<"unready:"<<detailstru.info.bunready;
                }
                else {
                    //qDebug()<< "通讯对象中WCS收到小车发送动作数据或者粘包数据报文："<<QDateTime::currentDateTime()<< array.length() <<array.toHex();
                }
            }
        }
       //测试部分收到的数据-----------------2020/11/07
        emit signalReadHWdeviceData(m_config.ID,m_config.hwtype,array);
       // qDebug()<<"收到流道数据内容"<<array.toHex() <<"id"<<m_config.ID;
    });
    bool connect =  connectServer(m_ip,m_port);

    emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
    return connect;
}

bool TCommTCPclient::connectServer(QString ip, qint16 port)
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

bool TCommTCPclient::reConnection()
{
    if(m_connectstate)
        return true;
    m_connectStatus = connectServer(this->m_ip,this->m_port);
    if(m_connectStatus)//连接成功发出成功状态
    {
        emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
         m_connectstate  = 1;
    }
    else{
          m_connectstate  = 0;
    }
    return m_connectStatus;
}
uint sendcnt = 0;
uint senaction = 0;
int TCommTCPclient::write(QByteArray array)
{
    if(!m_connectStatus)//连接成功发出成功状态
        return 1;
    if(socket == nullptr)
        return 1;
    int iresultsize = 0;
    iresultsize = socket->write(array);
    if(iresultsize != 40 && m_config.ID == 1)
    {
        QString str = QString("数据发送失败当前发送成功数量：%1; send array:%2，发送时间:%3; 发送结果：iresultsize").arg(sendcnt).arg(QString(array.toHex())).arg(QDateTime::currentDateTime().toString());
        qDebug()<< str<<QDateTime::currentDateTime();;
        GetSystemLogObj()->writeLog(str,2);
    }
//    else{
//        sendcnt++;
//        if(m_config.ID == 1&&(!(array[32] == 0 && array[33] == 0)))
//        {
//            QString str  = QString("WCS发送动作指令数据到小车ID:[%1],发送时间:%2，发送的数据16进制：%3;").arg(m_config.ID).arg(QDateTime::currentDateTime().toString()).arg(QString(array.toHex()));
//            qDebug() << str;
//        }
//        if(m_config.ID == 1)
//        {
//            QString str  = QString("WCS发送指令数据到小车ID:[%1],发送时间:%2，发送的数据16进制：%3;").arg(m_config.ID).arg(QDateTime::currentDateTime().toString()).arg(QString(array.toHex()));
//             qDebug() << str;
//        }
//    }
    return 0;
}

void TCommTCPclient::onDisconnected()
{
    m_connectstate = 0;
    m_connectStatus = false;
    emit clientDisconnect(m_config.ID,m_config.hwtype);
    emit signalHWDisconnect(m_config.ID,m_config.hwtype,m_connectStatus);
}
