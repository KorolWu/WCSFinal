#include "operationtask.h"

OperationTask::OperationTask()
{
    m_redis = new RedisBaseModule(Myconfig::GetInstance()->m_redisInfo.ip.toStdString().c_str(),Myconfig::GetInstance()->m_redisInfo.port);
    m_redisIsConnect = m_redis->getConnect();
}
///
/// \brief OperationTask::removeTask 移除该站点 regcode = regCode对应的任务
/// \param stationNum
/// \param regCode
/// \return
///
bool OperationTask::removeTask(const int stationNum, const QString regCode)
{
    Q_UNUSED(regCode);
    bool result = false;
    result = deleteInTask(stationNum,regCode);
    if(result)
    {
        result =  deleteTaskFromRedis(regCode);
        m_redis->insert_log(QString("%1 移除 %2").arg(regCode).arg(result?"成功":"失败"));
        return result;
    }
    result = deleteOutTask(stationNum,regCode);
    if(result)
    {
        result =  deleteTaskFromRedis(regCode);
        m_redis->insert_log(QString("%1 移除 %2").arg(regCode).arg(result?"成功":"失败"));
        return result;
    }
    return false;
}

bool OperationTask::deleteInTask(const int stationNum, const QString regCode)
{
    if(!Myconfig::GetInstance()->m_IntaskMap.contains(stationNum))
        return false;
    for (auto it = Myconfig::GetInstance()->m_IntaskMap[stationNum].begin();it != Myconfig::GetInstance()->m_IntaskMap[stationNum].end();it ++)
    {
        if(it.value().regCode == regCode)
        {
            QMutexLocker locker(&Myconfig::GetInstance()->m_in_task_mutex);
            Myconfig::GetInstance()->m_IntaskMap[stationNum].remove(it.key());
            return true;
        }
    }
    return false;
}

bool OperationTask::deleteOutTask(const int stationNum, const QString regCode)
{
    if(!Myconfig::GetInstance()->m_OuttaskMap.contains(stationNum))
    {
        return  false;
    }
    QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
    for (int i = 0;i < Myconfig::GetInstance()->m_OuttaskMap[stationNum].size();i++) {
        if(Myconfig::GetInstance()->m_OuttaskMap[stationNum].at(i).regCode == regCode)
        {
            Myconfig::GetInstance()->m_OuttaskMap[stationNum].removeAt(i);
            return true;
        }
    }
    return false;
}

bool OperationTask::deleteTaskFromRedis(const QString regCode)
{
    if(m_redis)
    {
        QString commd = QString("DEL %1").arg(regCode);
        redisReply *reply = m_redis->execommand(commd);
        if(reply != nullptr && reply->type == REDIS_REPLY_INTEGER)
        {
            if(reply->integer == 1)
            {
                qDebug()<<regCode<<"removed!";
                return true;
            }
            else {
                qDebug()<<"task "<<regCode<<"not exist!";
            }
        }
    }
    qDebug()<<regCode<<"remove failed!";
    return false;
}
