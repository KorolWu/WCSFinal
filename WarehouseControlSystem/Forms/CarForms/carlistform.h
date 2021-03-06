#ifndef CARLISTFORM_H
#define CARLISTFORM_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QLine>
#include <datastructure.h>
#include <Myconfig.h>
#include <QPoint>
#include <QMouseEvent>
#include <QPointer>
#include <QRect>
#include <QApplication>
#include <QtGui>
#include "CarStatusFrom.h"
#include <QTimer>
#include <QList>
#include "ControlClass/observerbase.h"
#include "ControlClass/basedevice.h"
#include "KDeviceSingleton.h"
#include <QMessageBox>
#include "ControlClass/externcommuincation/tcommtransceivermanager.h"
/**
 * @brief The CarListForm class
 * show list of car
 */
class CarStatusWidget : public QWidget,ObserverBase
{
public:
    //CarStatusWidget(int width, CarInfoStru c,BaseDevice *o, QWidget *parent =0);
    CarStatusWidget(int width, int carId, QWidget *parent =nullptr);
private:
    CarStatusFrom *m_pCar;
    CarInfoStru m_car;
    QWidget *w;
    int interval;
    QString m_ip;
    int y;
    int     m_carNum;
    QLabel *numLab;
    QLabel *staLab;
    QLabel *taskLab;
    QLabel *workLab;
    QLabel *batterLab;
    QPushButton *m_pResetBtn;
    QTimer *m_ptimer;
    void initUI();
    void onTimeoutUpdateUI();
    // QWidget interface
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
public:
    void onReseted();

    // ObserverBase interface
public:
    void updateStatusOnBase();

    // ObserverBase interface
};
class CarListForm : public QWidget
{
    Q_OBJECT
public:
    explicit CarListForm(int width,int height,QWidget *parent = 0);
    void initUi();
    void updateUI();

private:
    QWidget *m_pMainWidget;
    QLabel *m_pHeadLabel;
    QLabel *m_pInfoLabel;
    QVector<QLabel *> m_InfoVec;
    QPushButton *m_pMinBtn;
    int m_width;
    int m_height;
    bool m_pressflag;
    QPoint m_beginP;
    QPoint m_windowP;
    QTimer *m_pTimer;
    short m_onLineNum = 0;
    short m_disonLineNum = 0;
    short m_totalNum = 0;
    void initStatusForms();
    void updateCarListHeader();
    QList<CarStatusWidget *> list;
signals:
    void updateList();
    void minimize();
public slots:
    void onMinimize();
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);



};
#endif // CARLISTFORM_H
