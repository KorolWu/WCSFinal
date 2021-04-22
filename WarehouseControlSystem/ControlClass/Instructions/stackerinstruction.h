#ifndef STACKERINSTRUCTION_H
#define STACKERINSTRUCTION_H

#include <QObject>
#include "abstructinstruction.h"
#include <Myconfig.h>
#include <QThread>
#include <QApplication>
enum StackerCommd
{
    None = 0,
    Carray = 1,
    Single_put = 2,
    Move = 3,
    Cancel = 4
};
enum CHECKVAR
{
    ACK = 0,
};
class StackerInstruction: public AbstructInstruction , public QObject
{
public:
    StackerInstruction();

    // AbstructInstruction interface
public:
    void setParameter(OrderStru o, int device_id);
    void runInstruction();
    int  getResult(QString &exeMsg);
public slots:
    void Run();
private:
    bool CheckStackerState(qint16 state = 0);
    void SendCmdTask();
    int SendWriteACK();
    int CheckTaskisFinished();
    int CheckACkSignal(qint16  expectvalue, int role);
    int m_id;
    OrderStru   m_order;
    int     m_result;
    QString mprefix;
};

#endif // STACKERINSTRUCTION_H
