#ifndef JESON2OBJECT_H
#define JESON2OBJECT_H
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include "datastructure.h"
class Jeson2Object
{
public:
    Jeson2Object();
    //Jeson2Object(QString str,QMap<uint, MDetail *> &map);
    void parseJsonFile(const QString &fileName);
    void parseJsonObject(QJsonArray::const_iterator iter);
    void parseJsonObjectI(QJsonObject &object);
    void parseJsonObjectV(QJsonObject &object);
    void initList();
    QByteArray makeBody(const TaskInfoStru &t, const int interfaceType, QString nowLocation, bool result, QString errMsg);
    QList<QString> m_iList;
    QList<QString> m_vList;

    //QMap<uint, MDetail *> m_Object_map;

};

#endif // JESON2OBJECT_H
