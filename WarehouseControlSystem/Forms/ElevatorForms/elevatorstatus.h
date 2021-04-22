#ifndef ELEVATORSTATUS_H
#define ELEVATORSTATUS_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QTimer>
#include <Myconfig.h>
#include "../../ControlClass/Instructions/abstructinstruction.h"
#include "../../ControlClass/Instructions/carelevatorinstruction.h"

class ElevatorStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ElevatorStatusWidget(int elevatorId, QWidget *parent = 0);


signals:

public slots:
    void updateStatus();
private slots:
    void onCheckBoxStatusChange();
    void onResetClicked();
private:
    int m_elevatorId;
    void initUI();
    void onMove();
    QSpinBox *m_pCurrent_layer;
    QPushButton *m_pMove;
    QPushButton *m_pReset;
    QSpinBox *m_pOrder_layer;
    QLabel *m_pOnline_lab;
    QLabel *m_pLock_lab;
    QLabel *m_pOnline_image;
    QVBoxLayout *v;
    QCheckBox *m_pSelectRadioButton;
    QTimer *m_ptimer;


    QSpinBox *m_pcurachelayer;

};

#endif // ELEVATORSTATUS_H
