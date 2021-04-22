#ifndef TCOMMMCMELSECTCPCLIENT_H
#define TCOMMMCMELSECTCPCLIENT_H

#include "hwdeviceabstractinterface.h"

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>
#include <QApplication>
#include <QEventLoop>
#include <ctime>
#include <QTimer>
#include <time.h>
#include <sys/time.h>
#include <QThread>

enum SendDatatype {
    BatchRead = 1,
    BatchWrite = 2,
    UnknownSendDatatype = -1
};

enum Softcodetype {
    SoftCodeD = 1,
    UnknownSofttype = -1
};
class TCommMCMelsecTCPclient:public HWdeviceabstractInterface
{
 Q_OBJECT
public:
    TCommMCMelsecTCPclient();

    // HWdeviceabstractInterface interface
public:
    void SetCommParam(ComConfigStru paramstru);
    int GetNameID();
    int GetHWtype();
    int GetHWprotype();
    void CloseComm();
private:
    bool creadTcpClient();
    bool connectServer(QString ip,qint16 port);
    bool reConnection();
    qint64 write(QByteArray array);
    int GetWaitreplyresult();
    int CheckReceData(QByteArray array,int length);
   // void onDisconnected();
private:
    QTcpSocket *socket;//通讯套接字
    bool m_connectStatus;
    TcpStru m_config;
    QMutex m_Mutex; //通讯对象部分数据读写锁
    bool m_enbled;
    SendDatatype m_senddatatype;
    Softcodetype m_opsofttype;
    int m_requestaddress;
    int m_requestcount;
    uint8_t   m_RevData[128];
private slots:
    void readDataRequest(int type, int startAddress, int numberOfEntries);
    void on_writeData_request(int type, int startAddress, QVector<int> values);
    void onStateChanged(int state);
signals:
    void signalReadData(int type,int startAddress,int numberOfEntries);//读各种类型数据的状态 IO 寄存器 Vlaue  外部去写数据变化调用指令
  //void signalRecRead(int ID,int hwtype,int datatype,QMap<int,int> Data); // 读数据类型 data,第一个是参数是地址 第二个参数是 值
    void signalWriteData(int type, int startAddress, QVector<int> values); // 读数据类型 data,第一个是参数是地址 第二个参数是 值
    void signalcreateclient();
};

#endif // TCOMMMCMELSECTCPCLIENT_H
