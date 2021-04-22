#ifndef UPDATEREALTIMEDATAOBJECT_H
#define UPDATEREALTIMEDATAOBJECT_H

#include <QObject>
#include <Myconfig.h>
#include <datastructure.h>
#include <MysqlDataBase/readtabledata.h>
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"

class UpdateRealtimeDataObject: public QObject
{
    Q_OBJECT
public:
    explicit UpdateRealtimeDataObject();
    ~UpdateRealtimeDataObject();
    //保存数据库
    void SaveStoreinfotoDatabase();
    //定时更新数据任务
    void RequestTimingupdateHWinfo();
    void RequestTimingupdateCarHWinfo();
    void UpdateStackerDataRequest();
private:
    ReadTableData m_databaseopob;
    void UpdateCarDataRequest();
    void UpdateRunnerDataRequest();
    void UpdateElevatorDataRequest();   
    QVector<QString> mRstackerVar;
    void GetRstackerVar();
    void SetStackerhandshake(int16_t value,int id);
    int16_t mhandShakevalue;
};

#endif // UPDATEREALTIMEDATAOBJECT_H
