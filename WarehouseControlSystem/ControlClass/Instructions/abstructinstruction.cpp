#include "abstructinstruction.h"

AbstructInstruction::AbstructInstruction()
{

}

AbstructInstruction::~AbstructInstruction()
{

}

void AbstructInstruction::setParameter(OrderStru o, int device_id)
{
    Q_UNUSED(o)
    Q_UNUSED(device_id)
}
///
/// \brief AbstructInstruction::getResult
/// \return 0 success 1 fail
///
int AbstructInstruction::getResult(QString &exeMsg)
{
    exeMsg = "";
   return 0;
}

void AbstructInstruction::runInstruction()
{

}

void AbstructInstruction::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
