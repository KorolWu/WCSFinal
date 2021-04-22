#ifndef UPDATEELEVATORDATA_H
#define UPDATEELEVATORDATA_H

#include <QObject>
#include <datastructure.h>
#include <Myconfig.h>
class UpdateElevatorData:public QObject
{
    Q_OBJECT
public:
    UpdateElevatorData();
    ~UpdateElevatorData();
    int SetElevatorParamByFrame(int id, QMap<int, int> Data);
    void Updateconnectstate(int id,bool value);
};

#endif // UPDATEELEVATORDATA_H
