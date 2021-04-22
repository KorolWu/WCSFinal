#ifndef KDISPATCH_H
#define KDISPATCH_H

#include <QQueue>
#include <QObject>
#include <QThread>
#include <QRunnable>
#include <sys/time.h>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include "Myconfig.h"
#include "selectcar.h"
#include "basedevice.h"
#include "datastructure.h"
#include "redisbasemodule.h"
#include "KDeviceSingleton.h"
#include "AbstractClass/rgvorder.h"
#include "UnitClass/c_systemlogmng.h"
#include "Instructions/httpclien_k.h"
#include "Instructions/carinstruction.h"
#include "Instructions/runnerinstruction.h"
#include "WarehousingIn/getouttrajectory.h"
#include "MysqlDataBase/crudbaseoperation.h"
#include "Instructions/abstructinstruction.h"
#include "Instructions/carelevatorinstruction.h"
#include "WarehousingIn/generatechargingtrajectory.h"
#include "WarehousingIn/generateinputwarehousingorders.h"
/**
 * @brief The KDispatch class
 * 扫描任务队列消费任务队列 拿到一个任务 并将其完成
 */
class KDispatch : public QObject,public QRunnable
{
    Q_OBJECT
public:
    KDispatch(KPosition task_P, QString &ip, int carId, const TaskInfoStru task);
    ~KDispatch();
private:
    bool runSubTask();
    void initRedis();
    bool removeOldTask(QString regCode);
    void whereCarShouldbe(int i);
    void execRunnerInstruction(int startAddress, int value);
    bool runInstrucation(OrderStru o);
    QString transformationOrder(int i);
    void Delay_MSec(int msec);
    int errorCodeChange(const int errCode);
    bool saveSubTaskInfo(QQueue<OrderStru> taskQueue);
    void reply2WMS(const TaskInfoStru &task, const bool &result);
    void saveErrMassage(const QString &message,const int errCode);

    QString  m_ip;
    int      m_carId;
    KPosition m_task_p;
    bool m_inp = false;
    TaskInfoStru m_task;
    const int TIMEOUT = 1000;
    ReadTableData m_writeData;
    QQueue<OrderStru> m_taskQueue;
    AbstructInstruction *m_pAbstructInstruction;
    RedisBaseModule *m_redis;
    bool m_redisIsConnect = false;
    // QRunnable interface
public:
    QByteArray makeBody(const TaskInfoStru &t, const int interfaceType, QString nowLocation, bool result, QString errMsg);
    void run();
signals:
    void Ordered(OrderStru o);
};

#endif // KDISPATCH_H
