#ifndef ABSTRUCTINSTRUCTION_H
#define ABSTRUCTINSTRUCTION_H

#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include "datastructure.h"
#include "Myconfig.h"

class AbstructInstruction
{
public:
    AbstructInstruction();
    virtual ~AbstructInstruction();
    virtual void setParameter(OrderStru o, int device_id);
    virtual int getResult(QString &exeMsg);
    virtual void runInstruction();

public:
    bool isTerminate = false;
    void Delay_MSec(unsigned int msec);

};

#endif // ABSTRUCTINSTRUCTION_H
