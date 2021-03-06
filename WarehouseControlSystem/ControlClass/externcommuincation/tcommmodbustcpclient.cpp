#include "tcommmodbustcpclient.h"

TCommModbusTcpClient::TCommModbusTcpClient()
{
    modbusDevice = nullptr;
    connect(this,&TCommModbusTcpClient::signalReadData,this,&TCommModbusTcpClient::readDataRequest);
    connect(this,&TCommModbusTcpClient::signaWrite,this,&TCommModbusTcpClient::on_writeData_request);
}

void TCommModbusTcpClient::SetCommParam(ComConfigStru paramstru)
{
    m_configstru = paramstru.hwmodbustcpclistru;
    GetConnect(m_configstru.url_str); //创建主站连接信息
}

int TCommModbusTcpClient::GetNameID()
{
    return m_configstru.ID;
}

int TCommModbusTcpClient::GetHWtype()
{
    return m_configstru.hwtype;
}

int TCommModbusTcpClient::GetHWprotype()
{
    return KModbusTcpClient;
}

void TCommModbusTcpClient::CloseComm()
{
    if(modbusDevice != nullptr)
    {
        modbusDevice->disconnectDevice();
    }
}
///
/// \brief TCommModbusTcpClient::GetConnect
/// \param url_str
/// \return
///客户端modbus连接对方的数据部分内容
bool TCommModbusTcpClient::GetConnect(const QString url_str)
{
    if(modbusDevice != nullptr)
        return false;

    modbusDevice = new QModbusTcpClient(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
       // qDebug()<<(modbusDevice->errorString()) << "通讯发生了故障" << m_configstru.ID;
       // qDebug()<<"error:"<<m_configstru.ID<<errinfo;
         emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,modbusDevice->errorString());
        m_connectstate = 0;
         emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,false);
        return false;

    });
     connect(modbusDevice, &QModbusClient::stateChanged,this, &TCommModbusTcpClient::onStateChanged);
    if(modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        QUrl url = QUrl::fromUserInput(url_str);
        //qDebug()<<"url"<<url.port() << url.host();
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        //客户端使用超时来确定等待服务器响应的时间。如果在所需超时内未收到响应，则设置TimeoutError
        modbusDevice->setTimeout(500);
        //设置客户端在请求失败前将执行的重试次数
        modbusDevice->setNumberOfRetries(3);
        if (!modbusDevice->connectDevice()) {
            qDebug()<<(tr("Connect failed: ") + modbusDevice->errorString(), 5000) << m_configstru.ID ;
            emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,false);
            //m_connectstate = 0;
            return false;
        } else {
            qDebug()<<(tr("Connect successful"))<<m_configstru.ID <<m_configstru.url_str<<m_configstru.serveraddress;
            //emit connect_success_signal();
            emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,true);
            m_connectstate = 1;
            return true;
        }
    }
    return true;
}

///
/// \brief TCommModbusTcpClient::readReady
///请求之后接收到的数据的
bool state = true;
void TCommModbusTcpClient::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();
        // qDebug()<<"收到数据类型结果"<<  unit.registerType();
        QMap<int,int> address_value_map;
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i).arg(QString::number(unit.value(i),
                                                                                                                unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            // qDebug()<<"datatype:"<<unit.registerType()<< entry;
            if(!address_value_map.contains(unit.startAddress()))
            {
                address_value_map.insert(unit.startAddress()+i,unit.value(i));
            }
            else
                address_value_map[unit.startAddress()+i] = unit.value(i);
            emit signalReceModbusHWdeviceData(m_configstru.ID,m_configstru.hwtype,unit.registerType(),address_value_map);
        }
     //   qDebug()<<"type:"<<unit.registerType()<<"values:"<<unit.values();
        state = true;
    }
    else if (reply->error() == QModbusDevice::ProtocolError)
    {
        //if(state)
        {
           qDebug()<<(tr("Read response error: %1 (Mobus exception: 0x%2)"). arg(reply->errorString()).\
                      arg(reply->rawResult().exceptionCode(), -1, 16));
            state = false;
            QString errinfo = tr("Read response error: %1 (Mobus exception: 0x%2)").
                    arg(reply->errorString()).
                    arg(reply->rawResult().exceptionCode(), -1, 16);
           qDebug()<<"error:"<<m_configstru.ID<<errinfo;
            emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);

           // m_connectstate = -1;
        }
    } else
    {
       // if(state)
      {
            qDebug()<<(tr("Read response error: %1 (code: 0x%2)"). arg(reply->errorString()).
                  arg(reply->error(), -1, 16));
             QString errinfo = (tr("Read response error: %1 (code: 0x%2)"). arg(reply->errorString()).
                                arg(reply->error(), -1, 16));
                emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);
          state = false;
         // m_connectstate = -2;
        }
    }
    reply->deleteLater();
}


void TCommModbusTcpClient::on_writeData_request(int type,int startAddress,QVector<int> values)
{

    int numberOfEntries = values.size();
    if (!modbusDevice)
        return;
    if(modbusDevice->state()!=QModbusDevice::ConnectedState)
    {
        m_connectstate = 0;
       // qDebug()<<"未连接状态，请检查"<<modbusDevice->ConnectedState << m_configstru.ID;
        return;
    }
    QModbusDataUnit writeUnit = writeRequest(type,startAddress,numberOfEntries);
    QModbusDataUnit::RegisterType table = writeUnit.registerType();
    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        if (table == QModbusDataUnit::Coils)
            writeUnit.setValue(i, values[i]);
        else
            writeUnit.setValue(i, values[i]);
    }
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit,m_configstru.serveraddress)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug()<<  tr("Write response error: %1 (Mobus exception: 0x%2)")
                               .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                     QString errinfo = tr("Write response error: %1 (Mobus exception: 0x%2)")
                             .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                        emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);

                    //m_connectstate = -3;
                    //5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    //                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                    //                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                   qDebug()<<"通讯硬件ID"<<m_configstru.ID <<tr("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
                     QString errinfo = tr("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
                      emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        // statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
        qDebug()<<"通讯硬件ID："<<m_configstru.ID <<tr("Write error: ") + modbusDevice->errorString();
         QString errinfo  = tr("Write error: ") + modbusDevice->errorString();
          emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);
    }
}

void TCommModbusTcpClient::onStateChanged(int state)
{
    qDebug()<<"connectstate:"<<state << "通讯硬件ID："<<m_configstru.ID ;
    bool connected = (state != QModbusDevice::UnconnectedState);
    emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,connected);
    if (state == QModbusDevice::UnconnectedState)
    {
        //emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,state);
        // ui->connectButton->setText(tr("Connect"));
    }
    else if (state == QModbusDevice::ConnectedState)
    {
        // ui->connectButton->setText(tr("Disconnect"));
      emit signalHWDisconnect(m_configstru.ID,m_configstru.hwtype,connected);
    }
    m_connectstate = state;
}

void TCommModbusTcpClient::readDataRequest(int type,int startAddress,int numberOfEntries)
{
    if (!modbusDevice)
        return;
    if(modbusDevice->state()!=QModbusDevice::ConnectedState)
    {
        qDebug()<<"未连接状态，请检查"<<modbusDevice->ConnectedState << m_configstru.ID;
        return;
    }
    if (auto *reply = modbusDevice->sendReadRequest(readRequest(type,startAddress,numberOfEntries), m_configstru.serveraddress))
    {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &TCommModbusTcpClient::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
       // m_connectstate =-4;
        qDebug()<< tr("Read error: ") + (modbusDevice->errorString(), 5000) << m_configstru.ID<<m_configstru.url_str;

        QString errinfo  = tr("Read error: ") + (modbusDevice->errorString(), 5000);
         emit signalErrorInfo(m_configstru.ID,m_configstru.hwtype,errinfo);
    }
}
QModbusDataUnit TCommModbusTcpClient::readRequest(int type,int startAddress,int numberOfEntries) const
{
    const auto table =
            static_cast<QModbusDataUnit::RegisterType> (type);

    //int startAddress = ui->readAddress->value();
    //Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    // int numberOfEntries = qMin(ui->readSize->currentText().toInt(), 10 - startAddress);
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

QModbusDataUnit TCommModbusTcpClient::writeRequest(int type,int startAddress,int numberOfEntries) const
{
    const auto table =
            static_cast<QModbusDataUnit::RegisterType> (type);

    // int startAddress = ui->writeAddress->value();
    //  Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    //  int numberOfEntries = qMin(ui->writeSize->currentText().toInt(), 10 - startAddress);
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
