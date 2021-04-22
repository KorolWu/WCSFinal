#ifndef MYCONFIG_H
#define MYCONFIG_H
#include "datastructure.h"
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QQueue>
#include <datastructure.h>

class Myconfig
{
private:
    Myconfig()
    {
        //m_layerIsLocked<<false<<false<<false<<false<<false<<false<<false<<false<<false;
    }
public:
    static Myconfig* GetInstance()
    {
        static Myconfig Instance;
        return &Instance;
    }
    void InsertLog(int level,QString from,QString info)
    {
        LogInfoStru l;
        l.level = level;
        l.from = from;
        l.info = info;
        m_logQue.append(l);
    }

public:
    //carinfo < 小车编号,car_info>
    QMap<int, CarInfoStru> m_CarMap;
    QMutex m_carMap_mutex;
    QMap<int, ElevatorInfoStru> m_elevatorMap;
    //QMap<QString, BaseDevice *> m_carCommnicationMap;
    QMutex m_mutex;
    QMutex m_elevator_mutex; //电梯锁
    QMap<QString, StorePosInfoStru> m_storeinfoMap;
    QMutex m_rmutex;
    //taskNum,task 存储所有的任务用来显示和整理
    QMap<QString, TaskInfoStru> m_taskMap;
    QMutex m_mutex_taskMap;
    //2021/02/05 电梯专用锁
    QMutex m_elevatorMutex;
    //task queue
    QQueue<TaskInfoStru> m_taskQueue;//出库任务队列
    QMutex m_task_mutex;
    //in_task map<box_num,task>
    QMap<QString,TaskInfoStru> m_in_taskMap; //入库任务队列
    //增加以站号Key为基准出入库的数据结构 2021/01/08 新增此结构
    QMap<int,QQueue<TaskInfoStru>> m_OuttaskMap;
    QMap<int,QMap<QString,TaskInfoStru>> m_IntaskMap;
    //task queue 用来存储出库任务
    //用来记录最新的入库扫码信息
    QString m_boxNum_in = "BOX0001";
    QMutex m_in_task_mutex;
    DataBaseInfoStru m_databaseInfo;
    RedisInfoStru m_redisInfo;
    bool m_flag = false; //暂停
    bool m_exit = true; //退出应用程序
    QQueue<LogInfoStru> m_logQue;
    //
    QMutex m_mutex_sqlwrite;

    //runer
    RunerStru m_runer;
    //判断当前层是否锁住
    QMap<int,LayerStru> m_layerStatusMap;
    //指定小车任务的集合
    QMap<int, QQueue<TaskInfoStru>> m_appointMap;
    //硬件通讯配置的数据结构体,从数据库中读出来的数据内容
    HWComallconfigStru m_hwcommstru;
    //run mode =0 is autoMode
    int m_run_mode = 1;
    //流道的缓存位1-8，是否有料箱 有true  无false；
    QMap<int ,bool> m_cacheRunerMap;
    QMap<int ,ScanInfoStru> m_ScanMap;
    //red from db,about requery parameter<pleace(out,or in),parameter>
    QMap<QString ,HttpParameter> m_requestParameter;
    //用户权限的等级
    char m_curLoginlevel = 1;
    //
    int m_pixel = 4;
    QString m_byteorder = "high";

    //出库URL
    QString outOfWarehouseUrl = "";
   //入库URL
    QString inOfWareHouseUrl = "";
    //用来存储 条码对应的位置，小车复位时会用到这个数据
    QMap<int,KPosition> m_codeResiveMap;
    QMap<int,StackerInfo> m_stackerMap;
    //增加流道数据结构 <流道的站号(和硬件的Id相等),流道具体的数据> 2021 01 22
    QMap<int,RunerStru> m_RunnerMap;
    QString m_SrmWritePrefix = "";
    QString m_SrmReadPrefix = "";
};
#endif
