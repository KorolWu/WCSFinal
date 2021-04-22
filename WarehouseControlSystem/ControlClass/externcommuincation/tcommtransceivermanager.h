#ifndef TCOMMTRANSCEIVERMANAGER_H
#define TCOMMTRANSCEIVERMANAGER_H

#include <QObject>
#include "hwcommfactoryob.h"
#include <QTimer>
#include <datastructure.h>
#include <Myconfig.h>
#include "tcommtcpclient.h"
#include "tcommtcpserver.h"
#include "tcommmodbustcpclient.h"
#include "tcommmodbusserial.h"
#include "tcommmcmelsectcpclient.h"
#include "tcommfx3userialport.h"
#include <QMutexLocker>
#include <QMutex>
#include <QMap>
#include "UpdateHWData/updatecardata.h"
#include "UpdateHWData/updaterunnerdata.h"
#include "UpdateHWData/updateelevatordata.h"
#include "UpdateHWData/scanbarcode.h"
#include "tcommserialport.h"
#include "tcommopcuaclient.h"
#include "UpdateHWData/updatestackerdata.h"

///
/// \brief The TCommtransceivermanager class
///所有通讯收发的数据窗口管理器
///
struct ModbusStru{
    int type;
    int bit;
    int address;
    uint16_t value;
    int64_t data;
};
class TCommtransceivermanager:public QObject
{
    Q_OBJECT
private:
    TCommtransceivermanager();
    ~TCommtransceivermanager();
public:
    void InitHWcommob();
    void ResetHWcommob(int Id);//请求复位的ID，出现故障之后需要外界请求进行复位是否继续,比如复位累积的索引值，是否要丢掉
    void SendcommandByExtern(OrderStru cmd,int Id);
    static TCommtransceivermanager* GetInstance()
    {
        static TCommtransceivermanager Instance;
        return &Instance;
    }
    HWdeviceabstractInterface *GetCommOb(int Id);
    void CloseHWCommOb();
    void ReconnectComm(int carid);
signals:
    void SignalCarStatusUpdate();
    void signalError(QString info);
private slots:
    void ReceDataFromHWob(int ID,int hwtype,QByteArray data);//数据接收内容
    void ReceModbusDataFromHWob(int ID,int hwtype,int datatype,QMap<int,int> Data);
    void ReceOPCDataFromHWob(int ID,int hwtype,QMap<QString,QVariant> DataMap);
    void Slotconnectstate(int ID,int type,bool state);
    void SlotErrinfo(int ID,int type,QString info);
private:
    void sendDataToHWob(QByteArray data ,int id);
    void AnalysisDataFrame(QByteArray dataframe, int ID,bool high = true);//解析帧内容
    void SendDatatoCarHW(OrderStru cmd, int hwId);
    void SendDatatoCarV1test(OrderStru cmd, int hwId);
    void SendDatatoRunnerHW(OrderStru cmd, int hwId);
    uint16_t GetRunnerCmdValue(OrderStru cmd);
    void RunnerHighByteConversion(QByteArray &dataframe);

private:
    QMap<int,HWdeviceabstractInterface *> m_HWdeviceMap;
    QMutex m_TCommMutex; //通讯对象部分数据读写锁
    bool m_high;
    //增加更新硬件的数据对象的Map
    QMap<int,UpdateCarData*> m_updatecarMap;
    QMap<int,UpdateRunnerData*>m_updateRunnerMap;
    QMap<int,UpdateElevatorData*>m_updateElevatorMap;
    QMap<int,ScanBarCode*>m_updateScanBarCodeMap;
    QMap<int,UpdateStackerData*>m_updatestackerMap;
private://模板函数
    template<typename T1>
    void CreatObbyHWconfigData(QMap<int,T1> datamap ,HWDEVICEPROTYPE type)
    {
        for(auto it = datamap.begin();\
            it!= datamap.end();++it)
        {
            ComConfigStru stru;
            HWdeviceabstractInterface *ob = nullptr;
            ob = HWCommFactoryOb::CreatorHWCommFactoryOb(it.value().protype);
            if(ob)
            {
                switch (type) {
                case KTcpClient:
                case Tcpserver:
                {
                    int len = sizeof(T1);
                    char buffer[len] = {0};
                    memcpy(buffer,&it.value(),len);
                    TcpStru *tstru =(TcpStru*)(buffer);
                    stru.hwTcpstru = *tstru;
                    if(it.value().protype == kMCMelsecTCPClient)
                    {
                        QThread *MCTcpthread = new QThread();
                        ob->moveToThread(MCTcpthread);
                        connect(ob,&HWdeviceabstractInterface::signalReceModbusHWdeviceData,this,&TCommtransceivermanager::ReceModbusDataFromHWob);
                        MCTcpthread->start();
                    }
                    if(it.value().protype == KOPCUAClient)
                    {
                        connect(ob,&HWdeviceabstractInterface::signalReceOPCUAData,this,&TCommtransceivermanager::ReceOPCDataFromHWob);
                    }                    
                    break;
                }
                case KSerialPort:
                {
                    int len = sizeof(T1);
                    char buffer[len] = {0};
                    memcpy(buffer,&it.value(),len);
                    SerialPortstru *tstru =(SerialPortstru*)(buffer);
                    stru.hwserialstru =  *tstru;
                    if(tstru->protype == KModbusSerialport)
                    {
                        connect(ob,&HWdeviceabstractInterface::signalReceModbusHWdeviceData,this,&TCommtransceivermanager::ReceModbusDataFromHWob);
                    }
                    if(tstru->protype == KFx3uSerialport)
                    {
                        QThread *MCTcpthread = new QThread();
                        ob->moveToThread(MCTcpthread);
                        connect(ob,&HWdeviceabstractInterface::signalReceModbusHWdeviceData,this,&TCommtransceivermanager::ReceModbusDataFromHWob);
                        MCTcpthread->start();
                    }
                    if(tstru->protype == KSerialPort)
                    {
                        QThread *serialthread = new QThread();
                        ob->moveToThread(serialthread);
                        // connect(ob,&HWdeviceabstractInterface::signalReceModbusHWdeviceData,this,&TCommtransceivermanager::ReceModbusDataFromHWob);
                        serialthread->start();
                    }

                    break;
                }
                case KModbusTcpClient:
                {
                    int len = sizeof(T1);
                    char buffer[len] = {0};
                    memcpy(buffer,&it.value(),len);
                    ModbusTcpClientstru *tstru =(ModbusTcpClientstru*)(buffer);
                    stru.hwmodbustcpclistru =*tstru;
                    connect(ob,&HWdeviceabstractInterface::signalReceModbusHWdeviceData,this,&TCommtransceivermanager::ReceModbusDataFromHWob);
                    break;
                }
                    //                case KFx3uSerialport:
                    //                {
                    //                    int len = sizeof(T1);
                    //                    char buffer[len] = {0};
                    //                    memcpy(buffer,&it.value(),len);
                    //                    SerialPortstru *tstru =(SerialPortstru*)(buffer);
                    //                    stru.hwserialstru =  *tstru;
                    //                    break;
                    //                }
                default:
                    break;
                }
                m_HWdeviceMap.insert(it.key(),ob);
                connect(ob,&HWdeviceabstractInterface::signalHWDisconnect,this,&TCommtransceivermanager::Slotconnectstate);
                connect(ob,&HWdeviceabstractInterface::signalErrorInfo,this,&TCommtransceivermanager::SlotErrinfo);
                ob->SetCommParam(stru);
                if(it.value().protype == kMCMelsecTCPClient)//执行放在线程处理
                {
                    TCommMCMelsecTCPclient *MCTcpob = dynamic_cast<TCommMCMelsecTCPclient *>(ob);
                    emit MCTcpob->signalcreateclient();//初始化创建对象
                }
                if(it.value().protype == KFx3uSerialport)//执行放在线程处理
                {
                    TCommFx3uSerialport *fx3uob = dynamic_cast<TCommFx3uSerialport *>(ob);
                    emit fx3uob->signalCreateSerialOb();//初始化创建对象
                }
                if(it.value().protype == KSerialPort)//执行放在线程处理
                {
                    TcommSerialport *serialob = dynamic_cast<TcommSerialport *>(ob);
                    emit serialob->signalCreateSerialOb();//初始化创建对象
                }
                connect(ob,&HWdeviceabstractInterface::signalReadHWdeviceData,this,&TCommtransceivermanager::ReceDataFromHWob);
            }
        }
    }
};

#endif // TCOMMTRANSCEIVERMANAGER_H
