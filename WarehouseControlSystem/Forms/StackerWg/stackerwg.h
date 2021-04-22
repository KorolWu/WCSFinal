#ifndef STACKERWG_H
#define STACKERWG_H

#include <QObject>
#include <QWidget>
#include <QOpcUaClient>
#include <QOpcUaNode>
#include <memory>
#include <QTimer>
#include "opcuamodel.h"
#include "ControlClass/Instructions/stackerinstruction.h"
class QPlainTextEdit;
class QLineEdit;
class QComboBox;
class QPushButton;
class QOpcUaProvider;
class QTreeView;
class OpcUaModel;
class QLabel;
class StackerWg:public QWidget
{
     Q_OBJECT
public:
    explicit StackerWg(QWidget *parent = nullptr);
    ~StackerWg();
    void  setOpcUaClient(QOpcUaClient *);
    void SetOPCCommID(int ID);
private slots:
    void connectToServer();
    void clientDisconnected();
    void clientError(QOpcUaClient::ClientError);
    void clientState(QOpcUaClient::ClientState);
//    void slottestclient();
//    void handleAttributes(QOpcUa::NodeAttributes attr);
//    void readFinished(QVector<QOpcUaReadResult> results, QOpcUa::UaStatusCode serviceResult);
//    void WriteNodeIDValue(QString nodeid,QVariant value,int valuetype);
//    void testread();
//    void UpdateModelUI();
    void SlotUpdateState();
    void SlotrefreshTree();
    void SlotReadVariable();
    void SlotWriteVariable();
    void SlotExcCmd();
    void SlotExcEmpcmd();
    void SlotExcClrAlarmcmd();
    void SlotExcTestCmd();
    void SlotExcCancelTaskcmd();
private:
    void updateUiState();
    void UpateShowParam(QString key , QString valuestr,bool istext = true);
    void UpateShowParam();
    QString GetModeValueStr(qint16 value);
    QString GetStateValueStr(qint16 value);
    QVariant GetDatatypeValue(QString typestr,QString value);
    void  SetVarValue(int16_t value, QString name , QOpcUa::Types type);
    void StopStacker();
    QLabel *mconnectlabel;
    QLabel *mIdlabel;
    QPushButton *mConnectButton;
    QLineEdit *mServerUrl;
    QComboBox *mOpcUaPlugin;
    QPushButton *mRefreshButton;
    QPlainTextEdit *mLog;
    QLineEdit *m_nodeidEdit;
    QComboBox *mDatatypecombox;
    QLineEdit *m_valueEdit;
    QLineEdit *m_setSourceEdit;
    QLineEdit *m_setTraEdit;
    QPushButton *mReadButton;
    QPushButton *mWriteButton;
    QPushButton *mcmdButton;
    QPushButton *mEmpButton;
    QPushButton *mClrAlarmButton;
    QPushButton*mTestButton;
    QPushButton *mCancelTaskButton;
    QTreeView *mTreeView;
    OpcUaModel *mOpcUaModel;
    QOpcUaProvider *mOpcUaProvider;
    QOpcUaClient *mOpcUaClient;
    bool mClientConnected;
    int m_commID;
    QTimer *m_updateTimer;
    QMap<QString,QLabel *> m_RnamesMap;
    StackerInstruction *mstackercmd;
    QThread *mrunthread;
signals:
    void signalRun();
};

#endif // STACKERWG_H
