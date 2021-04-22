#include "rwredisinterfacelib.h"

RWRedisInterfaceLib::RWRedisInterfaceLib()
{
    m_redismoduleob = NULL;
    m_predistext = NULL;
}

bool RWRedisInterfaceLib::Connectredis(QString ip, int port)
{
      m_redismoduleob  = new RedisBaseModule(ip.toStdString().c_str(),port);
     if(m_redismoduleob->getConnect())
     {
         m_predistext = m_redismoduleob->GetredisContext();
     }
// struct timeval timeout={2,0};
// m_predistext = (redisContext*)redisConnectWithTimeout(ip.toStdString().c_str(),port,timeout);
    if(m_predistext != NULL)
        return true;
    return false;
}
///
/// \brief RWRedisInterfaceLib::ReadCurTaskDataFromRedis
/// \return
///得到所有任务数据内容
QMap<QString,TaskInfoStru> RWRedisInterfaceLib::ReadCurTaskDataFromRedis()
{
    QMap<QString,TaskInfoStru> curtaskData;
    //先执行指令查找所有的数据列表
    if(!ExcIsOK("select 0"))
    {
        return curtaskData;
    }
    QString cmd  = "keys *";
    redisReply* lreply =(redisReply*)redisCommand(m_predistext,cmd.toStdString().c_str());
    for(uint j = 0 ; j < lreply->elements;j++)
    {
        QString key = lreply->element[j]->str;
        // qDebug()<<"comand key:"<< key;
        int len = 0;
        redisReply* replylen =(redisReply*)redisCommand(m_predistext,"HLEN %s",key.toStdString().c_str());
        if(replylen->type == REDIS_REPLY_INTEGER)
            len = replylen->integer;
        freeReplyObject(replylen);
        if(len >= 6) //做了判断 暂时这样处理
        {
            TaskInfoStru tstru;
            if(GetTaskInfoStru(key,tstru))
            {
                curtaskData.insert(key,tstru);
            }
        }
    }
    freeReplyObject(lreply);
    return curtaskData;
}
///
/// \brief RWRedisInterfaceLib::WriteTaskDatatoRedis
/// \param t
/// \return
///
bool RWRedisInterfaceLib::WriteTaskDatatoRedis(TaskInfoStru t)
{
    //写入数据根据结构体内容
    QString cmdstr = QString("HMSET %1 regCode %2 interfaceType %3 from %4 to %5 taskId %6 taskMainType %7 taskSonType  %8 status %9 receiptNo %10 receiptNoType %11 containerCode %12 fromLocationCode %13 toLocationCode %14 taskResult %15 nowLocationCode %16 abnormalLocationCode %17 inbToLocationCode %18 responseMessage %19 createTime \'%20\' requestTimes %21 ").\
            arg(t.regCode).arg(t.regCode).arg(t.interfaceType).arg(t.from).arg(t.end).arg(t.taskNum).arg(t.taskMainType).arg(t.taskSonType)
                             .arg(t.status).arg(t.receiptNo).arg(t.receiptNoType).arg(t.boxNum).arg(t.fromLocationCode).arg(t.shelves).arg(t.taskResult).arg(t.nowlocationCode)\
                             .arg(t.abnormalLocationCode).arg(t.inbToLocationCode).arg(t.responseMessage).arg(t.creatTime.toString()).arg(t.requestTimes);

          qDebug()<<"write value:"<<  cmdstr;
          if(m_predistext == NULL)
              return false;
          bool isok = false;
          redisReply* reply =(redisReply*)redisCommand(m_predistext,"HMSET %s regCode %s interfaceType %s from %s to %s taskId %s taskMainType %s taskSonType  %s status %s receiptNo %s receiptNoType %s containerCode %s fromLocationCode %s toLocationCode %s taskResult %s nowLocationCode %s abnormalLocationCode %s inbToLocationCode %s responseMessage %s createTime %s requestTimes %s ",\
                                                    t.regCode.toStdString().c_str(), t.regCode.toStdString().c_str(),t.interfaceType.toStdString().c_str(),\
                                                       t.from.toStdString().c_str(),t.end.toStdString().c_str(),t.taskNum.toStdString().c_str(),t.taskMainType.toStdString().c_str(),\
                                                       t.taskSonType.toStdString().c_str(),\
                                                       t.status.toStdString().c_str(),t.receiptNo.toStdString().c_str(),t.receiptNoType.toStdString().c_str(),\
                                                      t.boxNum.toStdString().c_str(), t.fromLocationCode.toStdString().c_str(),t.shelves.toStdString().c_str(),\
                                                       t.taskResult.toStdString().c_str(),t.nowlocationCode.toStdString().c_str(),t.abnormalLocationCode.toStdString().c_str(),\
                                                       t.inbToLocationCode.toStdString().c_str(),t.responseMessage.toStdString().c_str(),\
                                                       t.creatTime.toString().toStdString().c_str(),t.requestTimes.toStdString().c_str());
           if(reply->type == REDIS_REPLY_ERROR)
                return false;
          if(QString(QLatin1String(reply->str)) == "OK")
              isok = true;
          freeReplyObject(reply);
          return isok;
          // return ExcIsOK(cmdstr);
}
///
/// \brief RWRedisInterfaceLib::DelCurTaskDataFromRedis
/// \param taskid
/// \return
///
bool RWRedisInterfaceLib::DelCurTaskDataFromRedis(QString taskid)
{
    if(ExcIsOK("select 0"))
    {
        QString cmd  =  QString("HDEL %1").arg(taskid);
        if(ExcIsOK(cmd))
        {
            return true;
        }
    }
    return false;
}


bool RWRedisInterfaceLib::ExcIsOK(QString str)
{
    if(m_predistext == NULL)
        return false;
    bool isok = false;
    redisReply* reply =(redisReply*)redisCommand(m_predistext,str.toStdString().c_str());
     if(reply->type == REDIS_REPLY_ERROR)
          return false;
    if(QString(QLatin1String(reply->str)) == "OK")
        isok = true;
    freeReplyObject(reply);
    return isok;
}
///
/// \brief RWRedisInterfaceLib::GetTaskInfoStru
/// \param key
/// \param stru
/// \return
///
bool RWRedisInterfaceLib::GetTaskInfoStru(QString key, TaskInfoStru &stru)
{
    bool isok = true;
    QString strcmd = QString("HGETALL %1").arg(key);
    redisReply* getreply =(redisReply*)redisCommand(m_predistext,strcmd.toStdString().c_str());
    for(uint i = 0; i < getreply->elements; ++i)
    {
        //字段的值
        strcmd = QString("HGET %1 %2").arg(key).arg(QString(getreply->element[i]->str));
        redisReply* getreplyvalue =(redisReply*)redisCommand(m_predistext,strcmd.toStdString().c_str());
        QString name =  getreply->element[i]->str;
        QString value = getreplyvalue->str;
       // qDebug()<<"name :" <<name << "value:"<<value;
        if(name =="regCode")
        {
            stru.regCode = value;
        }
        if(name =="interfaceType")
        {
            stru.interfaceType = value;
        }
        if(name=="from")
        {
            stru.from = value;
        }
        if(name=="to")
        {
            stru.end = value;
        }
        if(name=="taskId")
        {
            stru.taskNum = value;
        }
        if(name=="taskMainType")
        {
            stru.taskMainType = value;
        }
        if(name=="taskSonType")
        {
            stru.taskSonType = value;
        }
        if(name=="status")
        {
            stru.status = value;
        }
        if(name=="receiptNo")
        {
            stru.receiptNo = value;
        }
        if(name=="receiptNoType")
        {
            stru.receiptNoType = value;
        }
        if(name=="containerCode")
        {
            stru.boxNum = value;
        }
        if(name=="fromLocationCode")
        {
            stru.fromLocationCode = value;
        }
        if(name=="toLocationCode")
        {
            stru.shelves = value;
        }
        if(name =="taskResult")
        {
            stru.taskResult = value;
        }
        if(name =="nowLocationCode")
        {
            stru.nowlocationCode = value;
        }
        if(name =="abnormalLocationCode")
        {
            stru.abnormalLocationCode = value;
        }
        if(name =="inbToLocationCode")
        {
            stru.inbToLocationCode = value;
        }
        if(name =="responseMessage")
        {
            stru.responseMessage = value;
        }
        if(name =="createTime")
        {
            // stru.creatTime = value;
        }
        if(name =="requestTimes")
        {
            stru.requestTimes = value;
        }
        i++;
        freeReplyObject(getreplyvalue);
    }
    freeReplyObject(getreply);
    return isok;
}
