#include "tcommfx3userialport.h"
#include <QEventLoop>
#include <QDateTime>
#include <QTimer>

TCommFx3uSerialport::TCommFx3uSerialport()
{
    m_connectStatus = false;
    m_enbled = true;
    m_sendtype = -1;
    m_startaddress = 0;
    m_requestcount = 1;
    m_requestDatatype = 0;
    qRegisterMetaType<QMap<int,int>>("QMap<int,int>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    memset(m_RevData,0,255);
    m_count = 0;
    m_writetimer = new QTimer();
    connect(this,&TCommFx3uSerialport::signalReadData,this,&TCommFx3uSerialport::readDataRequest);
    connect(this,&TCommFx3uSerialport::signalWriteData,this,&TCommFx3uSerialport::on_writeData_request);
    connect(this,&TCommFx3uSerialport::signalCreateSerialOb,this,&TCommFx3uSerialport::CreateSerialOb);
    connect(m_writetimer,&QTimer::timeout,this,&TCommFx3uSerialport::slotTaskwrite);
    m_writetimer->start(250);

}

void TCommFx3uSerialport::SetCommParam(ComConfigStru paramstru)
{
    m_serialconfig = paramstru.hwserialstru;
}

int TCommFx3uSerialport::GetNameID()
{
    return m_serialconfig.ID;
}

int TCommFx3uSerialport::GetHWtype()
{
    return m_serialconfig.hwtype;
}

int TCommFx3uSerialport::GetHWprotype()
{
    return KFx3uSerialport;
}

void TCommFx3uSerialport::CloseComm()
{
    m_serialob->close();
    m_connectStatus = false;
}

void TCommFx3uSerialport::CreateSerialOb()
{
    InitSerialport();
}

bool TCommFx3uSerialport::InitSerialport()
{
    //设置串口名
    QString name = m_serialconfig.name;
    m_serialob = new QSerialPort();
    //connect(m_serialob, &QSerialPort::errorOccurred,this, &TCommFx3uSerialport::sloterrorOccurred);
    m_serialob->setPortName(name);
    if(m_serialob->isOpen())
    {
        m_serialob->close();
    }
    //打开串口 读写模式
    if(m_serialob->open(QIODevice::ReadWrite))
    {
       // qDebug()<<m_serialconfig.ID<<":与硬件串口打开成功";
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

        connect(m_serialob, SIGNAL(readyRead()), this, SLOT(slotRevSerialDataFromHW()));
        emit signalHWDisconnect(m_serialconfig.ID,m_serialconfig.hwtype,true);
        m_connectstate = 1;
        return true;
    }else
    {
        m_connectstate = -1;
        //qDebug()<<"串口打开失败...."<<m_serialconfig.ID<<endl;
        emit signalHWDisconnect(m_serialconfig.ID,m_serialconfig.hwtype,false);
        return false;
    }
}

int TCommFx3uSerialport::GetWaitreplyresult()
{
    int result= -1;
    struct timeval tpStart,tpEnd;
    float timeUse = 0;
    gettimeofday(&tpStart,NULL);//设置500ms
    while (timeUse < 2000 && (!m_connectStatus))
    {
        if(m_enbled)
        {
            result = 1;
            break ;
        }
        gettimeofday(&tpEnd,NULL);
        timeUse = 1000 *(tpEnd.tv_sec - tpStart.tv_sec) + 0.001*(tpEnd.tv_usec - tpStart.tv_usec);
        if(timeUse >= 2000)
            break;
        QThread::msleep(5);
    }
    if(result == 1)
    {
        //qDebug()<<"TCommFx3uSerialport:2S之内请求回复成功 id:"<<m_serialconfig.ID;
    }
    return result;
}

int TCommFx3uSerialport::SendData(QByteArray sendData)
{
    int iresult = m_serialob->write(sendData);
   // qDebug()<< "TCommFx3uSerialport"<<m_serialconfig.ID <<"sendDatasize: "<< iresult<<sendData.toHex();
    return iresult;
}
///
/// \brief TCommFx3uSerialport::Dataprocess
/// \param data
/// \param len
/// \return
/// 得到对应数量的结果
bool TCommFx3uSerialport::Dataprocess(char *data, int len)
{
    bool reccheck = false;

    char tempdata[255] = {0};
    memcpy(tempdata,data,len);
    //判断结束符号
    if(data[len-3] == (char)(0x03))
    {
        //4个字节转化成一个变量的值
         QString str;
        for(int i = 0 ; i < len - 3; ++i)
        {
            str.append(tempdata[i]);
        }
        if(str.size() > 0)
        {
           QByteArray bytearry = QString2Hex(str);//数据转int
           int varlistsize = str.length()/2;
          //int bytesize = str.length()%4;
           if(varlistsize == m_requestcount*2)
           {
               QMap<int,int> Datamap;
               for(int i = 0;i < varlistsize  ; ++i)
               {
                   IntToChar varunionto;
                   varunionto.strchr[0] = bytearry[i*2];
                   varunionto.strchr[1] = bytearry[i*2+1];
                   //qDebug()<<"转化得到的值:"<<varunionto.var <<QString::number(varunionto.var,16);
                   Datamap.insert(m_startaddress+i,varunionto.var);
               }
               if(Datamap.size() > 0)
               {
                   emit signalReceModbusHWdeviceData(m_serialconfig.ID,m_serialconfig.hwtype,m_requestDatatype,Datamap);
                   reccheck = true;
               }
           }
        }
    }
    return reccheck;
}

char TCommFx3uSerialport::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

QByteArray TCommFx3uSerialport::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i = 0; i < len; )
    {
        hstr = str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

QByteArray TCommFx3uSerialport::sendWriteFrame(int address, int value)
{
    QByteArray arry;
    //type 默认地址是50 或者 52
    switch (address) {
    case 50:
    {
        QByteArray headarray;//02 31 31303634 3032 30313030 03 3232
        headarray.append(0x02);
        headarray.append(0x31);
        headarray.append(0x31);
        headarray.append(0x30);
        headarray.append(0x36);
        headarray.append(0x34);
        headarray.append(0x30);
        headarray.append(0x32);
        switch (value) {
        case 1:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x31);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x32);
            break;
        }
        case 2:
         {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x32);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x33);
            break;
        }
        case 3:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x33);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x34);
            break;
        }
        case 4:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x34);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x35);
            break;
        }
        case 5:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x35);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x36);
            break;
        }
        case 6:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x36);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x37);
            break;
        }
        case 7:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x37);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x38);
            break;
        }
        case 8:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x38);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x39);
            break;
        }
        default:
            break;
        }

        break;
    }
    case 52://02 31 31303638 3032 30313030 03 3236
    {
        QByteArray headarray;
        headarray.append(0x02);
        headarray.append(0x31);
        headarray.append(0x31);
        headarray.append(0x30);
        headarray.append(0x36);
        headarray.append(0x38);
        headarray.append(0x30);
        headarray.append(0x32);
        switch (value) {
        case 1:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x31);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x36);
            break;
        }
        case 2:
        {
            arry.append(headarray);
            arry.append(0x30);
            arry.append(0x32);
            arry.append(0x30);
            arry.append(0x30);
            arry.append(0x03);
            arry.append(0x32);
            arry.append(0x37);
            break;
        }
        default:
            break;
        }

        break;
    }
    default:
        break;
    }
    qDebug()<<"elevator send write data:"<<arry.toHex();
    return arry;
}

void TCommFx3uSerialport::readDataRequest(int type, int startAddress, int numberOfEntries)
{
    //检查是否可以执行发送请求
//    if(m_connectstate <= 0)
//        return;
//    if(!m_enbled)
//    {
//        if(GetWaitreplyresult() != 1)
//        {
//            m_connectstate = -2;
//            qDebug()<<"TCommFx3uSerialport 上一个请求未回复超时2S" << m_serialconfig.ID;
//            return;
//        }
//    }

    //请求批量读数据02 30 31 30 43 38 30 36 03 37 35
    QByteArray arry;
    arry.append(0x02);
    arry.append(0x30);
    arry.append(0x31);
    arry.append(0x30);
    arry.append(0x43);
    arry.append(0x38);
    arry.append(0x30);
    arry.append(0x36);
    arry.append(0x03);
    arry.append(0x37);
    arry.append(0x35);
    TaskStru stru;
    stru.sendData.append(arry);
    stru.type = fx3uBatchRead;
    stru.startaddress = startAddress;
    stru.valuesize = numberOfEntries;
    stru.requestDatatype = type;
    QMutexLocker locker(&m_Mutex);
    m_sendTask.append(stru);
//    if(SendData(arry) > 0)
//    {
//        m_sendtype = fx3uBatchRead;
//        m_requestDatatype = type;
//        m_startaddress = startAddress;
//        m_requestcount = numberOfEntries;
//        m_enbled = false;
//        if(GetWaitreplyresult() != 1)
//        {
//            m_connectstate = -2;
//            qDebug()<<"批量读数据包不完整:" << m_serialconfig.ID;
//            return;
//        }
//    }
}


void TCommFx3uSerialport::on_writeData_request(int type, int startAddress, QVector<int> values)
{
    //检查是否可以执行发送请求
//    if(m_connectstate <= 0)
//        return;
//    if(!m_enbled)
//    {
//        if(GetWaitreplyresult() != 1)
//        {
//            m_connectstate = -2;
//            qDebug()<<"TCommFx3uSerialport:上一个请求未回复超时2S" << m_serialconfig.ID;
//            return;
//        }
//    }
     QByteArray arry;
     arry.append(sendWriteFrame(startAddress,values[0]));
     TaskStru stru;
     stru.sendData.append(arry);
     stru.type = fx3uBatchWrite;
     stru.valuesize = values.size();
     stru.requestDatatype = type;
     stru.startaddress = startAddress;
     QMutexLocker locker(&m_Mutex);
     m_sendTask.append(stru);
//    if(SendData(arry) > 0)
//    {
//        m_sendtype = fx3uBatchWrite;
//        m_requestDatatype = type;
//        m_startaddress = startAddress;
//        m_requestcount = values.size();
//        m_enbled = false;
//        if(GetWaitreplyresult() != 1)
//        {
//            m_connectstate = -2;
//            qDebug()<<"TCommFx3uSerialport:fx3uBatchWrite 等待回复" << m_serialconfig.ID;
//            return;
//        }
//    }

}

void TCommFx3uSerialport::sloterrorOccurred(int state)
{
    if(state > 0)
    {
        //qDebug()<<"串口出现的Error状态:"<<state;
        emit signalHWDisconnect(m_serialconfig.ID,m_serialconfig.hwtype,false);
    }
    switch (state) {
    case QSerialPort::SerialPortError::DeviceNotFoundError:
    {
        break;
    }
    case QSerialPort::SerialPortError::PermissionError:
    {
        break;
    }
    default:
        break;
    }
}

void TCommFx3uSerialport::slotRevSerialDataFromHW()
{
    QByteArray arry;
    arry = m_serialob ->readAll();
    if(arry.length() <= 0)
        return;
   // qDebug()<< m_serialconfig.ID<<":data hex --"<<arry.toHex() <<"size:"<< arry.length();
    //  if(arry.length() == m_serialconfig.bytesize)
    {
        // emit signalReadHWdeviceData(m_serialconfig.ID,m_serialconfig.hwtype,arry);
    }
    //判断是否是完整的包
    switch (m_sendtype) {
    case fx3uBatchRead://批量读 数据的格式
    {
        //qDebug()<<"BatchRead:tohex"<<arry.toHex();
        for(int i = 0; i < arry.length(); ++i )
        {
            switch(m_count)
            {
            case 0:
            {    if( arry.at(i)==0x02)
                {
                    m_count++;
                }
                else if(arry.at(i)==0x0f)
                {
                    m_count = 0;
                    m_enbled = true;
                    qDebug()<<"数据读错误故障返回结果";
                }
                break;
            }
            default:
            {
                if(m_count >= 1 && m_count < 255 )
                {
                    m_RevData[m_count-1]= arry.at(i);
                }
                m_count++;
                if(m_count == 1 + m_requestcount*4+1+2)
                {
                    int length = m_count-1;
                    if(Dataprocess(m_RevData,length)) //数据校验正确
                    {
                       // qDebug()<<"数据校验正确："<< length << m_requestcount;
                    }
                    m_count = 0;
                    m_enbled = true;
                }
                break;
            }
            }
        }
        break;
    }
    case fx3uBatchWrite://是读还是写
    {
        if(arry.length() == 1 && arry[0] == (char)(0x06)) //写成功的话回复的结果是06
        {
            m_enbled = true;
            qDebug()<<"批量写回复成功 start address：" << m_startaddress;
        }
        else if(arry[0] == (char)(0x0f))
        {
            m_enbled = true;
            qDebug()<<"批量写数据帧错误 start address：" << m_startaddress;
        }
        else
            qDebug()<<"excaption:"<<arry.toHex();
        break;
    }
    default:
        break;
    }
}

void TCommFx3uSerialport::slotTaskwrite()
{
   // QMutexLocker locker(&m_Mutex);
    while (!m_sendTask.isEmpty())
    {
        TaskStru t =  m_sendTask.dequeue();
        if(SendData(t.sendData) > 0)
        {
             m_sendtype = t.type;
             m_requestDatatype = t.requestDatatype;
             m_startaddress = t.startaddress;
             m_requestcount = t.valuesize;
        }
        QEventLoop loop;//定义一个新的事件循环
        QTimer::singleShot(80, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
        loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    }
}


