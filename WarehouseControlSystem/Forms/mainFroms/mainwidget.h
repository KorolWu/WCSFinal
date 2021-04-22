#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include "cpiewidget.h"
#include "torquechart.h"
#include "statusinfowidget.h"
#include <QList>
#include <QColor>
#include <math.h>
#include "warehouseprofileinfoshowwg.h"
/**
 * @brief The MainWidget class
 * show main info widget
 */
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(int width, int height, QWidget *parent = 0);
    ~MainWidget();

signals:
private:
    int m_width;
    int m_height;
    int m_totalErrCount = 0;
    QGridLayout *m_pgridlayout;
    QPushButton *m_ptest;
    QWidget     *m_pmain;
    CPieWidget  *m_piew;
    TorqueChart *m_tchar;
    CPieWidget  *m_perrwidget;
    //WarehouseprofileInfoShowWg *m_profileWidget;
    StatusInfoWidget *m_pstatusInfoWidget;
    void initUI();
    QList<QColor> m_colorlist;
    void SetErrwidgetInfo();
    void addTestPonit();
    QTimer *m_updatetimer;
    void UpdateWarehouseData();
    void UpdateAlarmlogData();
    void updateTaskData();
public:
    void appLog(QString str);
    void StartUpdateShowInfo();
    void CloseShowInfo();

public slots:
    void SlotUpdateData();
    void SlotUpdateTask(QString str);
};

#endif // MAINWIDGET_H
