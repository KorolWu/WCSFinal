#ifndef RWREDISINTERFACELIB_H
#define RWREDISINTERFACELIB_H

#include "rwredisinterfacelib_global.h"
#include  <redisbasemodule.h>
#include  <Myconfig.h>
#include  <hiredis/hiredis.h>


class RWREDISINTERFACELIBSHARED_EXPORT RWRedisInterfaceLib
{

public:
    RWRedisInterfaceLib();
    bool Connectredis(QString ip,int port);
    QMap<QString,TaskInfoStru> ReadCurTaskDataFromRedis(); //得到所有任务
    bool WriteTaskDatatoRedis(TaskInfoStru t);
    bool DelCurTaskDataFromRedis(QString taskid);
    redisContext *m_predistext;
    RedisBaseModule *GetRedisBaseModuleOb(){
        return  m_redismoduleob;
    }
private:
    RedisBaseModule *m_redismoduleob;
    bool ExcIsOK(QString str);
    bool GetTaskInfoStru(QString key,TaskInfoStru &stru);
};

#endif // RWREDISINTERFACELIB_H
