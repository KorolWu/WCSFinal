#ifndef CURRENTTASK_H
#define CURRENTTASK_H

#include <QObject>
#include <QWidget>
#include "../basefrom.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDebug>
#include <datastructure.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QMenu>
#include "Myconfig.h"
#include "MysqlDataBase/crudbaseoperation.h"
#include "UnitClass/c_systemlogmng.h"
#include "UnitClass/jeson2object.h"
#include "ControlClass/Instructions/httpclien_k.h"
#include "ControlClass/Instructions/runnerinstruction.h"
#include <UnitClass/operationtask.h>
#include <QContextMenuEvent>

#include "UnitClass/redis/qRedis.h"
#include <rwredisinterfacelib.h>
/**
 *
 * @author korol
 * @email wuwenpeng@cowain.com.cn
 * @date 2020-06-12 13:53:59
 * use 显示当前正在执行的任务
 */
class CurrentTask : public BaseFrom
{
public:
    CurrentTask(int width,int height,QWidget *parent);
    void setTableViewValue();
    void refreshTable();
private:
//    QPushButton *m_in_btn;
//    QPushButton *m_out_btn;
//    QPushButton *m_all_btn;
//    QDateEdit   *m_start_date;
//    QDateEdit   *m_end_date;
//    QPushButton *m_query_btn;
    QTableView  *m_table_view;
    QStandardItemModel *model;
    QMenu *popMenu; //菜单
    QString m_regCode = "";
    int m_stationNum = 99;
    //qRedis *m_pRedisCli;
    void initTableView();
    void initMenu();
    void setColumnWidth();
    bool saveTaskToDB(TaskInfoStru taskStru);
    int width;
    int height;
    TaskInfoStru ParsingWMSFrame(QString frame);
    void reply2Wms(const TaskInfoStru &t);
    void putBox(int cache);
    void SetTableData();
    void pushBox(QString boxNum);
    RWRedisInterfaceLib *m_redisob;
    bool TaskSWtoCutTask(TaskInfoStru );
    bool verificationData(TaskInfoStru &t);
    void onTableViewClick(const QPoint &pos);
    QStringList m_beforeInlist;
public slots:
    void handelHttpTask(QString reply);
    void onDeleteActionClicked();
protected:
     void contextMenuEvent(QContextMenuEvent *event);
};

#endif // CURRENTTASK_H
