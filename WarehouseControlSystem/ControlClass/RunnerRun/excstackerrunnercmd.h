#ifndef EXCSTACKERRUNNERCMD_H
#define EXCSTACKERRUNNERCMD_H

#include <QObject>
#include <Myconfig.h>
#include <QDateTime>
#include <QEventLoop>
class ExcStackerRunnerCmd:public QObject
{
public:
    ExcStackerRunnerCmd();
    void SetExcRunnerParam(int runnerid,int scanid);
    void ExcRunning();
    void StopRun();
private:
    void ExcInRequest();
    void ExcScanInstruction(int device_id);
    int CheckScanIsok(int scanid,bool inflag=true);
    bool ScancodeCheck(int scanid);
    void ExcRunnerCmd(int device_id,int cmdname);
    bool IsStop;
    int  m_runnerid;
    bool m_changed;
    int m_scanid;
};

#endif // EXCSTACKERRUNNERCMD_H
