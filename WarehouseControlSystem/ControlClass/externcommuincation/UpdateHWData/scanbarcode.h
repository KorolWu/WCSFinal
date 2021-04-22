#ifndef SCANBARCODE_H
#define SCANBARCODE_H

#include <QObject>
#include <datastructure.h>
#include <Myconfig.h>
class ScanBarCode:public QObject
{
    Q_OBJECT
public:
    ScanBarCode();
    int SetScanBarParamByFrame(QByteArray tempData, int ID);
    void Updateconnectstate(int id,bool state);
private:
     QMutex m_mutex;
};

#endif // SCANBARCODE_H
