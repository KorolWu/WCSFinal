#ifndef EXCRUNNEROUTCMD_H
#define EXCRUNNEROUTCMD_H

#include <QObject>
#include <Myconfig.h>
#include <QDateTime>
#include <QEventLoop>
#include <QTimer>

class ExcRunnerOutCmd:public QObject
{
public:
    ExcRunnerOutCmd();
    void SetExcRunnerParam(int id,int scanid);
    void ExcRun();
    void StopRun();
private:
    bool IsStop;
    void ExcRunnerForOutNew();
    void ExcRunnerForInNew();
   // void ExcScanCmd();
    void ExcScanInstruction(int device_id);
    int  CheckScanisOK();
    void ExcRunnerPass();
    void UpdateTempPos(int pos,QString codeinfo);
    int  CheckOutScanisOK();
     bool ScancodeCheck(bool isout = true);
    int  m_id;
    bool m_changeD6;
    bool m_changeD8;
    int m_scanid;
    qint64 m_time = 0;
};

#endif // EXCRUNNEROUTCMD_H
