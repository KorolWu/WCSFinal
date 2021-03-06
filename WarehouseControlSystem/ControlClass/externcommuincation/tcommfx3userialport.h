#ifndef TCOMMFX3USERIALPORT_H
#define TCOMMFX3USERIALPORT_H

#include <QObject>
#include "hwdeviceabstractinterface.h"
#include <QMutex>
#include <QMutexLocker>
#include <QApplication>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <QThread>
#include <QDebug>
#include <QSerialPort>
#include <QQueue>
#include <QTimer>
#include "Myconfig.h"
enum FX3uSendDatatype {
    fx3uBatchRead = 1,
    fx3uBatchWrite = 2,
    fx3uUnknownSendDatatype = -1
};
union  IntToChar{
    int16_t var;
    char strchr[2] ={0};
};
struct TaskStru{
    int type = -1;
    QByteArray sendData;
    int valuesize = 1;
    int startaddress = 0;
    int requestDatatype =0;
};

class TCommFx3uSerialport:public HWdeviceabstractInterface
{
    Q_OBJECT
public:
    TCommFx3uSerialport();

    // HWdeviceabstractInterface interface
public:
    void SetCommParam(ComConfigStru paramstru);
    int GetNameID();
    int GetHWtype();
    int GetHWprotype();
    void CloseComm();
    void CreateSerialOb();
private:
    bool InitSerialport();
    SerialPortstru m_serialconfig;
    QSerialPort *m_serialob;
    bool m_connectStatus;
    QMutex m_Mutex; //通讯对象部分数据读写锁
    bool m_enbled;
    int GetWaitreplyresult();
    int SendData(QByteArray  sendData);
    int m_sendtype;
    int m_startaddress;
    int m_requestcount;
    int m_requestDatatype;
    char m_RevData[255];
    bool Dataprocess(char *data,int len);
    int m_count;
    typedef union
    {
        char by[3] = {0};
        int carinfo ;
    }teststru;//小车信息结构体
    static char ConvertHexChar(char ch);
    static QByteArray QString2Hex(QString str);
    QByteArray sendWriteFrame(int address,int value);
    QQueue<TaskStru>m_sendTask;
    QTimer *m_writetimer;
private slots:
    void readDataRequest(int type, int startAddress, int numberOfEntries);
    void on_writeData_request(int type, int startAddress, QVector<int> values);
    void sloterrorOccurred(int state);
    void slotRevSerialDataFromHW();
    void slotTaskwrite();
signals:
    void signalReadData(int type,int startAddress,int numberOfEntries);//读各种类型数据的状态 IO 寄存器 Value  外部去写数据变化调用指令
    //void signalRecRead(int ID,int hwtype,int datatype,QMap<int,int> Data); // 读数据类型 data,第一个是参数是地址 第二个参数是 值
    void signalWriteData(int type, int startAddress, QVector<int> values); // 读数据类型 data,第一个是参数是地址 第二个参数是 值
    void signalCreateSerialOb();
    void signalCloseComm();
};

#endif // TCOMMFX3USERIALPORT_H
