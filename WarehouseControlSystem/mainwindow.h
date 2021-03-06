#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Forms/CarForms/CarStatusFrom.h>
#include <QRect>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <Forms/CarForms/adddevice.h>
#include <Forms/CarForms/agvform.h>
#include <Forms/ElevatorForms/elevatorfrom.h>
#include "Forms/TaskForms/currenttask.h"
#include "Forms/TaskForms/alltask.h"
#include "Forms/TaskForms/subtask.h"
#include "Forms/Dispatch/dispatchwidget.h"
#include <Forms/basefrom.h>
#include "Forms/StorePosInfo/storeinfowidget.h"
#include "Forms/LogForms/logforms.h"
#include <QTimer>
#include <QEventLoop>
#include <MysqlDataBase/readtabledata.h>
#include <MysqlDataBase/crudbaseoperation.h>
#include <QCoreApplication>
#include <QtGui>
#include <QDir>
#include "UnitClass/c_systemlogmng.h"
#include "UnitClass/logmanager.h"
#include "UnitClass/khttpserver.h"
#include "JQHttpServer.h"
#include "ControlClass/basedevice.h"
#include "KDeviceSingleton.h"
#include "datastructure.h"
#include "Forms/CarForms/carlistform.h"
#include "Forms/AlarmInfo/alarminfowg.h"
#include "Forms/Runner/runerform.h"
#include "Forms/mainFroms/mainwidget.h"
#include "ControlClass/dispatchcenter.h"
#include "MointorUI/monitorui.h"
#include "SimulationgenerationtaskUI/testcreateorderspath.h"
#include <QMessageBox>
#include "updaterealtimedataobject.h"
#include "Forms/UserInfo/userinfowg.h"
#include "ControlClass/RunnerRun/excrunneroutcmd.h"
#include "Forms/StackerWg/stackerwg.h"
#include "Forms/Runner/stackerrunnerwg.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QRect desk_rect;
    QWidget *treewidget;
    QStandardItemModel *p_treeStandarModel;
    QStandardItem *p_standarItem;
    QStandardItem *p_ordersItem;
    QStandardItem *p_agvItem;
    QStandardItem *p_elevator;
    QStandardItem *p_runer;
    QStandardItem *p_all_tasks;
    QStandardItem *p_current_tasks;
    QStandardItem *p_sub_tasks;
    QStandardItem *p_dispatch;
    QStandardItem *p_loginroleItem;
    QStandardItem *p_main_from;
    QStandardItem *p_map;
    QStandardItem *p_stacker;
    QStandardItem *p_stackerRunner;
    QTreeView     *p_treeView;
    QPushButton   *exit_btn;
    QPushButton   *user_btn;
    QPushButton   *m_pmode_btn;
    QPushButton   *m_psuspend; //zan ting
   // QLabel        *m_pmode_info_label;
    QLabel        *m_list_lab;
    QWidget       *p_main_widget;
    CurrentTask   *t;
    AlarmInfoWg   *m_palarmWg;
    LogForms      *m_pLog;
    AgvForm       *car_from;
    AllTask       *m_pTaskAll;
    SubTask       *m_pSubTask;
    ElevatorFrom  *p_mElevator;
    RunerForm     *p_mRunerForm;
    CarListForm   *p_mCarList;
    UserInfoWg    *p_mUserManagerWg;
    QThread       *p_mDispatchThread;
    QThread       *Runnerthread;
    MainWidget    *m_pMainWidget;
    JQHttpServer::TcpServerManage *m_pHttpServer;
    DispatchWidget *m_pDispatchForm;
    bool           m_listIsShow;

    void closeWcs();
    void closeObject();
    void initUI();
    void loadCarInitialLayer();
    void checkReidsConnected();
    void deleteChildrenList();
    void getParameterFromDB();
    void getConfigParameter();
    void delay_msc(int msc);
    QByteArray getReply();
private slots:
    void onTreeviewClicked(const QModelIndex &index);
    void slotlogin();
    void onReplyReady(QString str);
    void showCarList();
    void changeMode();
    void onSuspend();
    void slotCommError(QString info);
    void initRunnerD2();
private:
    StoreInfoWidget *m_pstoreWg;
    MonitorUI *m_pmonitorui;
    StackerWg *m_stackerWg;
    StackerRunnerWg *m_stackerRunnerWg;

    //test
    TestcreateOrdersPath *m_testorders;
    UpdateRealtimeDataObject m_updateRealData;
    QTimer *m_updatestoretimer;
    QTimer *m_updateComTimer;
    QTimer *m_updateCarComTimer;
     QTimer *m_updateStackerComTimer;
    ExcRunnerOutCmd * m_ExcrunnerOb;
signals:
    void httpRedReady(QString str);
    void dispatched();
    void SignaldisRunner();

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
