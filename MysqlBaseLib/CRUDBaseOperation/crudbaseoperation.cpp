#include "crudbaseoperation.h"
#include <QDebug>

CRUDBaseOperation *CRUDBaseOperation::Instance = new CRUDBaseOperation();
CRUDBaseOperation::CRUDBaseOperation()
{
}

bool CRUDBaseOperation::openDB(QString &err_msg)
{
    err_msg = "";
    data_base = QSqlDatabase::addDatabase(Myconfig::GetInstance()->m_databaseInfo.databaseName);
    data_base.setHostName(Myconfig::GetInstance()->m_databaseInfo.ip);
    data_base.setPort(Myconfig::GetInstance()->m_databaseInfo.port);
    data_base.setDatabaseName(Myconfig::GetInstance()->m_databaseInfo.sqlName);
    data_base.setUserName(Myconfig::GetInstance()->m_databaseInfo.userName);
    data_base.setPassword(Myconfig::GetInstance()->m_databaseInfo.passWord);
    if(data_base.isOpen())
    {
        err_msg = "The database is already open";
        return true;
    }
   if(!data_base.open())
    {
        err_msg = "open database fail";
        return true;
    }
   return false;
}

void CRUDBaseOperation::closeDB()
{
    if(data_base.isOpen())
        data_base.close();
    else
        return;
}

void CRUDBaseOperation::createTable(const QString &creatTableSql)
{
    QSqlQuery query(data_base);
    query.exec(creatTableSql);
}

QSqlQuery CRUDBaseOperation::queryDb(const QString &querySql)
{
    QSqlQuery query(data_base);
    query.exec(querySql);
    return query;
}

bool CRUDBaseOperation::queryUseStr(const QString &sqlStr)
{
    QSqlQuery query(data_base);
    return query.exec(sqlStr);
}

bool CRUDBaseOperation::saveCrruntTask(TaskInfoStru taskStru, QString &err)
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    err = "";
    if(data_base.isOpen())
    {
        QString sql =QString("INSERT t_crrunt_task SET taskNum=%1,taskStatus='%2',"
                             "boxNum='%3',priority='%4',origin='%5',"
                             "target='%6',carNum='%7';") .arg(taskStru.taskNum).arg(taskStru.status).arg(taskStru.boxNum)\
                                .arg(taskStru.pripty).arg(taskStru.from).arg(taskStru.end).arg(taskStru.carNum);
        QSqlQuery query(data_base);
        if(query.exec(sql))
       {
           return true;
       }
        else
        {
            err = query.lastError().text();
            return false;
        }

    }
    err = "database is not open !";
    return false;

}
///
/// \brief CRUDBaseOperation::removeCrruntTask ????????????????????????????????????????????????
/// \param taskStru
/// \return
///
bool CRUDBaseOperation::removeCrruntTask(TaskInfoStru taskStru, QString &err)
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    err = "";
    if(data_base.isOpen())
    {
        QString sql =QString("DELETE FROM t_crrunt_task WHERE taskNum=%1;").arg(taskStru.taskNum);
        QSqlQuery query(data_base);
        if(query.exec(sql))
       {
           return true;
       }
        else
        {
            err = query.lastError().text();
            return false;
        }

    }
    err = "database is not open !";
    return false;
}
///
/// \brief CRUDBaseOperation::saveCompletedTask ????????????????????????????????????????????????
/// \param taskStru
/// \return
///
bool CRUDBaseOperation::saveCompletedTask(TaskInfoStru taskStru, QString &err)
{
    err = "";
    if(data_base.isOpen())
    {
        QString sql =QString("INSERT t_all_task SET taskNum='%1',taskStatus='%2',"
                             "boxNum='%3',priority='%4',origin='%5',"
                             "target='%6',carNum='%7',beginTime = '%8';") .arg(taskStru.taskNum).arg(taskStru.status).arg(taskStru.boxNum)\
                                .arg(taskStru.pripty).arg(taskStru.from).arg(taskStru.shelves).arg(taskStru.carNum).arg(taskStru.creatTime.toString("yyyy-MM-dd hh:mm:ss"));
        QSqlQuery query(data_base);
        QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
         if(query.exec(sql))
        {
            return true;
        }
         else
         {
             err = query.lastError().text();
             return false;
         }

    }
    err = "database is not open !";
    return false;
}
///
/// \brief CRUDBaseOperation::changeSubtaskStatus ??????????????????????????????
/// \param taskNum ?????????
/// \param status  ?????????????????????
/// \param sequnce ????????????
/// \param err     sql??????
/// \return
///
bool CRUDBaseOperation::changeSubtaskStatus(const QString &taskNum, const QString &status, const QString &data,int sequnce, QString &err)
{
    err = "";
    QString sql = QString("UPDATE t_sub_taskInfo SET state = '%1',commandData = '%2',execDate = '%3' WHERE taskNum ='%4' AND sequence = %5;").arg(status).arg(data).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(taskNum).arg(sequnce);
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QSqlQuery query(data_base);
    //qDebug()<<sql;
    bool result = query.exec(sql);
    err = query.lastError().text();
    return result;
}
///
/// \brief CRUDBaseOperation::updateCarPosition
/// \param carId ?????????id
/// \param axis  ????????????
/// \param value ???????????????
/// \return
///
bool CRUDBaseOperation::updateCarPosition(const int carId, QString axis, int value, QString &sqlErr)
{
    axis = axis.toUpper();
    if(axis == "X")
        axis = "x";
    else if(axis == "Y")
        axis = "y";
    else if(axis == "Z")
        axis = "z";
    sqlErr = "";
    QString sql = QString("UPDATE t_device_info SET %1 = %2 WHERE id = %3;").arg(axis).arg(value).arg(carId);
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QSqlQuery query(data_base);
    bool result = query.exec(sql);
    sqlErr = query.lastError().text();
    return result;
}
///
/// \brief CRUDBaseOperation::saveKBaseStruct ?????? KBaseStruct ??????????????????
/// \param tableName
/// \param s
/// \return
///
bool CRUDBaseOperation::saveKBaseStruct(const QString &tableName, KBaseStruct &s, QString &errMessage)
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    if(s.getNameList().size() != s.getValueList().size())
        return false;
    QString sql = "INSERT "+tableName+" SET ";
    QString name;
    for(int i = 0;i < s.getNameList().size();i++)
    {
        name += s.getNameList()[i] + "="+"'"+ s.getValueList()[i].toString()+"'"+",";
    }
    name.chop(1);
    name+= ";";
    sql+=name;
    QSqlQuery query(data_base);
    bool result = query.exec(sql);
    errMessage = query.lastError().text();
    return result;

}
///
/// \brief CRUDBaseOperation::GetKeyType
/// \param tablename
/// \param name
/// \return
///???????????????????????????
QString CRUDBaseOperation::GetKeyType(QString tablename, QString name)
{
    //?????????????????????????????????
    QSqlQuery query(data_base);
    QString str ;
    str = QString("select COLUMN_NAME,DATA_TYPE,COLUMN_COMMENT from information_schema.columns where TABLE_NAME='%1'").arg(tablename) ;
    query.prepare(str);
    QString nametype ="";
    if(query.exec())
    {
        while (query.next())
        {
           // qDebug()<<"query"<<query.value(0).toString() << query.value(1).toString() <<query.value(2).toString();
            if(query.value(0).toString() == name)
            {
                nametype = query.value(1).toString();
                return nametype;
            }
        }
    }
    else
    {
        //qDebug() << query.lastError();
        //return query.lastError().text();
    }
    return nametype;
}
///
/// \brief CRUDBaseOperation::GettablesFromDb
///
void CRUDBaseOperation::GettablesFromDb(QStringList &tablelist)
{
    QStringList tables = data_base.tables();  //????????????????????????
    QStringListIterator itr(tables);
    while (itr.hasNext())
    {
        QString tableNmae = itr.next().toLocal8Bit();
        tablelist.append(tableNmae);
        // qDebug() << QString::fromLocal8Bit("?????????")+ tableNmae;
    }
}
///
/// \brief CRUDBaseOperation::ExcBatchDeleteDB
/// \param table
/// \param keyname
/// \param keys
/// \return
///????????????????????????????????????
bool CRUDBaseOperation::ExcBatchDeleteDB(const QString &table, QString keyname, QList<QVariant> keys,QString &error)
{
    QSqlQuery query(data_base);
    data_base.transaction();//????????????
    QString keyType = GetKeyType(table,keyname);
    for(int i = 0; i < keys.size();++i)
    {
        QString sql;
       if(keyType.contains("varchar"))
       {
           sql = QString("delete from %1 Where %2 = '%3'").arg(table).arg(keyname).arg(keys[i].toString());
       }
       else if(keyType.contains("int")|| keyType.contains("double")){

           sql = QString("delete from %1 Where %2 = %3").arg(table).arg(keyname).arg(keys[i].toString());
       }
       query.exec(sql);
   }
   if(!data_base.commit())
   {
       data_base.rollback();
       error = query.lastError().text();
       return false;
   }
   return true;
}
///
/// \brief CRUDBaseOperation::ExcBatchUpdateDB
/// \param table
/// \param names
/// \param valuesvec
/// \param keyname
/// \param keyvalue
/// \param sqlerror
/// \return
///??????????????????????????????????????????
bool CRUDBaseOperation::ExcBatchUpdateDB(const QString &table, QStringList &names, QVector<QVariantList> &valuesvec, QString keyname, QVector<QVariant> keyvalue, QString &sqlerror)
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QSqlQuery query(data_base);
   data_base.transaction();//????????????
   QString updatesql = QString("update %1 set ").arg(table);//?????????????????????????????????
   for(int i=0;i<valuesvec.size();++i)
   {
       if(names.size()!= valuesvec[i].size())
       {
           sqlerror = "???????????????????????????????????????????????????";
           return false;
       }
   }
   if(valuesvec.size()!= keyvalue.size())
   {
       return false;
   }
   for(int j=0;j<names.size();++j)
   {
       updatesql+=QString("%1=?,").arg(names[j]);
   }
   updatesql.chop(1);
   updatesql += QString(" where %1").arg(keyname);//????????????????????????
   QString keytype = GetKeyType(table,keyname);
   for(int i=0;i<valuesvec.size();++i )
   {
       QString keyid;
       if(keytype.contains("char"))
       {
           keyid+=QString("='%1'").arg(keyvalue[i].toString());
       }
       else if((keytype.contains("int"))|| (keytype.contains("double")))
       {
           keyid+=QString("=%1").arg(keyvalue[i].toString());
       }
       QString str =QString("%1 %2").arg(updatesql).arg(keyid);
//       qDebug()<<"sql:"<<str;
       query.prepare(str);
       for(int j=0; j< valuesvec[i].size();++j)
       {
           query.bindValue(j,valuesvec[i][j]);
       }
       if(!query.exec())
       {
           sqlerror = query.lastError().text();
           return false;
       }
   }
   if(!data_base.commit())
   {
       data_base.rollback();
       sqlerror=query.lastError().text();
       return false;
   }
   return true;
}
///
/// \brief CRUDBaseOperation::ExcBatchSelectDb
/// \param table
/// \return
///?????????????????????????????????
QSqlQuery CRUDBaseOperation::ExcBatchSelectDB(const QString table)
{
    QSqlQuery query(data_base);
    data_base.transaction();//????????????
    QString selectsql = QString("select * from %1").arg(table);
    query.prepare(selectsql);//
    query.exec();
    data_base.commit();
    return query;
}
///
/// \brief CRUDBaseOperation::ExcBatchReplaceDB
/// \param table
/// \param names
/// \param values
/// \return
/// replace into ????????????????????????????????????????????????//???????????????????????????????????????????????????????????????
bool CRUDBaseOperation::ExcBatchReplaceDB(const QString &table, QStringList &names, QList<QVariantList> &values,QString &sqlerror)
{
    QString msg;

    for(int i = 0 ; i < values.size();++i )
    {
        if(names.size() != values[i].size() )
        {
                msg = "???????????????????????????????????????????????????";
                sqlerror = msg;
            return false;
        }
    }
    //QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    QSqlQuery query(data_base);
    data_base.transaction();//????????????
    QString insertsql = QString("replace into %1(").arg(table);//???????????????????????????
    QString valuesnumstr ;
    for( int j = 0;j < names.size(); ++j)
    {
        insertsql += names[j] +",";
        valuesnumstr += QString("?")+",";
    }
    insertsql.chop(1);
    insertsql += QString(") values(") + valuesnumstr;
    insertsql.chop(1);
    insertsql +=")";
    if(!query.prepare(insertsql)){
           sqlerror = query.lastError().text();
    }
    for(int i = 0 ; i < names.size();++i)
    {
        QVariantList list;
        for(int j = 0; j < values.size();++j)
        {
            list.append(values[j][i]);
        }
        query.addBindValue(list);
    }
//    if(false == data_base.isOpen())
//    {
//        if(false == data_base.open())
//        {
//            sqlerror = "???????????????????????????";
//            return false;
//        }
//    }
    if(!query.execBatch()){
        sqlerror = query.lastError().text();
        qDebug()<<"in execBatch ??????: " << sqlerror<<endl;
         return false;

    }//???????????????????????????????????????
    if(!data_base.commit())
    {
        sqlerror = query.lastError().text();
        data_base.rollback();
        qDebug()<<"info" << sqlerror<<endl;
        return false;
    }
    return true;
}

bool CRUDBaseOperation::ExcBatchInsertDb(const QString &table, QStringList &names, QList<QVariantList> &values,QString &errorinfo)
{
    QMutexLocker locker(&Myconfig::GetInstance()->m_mutex_sqlwrite);
    for(int i = 0 ; i < values.size();++i )
    {
        if(names.size() != values[i].size() )
        {
            errorinfo = "??????????????????????????????????????????";
            return false;
        }
    }
    QSqlQuery query(data_base);
    data_base.transaction();//????????????
    QString insertsql = QString("insert into %1(").arg(table);//???????????????????????????
    for( int j = 0;j< names.size(); ++j)
    {
        insertsql += names[j] +",";
    }
    insertsql.chop(1);
    insertsql += QString(") values(");
    for(int i = 0; i < names.size();++i)
    {
        insertsql +=QString("?")+",";
    }
    insertsql.chop(1);
    insertsql +=")";
    //qDebug()<<"sql:"<<insertsql;
    query.prepare(insertsql);
    QVector<QVariantList> listvec;//
    for(int i = 0 ; i< names.size();++i)
    {
        QVariantList list;
        for(int j=0; j < values.size();++j)
        {
            list.append(values[j][i]);
        }
        listvec.append(list);
        //list
    }
    for(int i=0;i<listvec.size();++i)
    {
        query.addBindValue(listvec[i]);
    }
    if(!query.execBatch()){
       qDebug()<<"eeror:"<<query.lastError().text();
         errorinfo =  query.lastError().text();
         return false;
    }//???????????????????????????????????????
    if(!data_base.commit())
    {
        data_base.rollback();
        errorinfo =  query.lastError().text();
        return false;
    }
    return true;
}

CRUDBaseOperation *CRUDBaseOperation::getInstance()
{
    if (Instance != nullptr)
        return Instance;
    return nullptr;
}
