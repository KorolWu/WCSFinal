#include "jeson2object.h"
#include <QDebug>
Jeson2Object::Jeson2Object()
{

}

//Jeson2Object::Jeson2Object(QString str,QMap<uint, MDetail *> &map)
//{
//   parseJsonFile(str);
//   map = m_Object_map;
//}
void Jeson2Object::initList() {
    m_iList << "I1" << "I2" << "I3" << "I4" << "I5";

}

QByteArray Jeson2Object::makeBody(const TaskInfoStru &t, const int interfaceType, QString nowLocation, bool result, QString errMsg)
{
    QVariantHash data;
    data.insert("regCode",t.regCode);
    data.insert("interfaceType",QString("%1").arg(interfaceType));
    data.insert("from",t.end);
    data.insert("to",t.from);
    data.insert("taskId",t.taskNum);
    data.insert("taskMainType",t.taskMainType);
    data.insert("taskSonType",t.taskSonType);
    data.insert("status","执行中");
    data.insert("receiptNo",t.receiptNo);//wms number
    data.insert("receiptNoType",t.receiptNoType);//wms number type
    data.insert("containerCode",t.boxNum);
    data.insert("fromLocationCode",t.fromLocationCode);
    data.insert("toLocationCode",t.shelves);//tager position
    data.insert("taskResult",result? "否":"是");
    data.insert("nowLocationCode",nowLocation);
    if(result == false)
        data.insert("abnormalLocationCode",nowLocation);
    else
        data.insert("abnormalLocationCode","");
    data.insert("inbToLocationCode","");
    data.insert("responseMessage",errMsg);
    data.insert("createTime",t.creatTime.toString("yyyy-MM-dd hh:mm:ss"));
    data.insert("requestTimes",t.requestTimes);
    QJsonObject rootObj = QJsonObject::fromVariantHash(data);
    QJsonDocument document;
    document.setObject(rootObj);
    return document.toJson(QJsonDocument::Compact);
}
void Jeson2Object::parseJsonFile(const QString &json_str)
{
     m_vList << "name" << "url";
    QByteArray buf = json_str.toLatin1();
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(buf, &jsonParseError));
    if(QJsonParseError::NoError != jsonParseError.error)
    {
        qDebug() << QString("JsonParseError: %1").arg(jsonParseError.errorString());
        return;
    }
    QJsonObject rootObject = jsonDocument.object();
    if(!rootObject.keys().contains("data"))
    {
        qDebug() << "No target value";
        return;
    }
    QJsonValue jsonValue = rootObject.value("data");
    QJsonObject obj = jsonValue.toObject();
    if(!obj.keys().contains("场景1"))//variable
    {
        qDebug() << "No target value -- terminal1";
        return;
    }
    jsonValue = obj.value("场景1");
    if(!jsonValue.isArray())
    {
        qDebug() << "No target array";
    }
    QJsonArray jsonArray = jsonValue.toArray();
    for(auto iter = jsonArray.constBegin(); iter != jsonArray.constEnd(); ++iter)
    {
        parseJsonObject(iter);
    }
}
void Jeson2Object::parseJsonObject(QJsonArray::const_iterator iter) {
    QJsonObject jsonObject = (*iter).toObject();
    QString name = "";
    //QString mettState ="";
    QString mettTime_e = "";
    QString mettTime_s ="";
    QString topic ="";
    if(jsonObject.keys().contains("WorkerNumber"))
    {
        name = jsonObject.value("WorkerNumber").toString();
    }
    if(jsonObject.keys().contains("StartTime"))
    {
        mettTime_s = jsonObject.value("StartTime").toString();
    }
    if(jsonObject.keys().contains("EndTime"))
    {
       mettTime_e = jsonObject.value("EndTime").toString();
    }
    if(jsonObject.keys().contains("topic"))
    {
        topic = jsonObject.value("topic").toString();
    }
    QDateTime start_time = QDateTime::fromString(mettTime_s, "yyyy-MM-dd hh:mm:ss");
//    uint int_start_time = start_time.toTime_t();
//    MDetail *ob = new MDetail(name,topic,"",mettTime_s,mettTime_e);
//    m_Object_map.insert(int_start_time,ob);

}
void Jeson2Object::parseJsonObjectI(QJsonObject &object)
{
    qDebug() << object.keys();
    for(int i = 0; i < object.keys().size(); ++i)
    {
        QJsonValue jsonValue = object.value(object.keys().at(i));
        if(jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            if(jsonObject.keys().contains("quality"))
            {
                qDebug() << QString("%1 : %2").arg("quality").arg(jsonObject.value("quality").toDouble());
            }
            if(jsonObject.keys().contains("temp"))
            {
                qDebug() << QString("%1 : %2").arg("temp").arg(jsonObject.value("temp").toDouble());
            }
        }
    }
    qDebug() << object.keys();
}
void Jeson2Object::parseJsonObjectV(QJsonObject &object) {
    for(int i = 0; i < object.keys().size(); ++i)
    {
        QJsonValue jsonValue = object.value(object.keys().at(i));
        if(jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            if(jsonObject.keys().contains("value"))
            {

                qDebug() << QString("%1 : %2").arg("value").arg(jsonObject.value("value").toInt());
            }
        }
    }

}
