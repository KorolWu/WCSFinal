#ifndef CarStatusFrom_H
#define CarStatusFrom_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QFont>
#include <QPixmap>
#include <QVBoxLayout>
#include <QTimer>
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>
#include <QTableWidget>
#include <datastructure.h>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLineEdit>
#include <QSpinBox>
#include <QMouseEvent>
#include "../../ControlClass/observerbase.h"
#include "KDeviceSingleton.h"
#include "Myconfig.h"
#include <QMessageBox>
#include <QThreadPool>
#include <QRandomGenerator>
#include <ControlClass/kdispatch.h>
#include <ControlClass/Instructions/carinstruction.h>
#include <ControlClass/Instructions/abstructinstruction.h>
#include <ControlClass/WarehousingIn/generatechargingtrajectory.h>
class CarStatusFrom : public QWidget,ObserverBase
{
    Q_OBJECT
public:
    explicit CarStatusFrom(int carId, QWidget *parent = 0);

public slots:
    void fromClose();
    void relative_move_x();
    void relative_move_y();
    void left_pickup();
    void right_pickup();
private:
     int PAD_X = 550;
     int PAD_Y = 337;
    QRect desktop;
    QLabel *header_lab;
    QLabel *car_image;
    QLabel *car_number;
    QLabel *battery_lab;
    QLabel *battery_image;
    QLabel *online_image;
    QLabel *online_lab;
    QLabel *l ;
    QLabel *err_lab;
    QLabel *ready_lab;
    QLabel *working_lab;
    QLabel *notReady_lab;
    QLabel *m_pLeft_haveBox;
    QLabel *m_pRight_haveBox;
    QLabel *m_pTop_haveBox;
    QLabel *m_pExe_done;
    QLabel *m_pElectricity_low;
    QSpinBox *m_pLineEdit_x;
    QSpinBox *m_pLineEdit_y;
    QPushButton *m_pMove_x;
    QPushButton *m_pMove_y;
    QLabel *autoCalibration;


    QTableWidget *table;
    QPushButton *up_btn;
    QPushButton *down_btn;
    QPushButton *m_pLeftPutDown_btn;
    QPushButton *m_pRightPutDown_btn;
    QPushButton *m_pCharg_btn;

    QPushButton *closeBtn;
    QTableWidgetItem *m_crunt_item;
    int m_id;
    int     m_carNum;
    QTimer* timer;
    bool m_pressflag;
    QPoint m_beginP;
    QPoint m_windowP;
    bool check_y(int y);
    bool check_x(int x);
    bool check_move_state();
    // ObserverBase interface
    QPushButton *m_reconnectbtn;
    void SetBtnState(bool enble);
    void move_test(int type);
    void Delay_MSec(int msec);
private slots:
    void SlotReconnect();
    void onCharged();
public:
    void updateStatusOnBase();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void onPickup();
};

#endif // CarStatusFrom_H
