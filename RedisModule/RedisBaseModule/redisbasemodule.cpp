#include "redisbasemodule.h"
#include <QDebug>

RedisBaseModule::RedisBaseModule(const char* ip, const int port)
{
    //memcpy(m_ip,ip,strlen(ip));
    m_port = port;
    m_prct = nullptr;
    m_ip = new char[64];
    strcpy(m_ip,ip);
}

bool RedisBaseModule::getConnect()
{
    struct timeval timeout={2,0};
    m_prct = (redisContext*)redisConnectWithTimeout(m_ip,m_port,timeout);
    if(m_prct == nullptr)
        return false;
    if(m_prct->err != 0)
        return false;
    return true;
}
///
/// \brief RedisBaseModule::hmset 同时将多个field-value对设置到哈希表key中
/// \param key 哈希表key
/// \param map 向key中写入的值
/// \return
///
bool RedisBaseModule::hmset(QString key, QMap<QString, QVariant> map)
{
    QString cmd("HMSET ");
    cmd.append(key);
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext())
    {
        i.next();
        cmd.append(" ");
        cmd.append(i.key());
        cmd.append(" ");
        cmd.append(i.value().toString());
    }
    redisReply * reply = execommand(cmd);
    if(reply == nullptr)
        return false;

    if(QString(QLatin1String(reply->str)) == "OK")
        return true;

    return false;
}
///
/// \brief RedisBaseModule::hashHexists 判断哈希表中的某个字段是否存在
/// \param key
/// \param field
/// \return 存在返回1 不存在返回0
///
int RedisBaseModule::hashHexists(const QString &key, const QString &field)
{
    int result = 0;
    QString cmd("HEXISTS ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(field);
    redisReply* reply =(redisReply*)redisCommand(m_prct,cmd.toStdString().c_str());
    if(reply->type == REDIS_REPLY_INTEGER)
        result = reply->integer;
    freeReplyObject(reply);
    return result;
}
///
/// \brief RedisBaseModule::hashHget
/// \param key   返回哈希表 key
/// \param field 中给定域 field 的值
/// \return 成功返回给定的域,否则NULL
///
QString RedisBaseModule::hashHget(const QString &key, const QString &field)
{
    QString tmp("");
    QString cmd("HGET ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(field);
    QByteArray buff = cmd.toLatin1();
    redisReply* reply =(redisReply*)redisCommand(m_prct,buff.data());
    if(reply->type == REDIS_REPLY_STRING)
        tmp = reply->str;
    freeReplyObject(reply);
    return tmp;
}
///
/// \brief RedisBaseModule::hashHset 修改哈希表中某个字段的值
/// \param key    hash的Key
/// \param field  key中的某个字段
/// \param value  想要写入的新值
/// \return true field之前不存在  之前存在值被覆盖
///
bool RedisBaseModule::hashHset(const QString &key, const QString &field, const QString &value)
{
    int result=0;
    QString cmd("HSET ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(field);
    cmd.append(" ");
    cmd.append(value);
    redisReply * reply = execommand(cmd);
    if(reply->type == REDIS_REPLY_INTEGER)
        result += reply -> integer;
    freeReplyObject(reply);
    return result !=0?true:false;
}
///
/// \brief RedisBaseModule::get
/// \param cmmand field
/// \return return field value
///
QString RedisBaseModule::get(const QString &cmmand)
{
    QString cmd("GET ");
    QString s(" ");
    cmd.append(cmmand);
    redisReply * reply = execommand(cmd);
    if(reply->type == REDIS_REPLY_STRING)
        s = reply->str;
    freeReplyObject(reply);
    return s;
}
///
/// \brief RedisBaseModule::set reids set cammand
/// \param field
/// \param value
/// \return
///
bool RedisBaseModule::set(const QString &field, const QString &value)
{
    QString cmd("SET ");
    QString s(" ");
    cmd.append(field);
    cmd.append(" ");
    cmd.append(value);
    redisReply * reply = execommand(cmd);
    if(reply->type == REDIS_REPLY_STRING)
        s = reply->str;
    freeReplyObject(reply);
    if(s == "OK")
        return true;
    return false;

}
///
/// \brief RedisBaseModule::lpush 向list key插入字段field
/// \param key
/// \param field
/// \return
///
bool RedisBaseModule::lpush(const QString &key, const QString &field)
{
     QString cmd("LPUSH ");
     cmd.append(key);
     cmd.append(" ");
     cmd.append("'");
     cmd.append(field);
     cmd.append("'");
     qDebug()<<cmd;
     redisReply * reply = (redisReply*)redisCommand(m_prct,cmd.toStdString().c_str());
     if(reply == nullptr)
         return false;
     return true;
}
///
/// \brief RedisBaseModule::lange
/// \param key   list字段值
/// \param start 开始位置默认为0
/// \param end   结束位置默认为1000
/// \return     以QList的形式返回，有序可重复
///
QList<QString> RedisBaseModule::lange(const QString &key, const int start, const int end)
{
    QList<QString> list;
    QString str;
    QString cmd("LRANGE ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString("%1").arg(start));
    cmd.append(" ");
    cmd.append(QString("%1").arg(end));
    redisReply *reply = (redisReply*)redisCommand(m_prct,cmd.toStdString().c_str());
    if(reply->type == REDIS_REPLY_ARRAY)
    {
        for(uint j = 0 ; j< reply->elements;j++)
        {
            str = reply->element[j]->str;
            list.append(str);
        }
        freeReplyObject(reply);
    }
     return list;
}


bool RedisBaseModule::insert_log(const QString log)
{
    if(m_prct == nullptr || m_prct->err != 0)
        return false;
    QString temp_str = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.mmm")+"] "+ log;
    redisReply* reply = NULL;
    reply = reinterpret_cast<redisReply *>(redisCommand(m_prct, "LPUSH WCS_LOG %s", temp_str.toStdString().c_str()));
    if(reply && reply->type != REDIS_ERR)
    {
        freeReplyObject(reply);
        return true;
    }
    return false;

}

///
/// \brief RedisBaseModule::execommand 指令执行的终极入口
/// \param command  命令语句
/// \return
///
redisReply *RedisBaseModule::execommand(QString &command)
{
    if(m_prct == nullptr)
        return nullptr;
    QByteArray buff = command.toLatin1();

     redisReply* reply =(redisReply*)redisCommand(m_prct,buff.data());
     qDebug()<<"comand:"<<command;
     if(reply == nullptr)
     {
         freeReplyObject(reply);
         return nullptr;
     }
     else
         return reply;
}
///
/// \brief RedisBaseModule::hgetall 根据hash的key 获取里面所有的键值对，并以map的形式返回
/// \param key   hash的key
/// \return
///
QMap<QString, QVariant> RedisBaseModule::hgetall(QString key)
{
    QString cmd("HGETALL ");
    cmd.append(key);

    redisReply* reply = execommand(cmd);
    QString k,v;
    QMap<QString,QVariant> keypairs;
    if(reply->type == REDIS_REPLY_ARRAY)
    {
        for(uint j = 0 ; j< reply->elements;j++)
        {
            k = reply->element[j]->str;
            v = hashHget(key,reply->element[j]->str);
            keypairs.insert(k,v);
            j++;
        }
        freeReplyObject(reply);
    }
    return keypairs;
}
///
/// \brief RedisBaseModule::hashHlen 返回哈希表中域的数量
/// \param key
/// \return count of hash
///
int RedisBaseModule::hashHlen(const QString &key)
{
    int result = 0;
    QString cmd("HLEN ");
    cmd.append(key);
    redisReply* reply = execommand(cmd);
    if(reply->type == REDIS_REPLY_INTEGER)
    {
        result = reply->integer;
    }
    return result;
}
