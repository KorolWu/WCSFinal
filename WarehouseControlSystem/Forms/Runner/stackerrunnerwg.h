#ifndef STACKERRUNNERWG_H
#define STACKERRUNNERWG_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <Myconfig.h>
#include <QGridLayout>
#include <QSplitter>
#include <QTimer>
class StackerRunnerWg: public QWidget
{
    Q_OBJECT
public:
    explicit  StackerRunnerWg(QWidget *parent = nullptr);
    void SetParam(int runnerid,int outrunnerid);// 用于显示界面的流道的ID
    ~StackerRunnerWg();
private:
    QMap<QString, QMap<QString,QLabel*>> m_RnamesMap;
    void UpRunnerstatebit();
    int m_runnerid;
    int m_outrunnerid;
    QLabel *mconnectlabel[2];
    QLabel *mIdlabel[2];
    QPushButton* mConnectButton[2];
    QLineEdit *mRunnerIP[2];
    QTimer *m_updateTimer;
    QMap<QString,QVector<QLabel *>> m_bitstateMap;
    QComboBox *  m_RunercmdComboBox;
    QComboBox *  m_runneridCombox;
    QPushButton*m_startcmdBtn;
    void UpdateRunnerState(QString runnername);
    void UpdateLabelState(int value ,  QLabel * label);
    void UpdateConnectState(QString runnername);
private slots:
    void SlotUpdateState();
    void slotCmdDispatch();
};

#endif // STACKERRUNNERWG_H
