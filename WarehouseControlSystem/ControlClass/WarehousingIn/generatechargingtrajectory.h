#ifndef GENERATECHARGINGTRAJECTORY_H
#define GENERATECHARGINGTRAJECTORY_H

#include <QObject>
#include <datastructure.h>
#include <QQueue>
#include <Myconfig.h>
///
/// \brief The GenerateChargingTrajectory class
///小车充电轨迹生成指令方式;
/// 默认小车的充电位置为电梯的等待位置
class GenerateChargingTrajectory:public QObject
{
     Q_OBJECT
public:
    GenerateChargingTrajectory();
    ~GenerateChargingTrajectory();
    void SetPathParam(KPosition task_P, KPosition curcarPosion);
    QQueue<OrderStru> GetChargingTrajectoryOrders();
private:
    QString GetorderNameByValue(Order value);
    QQueue<OrderStru> m_taskQueue;
    KPosition m_trapos;
    KPosition m_carPos;
    void GetCarchargingPath();
    double m_chargingposx;
    double m_chargingposy;
    double m_roady;
};

#endif // GENERATECHARGINGTRAJECTORY_H
