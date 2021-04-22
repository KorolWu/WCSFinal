#include "tcommserialport.h"

TcommSerialport::TcommSerialport()
{
   connect(this,&TcommSerialport::signalCreateSerialOb,this,&TcommSerialport::SlotCreateSerialOb);
   connect(this,&TcommSerialport::signalCloseOb,this,&TcommSerialport::SlotCreateSerialOb);
}

void TcommSerialport::SetCommParam(ComConfigStru paramstru)
{
    m_serialconfig = paramstru.hwserialstru;
    connect(this,&TcommSerialport::signalSendHWdeviceData,this,&TcommSerialport::SlotSendDataToHW);
}

int TcommSerialport::GetNameID()
{
    return m_serialconfig.ID;
}

int TcommSerialport::GetHWtype()
{
    return m_serialconfig.hwtype;
}

int TcommSerialport::GetHWprotype()
{
    return HWDEVICEPROTYPE::KSerialPort;
}

void TcommSerialport::CloseComm()
{
    m_serialob->close();
}

bool TcommSerialport::InitSerialport()
{
    //设置串口名
    QString name = m_serialconfig.name;
    m_serialob = new QSerialPort();
    m_serialob->setPortName(name);
    if(m_serialob->isOpen())
    {
        m_serialob->close();
    }
    //打开串口 读写模式
    if(m_serialob->open(QIODevice::ReadWrite))
    {
        qDebug()<<m_serialconfig.ID<<":与硬件串口打开成功";
        //设置波特率 9600
        m_serialob->setBaudRate(static_cast<QSerialPort::BaudRate> (m_serialconfig.BaudRate));
        //设置数据位 8
        m_serialob->setDataBits(static_cast<QSerialPort::DataBits>(m_serialconfig.DataBits));
        //设置奇偶校验 无
        m_serialob->setParity(static_cast<QSerialPort::Parity>(m_serialconfig.Parity));
        //设置停止位 1
        m_serialob->setStopBits(static_cast<QSerialPort::StopBits>(m_serialconfig.StopBits));
        //设置流控制
        m_serialob->setFlowControl(QSerialPort::NoFlowControl);
        m_serialob->setReadBufferSize(4096);
        connect(m_serialob, SIGNAL(readyRead()), this, SLOT(RevSerialDataFromHW()));
        emit signalHWDisconnect(m_serialconfig.ID,m_serialconfig.hwtype,true);
        m_connectstate = 1;
        return true;
    }else
    {
        m_connectstate = -1;
       // qDebug()<<"串口打开失败...."<<m_serialconfig.ID<<endl;
         emit signalHWDisconnect(m_serialconfig.ID,m_serialconfig.hwtype,false);
        return false;
    }
}
///
/// \brief TcommSerialport::RevSerialDataFromHW
/// 串口接收数据部分的内容
void TcommSerialport::RevSerialDataFromHW()
{
    QByteArray arry;
    arry = m_serialob ->readAll();

   if(arry[0] != (char)0x06)
    {
        emit signalReadHWdeviceData(m_serialconfig.ID,m_serialconfig.hwtype,arry);    
    }
}
///
/// \brief TcommSerialport::SlotSendDataToHW
/// \param sendData
/// 发送数据到
void TcommSerialport::SlotSendDataToHW(QByteArray sendData)
{
    if(m_serialob->isOpen())
    {
        m_serialob->write(sendData);
    }
    else
    {
        qDebug()<<"scanbar write failed<<"<<sendData.toHex();
    }
}

void TcommSerialport::SlotCreateSerialOb()
{
    InitSerialport();
}

void TcommSerialport::SlotCloseOb()
{
     m_serialob->close();
}
