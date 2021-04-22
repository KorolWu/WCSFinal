#ifndef CARINSTRUCTION_H
#define CARINSTRUCTION_H

#include <ctime>
#include <time.h>
#include <QObject>
#include <QThread>
#include "Myconfig.h"
#include <sys/time.h>
#include <QApplication>
#include "datastructure.h"
#include "abstructinstruction.h"
#include <MysqlDataBase/crudbaseoperation.h>
#include "../externcommuincation/tcommtransceivermanager.h"
/**
 * @brief The CarInstruction class
 * @author korol
 * @title 负责小车 X Y的运动
 */
class  CarInstruction : public AbstructInstruction
{
public:
    CarInstruction();
private:
    int     m_id;
    int     m_result;
    int     m_tempValue;
    QString m_instructMsg;
    OrderStru   m_order;
    // AbstructInstruction interface
public:
    void runInstruction();
    int getResult(QString &exeMsg);
    void setParameter(OrderStru o, int device_id);
};

#endif // CARINSTRUCTION_H
