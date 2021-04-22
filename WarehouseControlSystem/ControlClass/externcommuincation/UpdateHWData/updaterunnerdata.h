#ifndef UPDATERUNNERDATA_H
#define UPDATERUNNERDATA_H

#include <QObject>
#include <datastructure.h>
#include <Myconfig.h>
#include <QMutex>
#include <QMutexLocker>
enum statebit
{
    Runnerstate = 1,//流道状态
    cache_in_current = 2,//入库缓存
    cache_out_current  = 3,//出库缓存
    statebitmap = 4,//状态位图
    codeinfo = 5,// 条码信息数据
};
class UpdateRunnerData:public QObject
{
    Q_OBJECT
public:
    UpdateRunnerData();
    ~UpdateRunnerData();
    int SetRunnerParamByFrame(int datatype, QMap<int, int> Data);
    void Updateconnectstate(int id,bool value);
    int SetRunnParamByPiFrame(QByteArray dataframe, int ID);
    bool GetSendCmdIndex(int ID, int frametype,uint16_t &index);
private:
    QMutex m_mutex;
    QList<uint16_t> m_actIndexlist;
    QList<uint16_t> m_stateIndexlist;
    uint16_t m_sendstateindex;
    uint16_t m_actiontindex;
    uint16_t m_runnerID;
    void AnalysisFrameData(QByteArray frame,int id);
    void UpdateRunnerDataByrole(uint16_t id,statebit role, uint8_t value,QString codeinfo);
    void UpdateRunnerDataByrole(uint16_t id,statebit role, uint8_t *value);
    int D2;
    void HighByteorderconversion(QByteArray &frame);

};
#endif // UPDATERUNNERDATA_H
