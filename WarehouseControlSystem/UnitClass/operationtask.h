#ifndef OPERATIONTASK_H
#define OPERATIONTASK_H

#include <QObject>
#include <Myconfig.h>
#include <QMutexLocker>
#include <redisbasemodule.h>

///
/// \brief The OperationTask class 对内存里的任务进行删除等操作
///
class OperationTask
{
public:
    OperationTask();
    bool removeTask(int stationNum, QString regCode);
private:
    bool deleteInTask(const int stationNum,const QString regCode);
    bool deleteOutTask(const int stationNum, const QString regCode);
    bool deleteTaskFromRedis(const QString regCode);
    bool m_redisIsConnect;
    RedisBaseModule *m_redis;
};

#endif // OPERATIONTASK_H
