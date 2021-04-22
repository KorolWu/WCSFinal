#ifndef TCOMMOPCUACLIENT_H
#define TCOMMOPCUACLIENT_H

#include <QObject>
#include "hwdeviceabstractinterface.h"
class QOpcUaProvider;
#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QVariant>
class TCommOPCUAClient:public HWdeviceabstractInterface
{
    Q_OBJECT
public:
    TCommOPCUAClient();
    QOpcUaClient *mOpcUaClient;
    // HWdeviceabstractInterface interface
public:
    void SetCommParam(ComConfigStru paramstru);
    int GetNameID();
    int GetHWtype();
    int GetHWprotype();
    void CloseComm();
private:
    QOpcUaProvider *mOpcUaProvider;

    bool mClientConnected;
    TcpStru m_config;
    void connectToServer();
signals:
    void SignalBatchRead(QVector<QString> names);// 读对应节点ID的名字和名字的类型
    void SiganlBatchWrite(QMap<QString,QVariant> valuesMap,int valuetype);
private slots:
    void clientConnected();
    void clientDisconnected();
    void clientError(QOpcUaClient::ClientError);
    void clientState(QOpcUaClient::ClientState);
    void ReadFinished(QVector<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult);
    void WriteFinished(QVector<QOpcUaWriteResult> results, QOpcUa::UaStatusCode serviceResult);
    void slotBatchRead(QVector<QString> names);
    void slotBatchWrite(QMap<QString,QVariant> valuesMap,int valuetype);

};

#endif // TCOMMOPCUACLIENT_H
