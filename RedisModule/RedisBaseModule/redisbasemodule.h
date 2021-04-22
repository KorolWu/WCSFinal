#ifndef REDISBASEMODULE_H
#define REDISBASEMODULE_H
#include <QObject>
#include <QMap>
#include <QVariant>
#include <hiredis/hiredis.h>
#include <QDateTime>
#include "hiredis/hiredis.h"
#include "redisbasemodule_global.h"
///
/// \brief The RedisBaseModule class
/// 本模块建立Reids链接，封装常用的函数
class REDISBASEMODULESHARED_EXPORT RedisBaseModule
{

public:
    bool getConnect();
    //-----------------HashMap----------------
    int hashHlen(const QString& key);
    redisReply *execommand(QString &command);
    QMap<QString,QVariant> hgetall(QString key);
    RedisBaseModule(const char *ip, const int port);
    bool hmset(QString key, QMap<QString,QVariant> map);
    int hashHexists(const QString& key,const QString& field);
    QString hashHget(const QString& key,const QString& field);
    bool hashHset(const QString &key, const QString &field , const QString& value);

    //-----------------set get----------------
    QString get(const QString &cmmand);
    bool set(const QString &field, const QString &value);
    redisContext* GetredisContext(){
       return m_prct;
    }

    //-----------------list-------------------
    bool lpush(const QString &key,const QString &field);
    QList<QString> lange(const QString &key,const int start=0,const int end=1000);
    bool insert_log(const QString log);
private:
    char* m_ip;
    int m_port;
    redisContext* m_prct;
};

#endif // REDISBASEMODULE_H
