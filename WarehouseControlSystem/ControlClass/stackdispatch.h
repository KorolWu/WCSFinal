#ifndef STACKDISPATCH_H
#define STACKDISPATCH_H

#include <QObject>
#include <QThread>
#include <QThreadPool>
#include "Myconfig.h"
#include "kdispatch.h"
#include "selectcar.h"
#include "basedatainfooperate.h"
#include "MysqlDataBase/readtabledata.h"
#include "MysqlDataBase/crudbaseoperation.h"
#include "MysqlDataBase/crudbaseoperation.h"
#include "../ControlClass/Instructions/abstructinstruction.h"
#include "../ControlClass/Instructions/runnerinstruction.h"
#include "../ControlClass/Instructions/stackerinstruction.h"
///
/// \brief The StackDispatch class 用来调度和消费堆垛机的仓的任务，因为和穿梭车仓库的调度逻辑和复杂程度差异较大，特此分开
///
class StackDispatch : public QObject
{
    Q_OBJECT
public:
    explicit StackDispatch(QObject *parent = nullptr);
    void dispatchTaskThread();
signals:

public slots:

private:

    ReadTableData m_writeData;
    //流道上可以存放的最大数量
    TaskInfoStru m_task;
    void delayMSec(int msec);
    void handleTask(TaskInfoStru &task, int station);
    int errorCodeChange(const int errCode);
    // remove task
    void remove_out_task(int station);
    void remove_in_task(const QString &frist_in_boxNum, int station = 99);
    //流道上可以存放的最大数量
    const uint8_t MAX_OUT_CAHCHE = 4;
    //一层的站点的名称
    const QString IN_OUT_STATION = "1";
    //二层的站点名称
    const QString OUT_STATION = "2";
    //stack HW id
    const int     M_ID = 77;

    //一楼放目标站点的坐标
    const KPosition LAYER_PUTDOWN_1{88,8,8,8};
    //一楼取目标站点坐标
    const KPosition LAYER_PICKUP_1{88,8,8,8};
    //二楼方目标站点的坐标
    const KPosition LAYER_PUTDOWN_2{88,8,8,8};
    void writeFailedLog(const QString &message,const int errCode,const QString boxNum,QString taskId);

};

#endif // STACKDISPATCH_H
