#include "tcommopcuaclient.h"
#include <QOpcUaProvider>

TCommOPCUAClient::TCommOPCUAClient()
{
    mOpcUaProvider = new QOpcUaProvider(this);
    mClientConnected =false;
    mOpcUaClient = nullptr;
    qRegisterMetaType<QMap<QString,QVariant>>("QMap<QString,QVariant>");
    m_connectstate = 0;
}

void TCommOPCUAClient::SetCommParam(ComConfigStru paramstru)
{
    m_config = paramstru.hwTcpstru;
    connectToServer();
}

int TCommOPCUAClient::GetNameID()
{
    return  m_config.ID;
}

int TCommOPCUAClient::GetHWtype()
{
    return m_config.hwtype;
}

int TCommOPCUAClient::GetHWprotype()
{
    return KOPCUAClient;
}

void TCommOPCUAClient::CloseComm()
{
    if (mClientConnected) {
        mOpcUaClient->disconnectFromEndpoint();
    }
    if(mOpcUaClient != nullptr)
    {
        mOpcUaClient->deleteLater();
    }
}
///
/// \brief TCommOPCUAClient::connectToServer
///
void TCommOPCUAClient::connectToServer()
{
    if (mClientConnected) {
        mOpcUaClient->disconnectFromEndpoint();
        return;
    }
    if(mOpcUaProvider->availableBackends().size() == 0)
    {
        const QString message(tr("The list of available OPCUA plugins is empty. No connection possible."));
        qDebug()<<"TCommOPCUAClient errormessage:"<<message;
        emit signalErrorInfo(m_config.ID,m_config.hwtype,message);
        return;
    }
    mOpcUaClient = mOpcUaProvider->createClient(mOpcUaProvider->availableBackends()[0]);
    if (!mOpcUaClient)

    {
        const QString message(tr("Connecting to the given sever failed. See the log for details."));
        qDebug()<<"TCommOPCUAClient errormessage:"<<message;
        emit signalErrorInfo(m_config.ID,m_config.hwtype,message);
        //        log(message, QString(), Qt::red);
        //        QMessageBox::critical(this, tr("Failed to connect to server"), message);
        return;
    }
    connect(mOpcUaClient, &QOpcUaClient::connected, this, &TCommOPCUAClient::clientConnected);
    connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &TCommOPCUAClient::clientDisconnected);
    connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &TCommOPCUAClient::clientError);
    connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &TCommOPCUAClient::clientState);
    connect(mOpcUaClient,&QOpcUaClient::batchReadFinished,this, &TCommOPCUAClient::ReadFinished);
    connect(mOpcUaClient,&QOpcUaClient::batchWriteFinished,this, &TCommOPCUAClient::WriteFinished);
    connect(this,&TCommOPCUAClient::SignalBatchRead,this, &TCommOPCUAClient::slotBatchRead);
    connect(this,&TCommOPCUAClient::SiganlBatchWrite,this, &TCommOPCUAClient::slotBatchWrite);
    //  设置用户验证部分的内容
    //mOpcUaClient->UserIdentity = new UserIdentity( "user", "password" );
    //    QUrl myurl;
    //    myurl.setUserName("user");
    //    myurl.setUserName("password");
    //    myurl.setUrl(mServerUrl->text());
    //name 格式  "opc.tcp://127.0.0.1:43344"
    //m_config.name = "opc.tcp://127.0.0.1:43344";
    mOpcUaClient->connectToEndpoint(m_config.name);
}
///
/// \brief TCommOPCUAClient::clientConnected
///
void TCommOPCUAClient::clientConnected()
{
    mClientConnected = true;
    emit signalHWDisconnect(m_config.ID,m_config.hwtype,mClientConnected);
    m_connectstate  = 1;
}
///
/// \brief TCommOPCUAClient::clientDisconnected
///
void TCommOPCUAClient::clientDisconnected()
{
    mClientConnected = false;
    //mOpcUaClient->deleteLater();
    emit signalHWDisconnect(m_config.ID,m_config.hwtype,mClientConnected);
    m_connectstate = -1;

}
///
/// \brief TCommOPCUAClient::clientError
/// \param error
///
void TCommOPCUAClient::clientError(QOpcUaClient::ClientError error)
{
    qDebug() << "TCommOPCUAClient---Client error changed" << error;
    emit signalErrorInfo(m_config.ID,m_config.hwtype,QString("TCommOPCUAClient---Client error changed").arg(error));
}
///
/// \brief TCommOPCUAClient::clientState
/// \param state
///
void TCommOPCUAClient::clientState(QOpcUaClient::ClientState state)
{
    qDebug() << "TCommOPCUAClient---Client state changed" << state <<"url"<<m_config.name;
}
///
/// \brief TCommOPCUAClient::ReadFinished
/// \param results
/// \param serviceResult
///
void TCommOPCUAClient::ReadFinished(QVector<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult)
{
    if(serviceResult != QOpcUa::UaStatusCode::Good)
    {
        QString error = QString("TCommOPCUAClient---ReadFinished error::").arg(serviceResult);
        qDebug() << error;
        emit signalErrorInfo(m_config.ID,m_config.hwtype,error);
        return;
    }
    QMap<QString,QVariant> DataMap;
    for(int it = 0;it < results.size(); ++it)
    {
        //qDebug()<<"read result fininshed= nodeid:"<<results[it].nodeId()<<"value:"<<results[it].value();
        if(results[it].value().type() != QVariant::Type::Invalid)
        {
            DataMap.insert(results[it].nodeId(),results[it].value());
        }
    }
    emit signalReceOPCUAData(m_config.ID,m_config.hwtype,DataMap);
}
///
/// \brief TCommOPCUAClient::WriteFinished
/// \param results
/// \param serviceResult
///
void TCommOPCUAClient::WriteFinished(QVector<QOpcUaWriteResult> results, QOpcUa::UaStatusCode serviceResult)
{
    if(serviceResult != QOpcUa::UaStatusCode::Good)
    {
        QString error = QString("TCommOPCUAClient---ReadFinished error:").arg(serviceResult);
        qDebug() << error;
        emit signalErrorInfo(m_config.ID,m_config.hwtype,error);
        return;
    }
    for(int it = 0;it < results.size(); ++it)
    {
        //qDebug()<< "WriteFinished result = nodeid:"<<results[it].nodeId();
    }
}
///
/// \brief TCommOPCUAClient::slotBatchRead
/// \param nametypeMap
///
void TCommOPCUAClient::slotBatchRead(QVector<QString> names)
{
    if(!mClientConnected)
        return;
    //NodeAttribute
    //request.push_back(QOpcUaReadItem("ns=2;s=Machine.Designation",
    //                                  QOpcUa::NodeAttribute::Value));
    QVector<QOpcUaReadItem> request;
    for(int it = 0; it < names.size(); ++it)
    {
        request.push_back(QOpcUaReadItem(names[it], QOpcUa::NodeAttribute::Value));
    }
    mOpcUaClient->batchRead(request);
}
///
/// \brief TCommOPCUAClient::slotBatchWrite
/// \param valuesMap
/// \param valuetype
//Boolean         = 0,
//Int32           = 1,
//UInt32          = 2,
//Double          = 3,
//Float           = 4,
//String          = 5,
//LocalizedText   = 6,
//DateTime        = 7,
//UInt16          = 8,
//Int16           = 9,
//UInt64          = 10,
//Int64           = 11,
void TCommOPCUAClient::slotBatchWrite(QMap<QString, QVariant> valuesMap, int valuetype)
{
    if(!mClientConnected)
        return;
    const auto type =
            static_cast<QOpcUa::Types> (valuetype);
    QVector<QOpcUaWriteItem> request;
    for(auto it = valuesMap.begin(); it != valuesMap.end(); ++it)
    {
        request.push_back(QOpcUaWriteItem(it.key(), QOpcUa::NodeAttribute::Value,it.value(),type));
    }

    if(request.size() > 0)
        mOpcUaClient->batchWrite(request);
}
