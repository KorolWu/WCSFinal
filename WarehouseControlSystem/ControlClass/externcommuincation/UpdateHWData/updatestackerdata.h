#ifndef UPDATESTACKERDATA_H
#define UPDATESTACKERDATA_H
#include <QVariant>
#include <datastructure.h>
#include <Myconfig.h>
#include <QObject>

class UpdateStackerData:public QObject
{
      Q_OBJECT
public:
    UpdateStackerData();
    void Updateconnectstate(int id,bool value);
    void SetStackerParam(int id,QMap<QString,QVariant> valueData);
   qint32 GetTaskIndex(int role = 0);
private:
    qint32 m_sendcarryindex;
    QString mRprefix;
};

#endif // UPDATESTACKERDATA_H
