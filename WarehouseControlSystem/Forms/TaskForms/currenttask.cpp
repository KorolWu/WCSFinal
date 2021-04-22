#include "currenttask.h"

CurrentTask::CurrentTask(int width,int height,QWidget *parent):BaseFrom(parent)
{
    this->width = width;
    this->height = height;
    this->setFixedWidth(parent->width());
    initTableView();
    initMenu();
    m_redisob = new RWRedisInterfaceLib();
    if(!m_redisob->Connectredis(Myconfig::GetInstance()->m_redisInfo.ip.toStdString().c_str(),Myconfig::GetInstance()->m_redisInfo.port))
    {
        qDebug()<<"redis 连接失败";
    }
    else{
        qDebug()<<"redis 连接成功";
        QMap<QString,TaskInfoStru> curtaskData;
        curtaskData = m_redisob->ReadCurTaskDataFromRedis();
        for(auto it = curtaskData.begin(); it != curtaskData.end();++it )
        {
            TaskSWtoCutTask(it.value());
        }
        SetTableData();
    }
    //   //接口方式测试
    //    QString str ="周五 1月 15 17:48:07 2021";
    //    QString teststr = QString("HMSET time00 createTime %1 requestTimes 1 ").arg(str,);
    //    char * testchar = "HMSET time00 createTime ";
    //   // memcpy()
    //    redisReply* reply00 =(redisReply*)redisCommand(m_redisob->m_predistext,"HMSET time00 createTime %s requestTimes 1 ",str);
    //    if(reply00->type == REDIS_REPLY_ERROR)
    //    {
    //        qDebug()<<"写入值失败，指令错误";
    //    }
    //    if(QString(QLatin1String(reply00->str)) == "OK")
    //    {
    //        qDebug()<<"写入成功";
    //    }
    //    freeReplyObject(reply00);
}

void CurrentTask::setTableViewValue()
{
    int row_count = Myconfig::GetInstance()->m_taskMap.size();
    for(int j = 0; j< 8 ;j++)
    {
        auto it = Myconfig::GetInstance()->m_taskMap.begin();
        for(int i = 0;i<row_count;i++)
        {
            if(j == 111)
                model->setItem(i,j,new QStandardItem(QString("%1").arg(i)));
            else if(j == 0)
                model->setItem(i,j,new QStandardItem(it.value().taskNum));
            else if(j == 1)
                model->setItem(i,j,new QStandardItem(it.value().status));
            else if(j == 2)
                model->setItem(i,j,new QStandardItem(it.value().boxNum));
            else if(j == 3)
                model->setItem(i,j,new QStandardItem(QString("%1").arg(it.value().pripty)));
            else if(j == 4)
                model->setItem(i,j,new QStandardItem(it.value().from));
            else if(j == 5)
                model->setItem(i,j,new QStandardItem(it.value().end));
            else if(j == 6)
                model->setItem(i,j,new QStandardItem(it.value().carNum));
            else if(j == 7)
                model->setItem(i,j,new QStandardItem(it.value().creatTime.toString("yyyy-MM-dd hh:mm:ss")));
            it ++;
        }
    }
}

void CurrentTask::refreshTable()
{
    //setTableViewValue();
    SetTableData();
    setColumnWidth();
}

void CurrentTask::initTableView()
{
    m_table_view = new QTableView(this);
    m_table_view->horizontalHeader()->setStyleSheet(headstlye);
    m_table_view->move(5,10);
    m_table_view->verticalHeader()->hide();
    m_table_view->resize(width,height);
    connect(m_table_view,&QTableView::customContextMenuRequested,this,&CurrentTask::onTableViewClick);
    model = new QStandardItemModel(0,7);
    m_table_view->setModel(model);

    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
   //m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList header;
    header<<"任务id"<<"任务主类型"<<"任务子类型"<<"料箱号"<<"起始库位"<<"目的库位"<<"创建时间";
    for(int i = 0 ;i <header.size();i++)
    {
        model->setHeaderData(i, Qt::Horizontal, header.at(i));
        m_table_view->setColumnWidth(i,width/header.size());
    }
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
}

void CurrentTask::initMenu()
{
    popMenu = new QMenu(m_table_view);
    QAction *actionDelInfo = new QAction();
    actionDelInfo ->setText(QString("删除"));
    popMenu->addAction(actionDelInfo);
    connect(actionDelInfo,&QAction::triggered,this,&CurrentTask::onDeleteActionClicked);
}

void CurrentTask::setColumnWidth()
{
    m_table_view->setColumnWidth(0,width/20*5);
    m_table_view->setColumnWidth(1,width/20*1);
    m_table_view->setColumnWidth(2,width/20*1);
    m_table_view->setColumnWidth(3,width/20*2);

    m_table_view->setColumnWidth(4,width/20*3);
    m_table_view->setColumnWidth(5,width/20*3);
    m_table_view->setColumnWidth(6,width/20*4);
    m_table_view->horizontalHeader()->setMinimumHeight(40);
    m_table_view->setFont(QFont("宋体",12)); //设置字体
}
//save to crruntTask table
bool CurrentTask::saveTaskToDB(TaskInfoStru taskStru)
{
    QString err = "";
    if(!CRUDBaseOperation::getInstance()->saveCrruntTask(taskStru,err))
    {
        GetSystemLogObj()->writeLog(err,3);
        return false;
    }
    return true;
}

TaskInfoStru CurrentTask::ParsingWMSFrame(QString frame)
{
    TaskInfoStru t;
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(frame.toUtf8(), &jsonParseError));
    if(!jsonDocument.isNull()&&QJsonParseError::NoError == jsonParseError.error)//没有错误
    {
        if(jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if(jsonObject.contains("regCode")&&jsonObject.value("regCode").isString())
            {
                //qDebug()<<"regCode"<<jsonObject.value("regCode").toString();
                t.regCode = jsonObject.value("regCode").toString();
            }
            if(jsonObject.contains("interfaceType")&&jsonObject.value("interfaceType").isString())
            {
                //qDebug()<<"interfaceType"<<jsonObject.value("interfaceType").toString();
                t.interfaceType = jsonObject.value("interfaceType").toString();
            }
            if(jsonObject.contains("from")&&jsonObject.value("from").isString())
            {
                //qDebug()<<"from"<<jsonObject.value("from").toString();
                t.from = jsonObject.value("from").toString();
            }
            if(jsonObject.contains("to")&&jsonObject.value("to").isString())
            {
                //qDebug()<<"to"<<jsonObject.value("to").toString();
                t.end = jsonObject.value("to").toString();
            }
            if(jsonObject.contains("taskId")&&jsonObject.value("taskId").isString())
            {
                //qDebug()<<"taskId"<<jsonObject.value("taskId").toString();
                t.taskNum = jsonObject.value("taskId").toString();
            }
            if(jsonObject.contains("taskMainType")&&jsonObject.value("taskMainType").isString())
            {
                //qDebug()<<"taskMainType"<<jsonObject.value("taskMainType").toString();
                t.taskMainType = jsonObject.value("taskMainType").toString();
            }
            if(jsonObject.contains("taskSonType")&&jsonObject.value("taskSonType").isString())
            {
                //qDebug()<<"taskSonType"<<jsonObject.value("taskSonType").toString();
                t.taskSonType = jsonObject.value("taskSonType").toString();
            }
            if(jsonObject.contains("status")&&jsonObject.value("status").isString())
            {
                //qDebug()<<"status"<<jsonObject.value("status").toString();
                t.status = jsonObject.value("status").toString();
            }
            if(jsonObject.contains("receiptNo")&&jsonObject.value("receiptNo").isString())
            {
                //qDebug()<<"receiptNoType"<<jsonObject.value("receiptNoType").toString();
                t.receiptNo = jsonObject.value("receiptNo").toString();
            }
            if(jsonObject.contains("receiptNoType")&&jsonObject.value("receiptNoType").isString())
            {
                //qDebug()<<"receiptNoType"<<jsonObject.value("receiptNoType").toString();
                t.receiptNoType = jsonObject.value("receiptNoType").toString();
            }
            if(jsonObject.contains("containerCode")&&jsonObject.value("containerCode").isString())
            {
                //qDebug()<<"containerCode"<<jsonObject.value("containerCode").toString();
                t.boxNum = jsonObject.value("containerCode").toString();
            }
            if(jsonObject.contains("fromLocationCode")&&jsonObject.value("fromLocationCode").isString())
            {
                //qDebug()<<"fromLocationCode"<<jsonObject.value("fromLocationCode").toString();
                t.fromLocationCode = jsonObject.value("fromLocationCode").toString();
            }
            if(jsonObject.contains("toLocationCode")&&jsonObject.value("toLocationCode").isString())
            {
                // qDebug()<<"toLocationCode"<<jsonObject.value("toLocationCode").toString();
                t.shelves = jsonObject.value("toLocationCode").toString();
            }
            if(jsonObject.contains("taskResult")&&jsonObject.value("taskResult").isString())
            {
                //qDebug()<<"taskResult"<<jsonObject.value("taskResult").toString();
                t.taskResult = jsonObject.value("taskResult").toString();
            }
            if(jsonObject.contains("nowLocationCode")&&jsonObject.value("nowLocationCode").isString())
            {
                //qDebug()<<"nowLocationCode"<<jsonObject.value("nowLocationCode").toString();
                t.nowlocationCode = jsonObject.value("nowLocationCode").toString();
            }
            if(jsonObject.contains("abnormalLocationCode")&&jsonObject.value("abnormalLocationCode").isString())
            {
                //qDebug()<<"abnormalLocationCode"<<jsonObject.value("nowLocationCode").toString();
                t.abnormalLocationCode = jsonObject.value("abnormalLocationCode").toString();
            }
            if(jsonObject.contains("inbToLocationCode")&&jsonObject.value("inbToLocationCode").isString())
            {
                //qDebug()<<"inbToLocationCode"<<jsonObject.value("inbToLocationCode").toString();
                t.inbToLocationCode = jsonObject.value("inbToLocationCode").toString();
            }
            if(jsonObject.contains("responseMessage")&&jsonObject.value("responseMessage").isString())
            {
                //qDebug()<<"responseMessage"<<jsonObject.value("responseMessage").toString();
                t.responseMessage = jsonObject.value("responseMessage").toString();
            }
            if(jsonObject.contains("createTime")&&jsonObject.value("createTime").isString())
            {
                //qDebug()<<"createTime"<<jsonObject.value("createTime").toString();
                //  t.creatTime =
            }
            if(jsonObject.contains("requestTimes")&&jsonObject.value("requestTimes").isString())
            {
                //qDebug()<<"requestTimes"<<jsonObject.value("requestTimes").toString();
                t.requestTimes = jsonObject.value("requestTimes").toString();
            }
        }
        //        if(!m_pRedisCli->hset("task",t.taskNum,QString("%1").arg(frame.replace("\"", "").remove(QRegExp("\\s")))))
        //            qDebug()<<"reids save Error";

    }
    else{
        qDebug() << QString("FROM WMS TO WCS  JsonParseError: %1").arg(jsonParseError.errorString());
    }
    t.creatTime = QDateTime::currentDateTime();
    return t;
}

void CurrentTask::reply2Wms(const TaskInfoStru &t)
{
    Jeson2Object j;
    QByteArray array("");
    if(t.taskSonType =="上架")
    {
        array = j.makeBody(t,150,"流道",true,"");
    }
    else if(t.taskSonType == "回库")
    {
        array = j.makeBody(t,630,"流道",true,"");
    }
    // qDebug()<<QString(array);
    HttpClien_k *k = new HttpClien_k();
    OrderStru o;
    o.http.body = array;
    o.http.url = Myconfig::GetInstance()->inOfWareHouseUrl;
    QString errmsg = "";
    k->setParameter(o,0);
    k->runInstruction();
    k->getResult(errmsg);
}
///
/// \brief CurrentTask::putBox 当wms发送入库任务时，将缓存区的料想推到流道
/// \param cache 缓存位的编号
///
void CurrentTask::putBox(int cache)
{
    OrderStru o;
    o.value = 99;
    o.order = call_Runner_Putbox;
    o.startaddress = (cache-1)*2 + 60;
    o.Datatype = 168; //寄存器 读写
    o.childtype = 2; //modebus read or write ;read = 5 other is write
    AbstructInstruction *e = new RunnerInstruction();
    e->setParameter(o,9);
    e->runInstruction();
    QString resultMsg ="";
    e->getResult(resultMsg);
    qDebug()<<resultMsg;
}
///
/// \brief CurrentTask::SetTableData
/// 根据内存的内容进行刷新
void CurrentTask::SetTableData()
{
//m_table_view
    model->removeRows(0, model->rowCount());
    //出库任务数据map
    int currowsize = 0;
    for(auto it = Myconfig::GetInstance()->m_OuttaskMap.begin(); it != Myconfig::GetInstance()->m_OuttaskMap.end();++it)
    {
        for(int i = 0; i < it.value().size(); ++i)
        {
            model->setItem(i+currowsize,0,new QStandardItem(it.value().at(i).regCode));
            model->setItem(i+currowsize,1,new QStandardItem(it.value().at(i).taskMainType));
            model->setItem(i+currowsize,2,new QStandardItem(it.value().at(i).taskSonType));
            model->setItem(i+currowsize,3,new QStandardItem(it.value().at(i).boxNum));
            model->setItem(i+currowsize,4,new QStandardItem(it.value().at(i).fromLocationCode));
            model->setItem(i+currowsize,5,new QStandardItem(it.value().at(i).shelves));
            model->setItem(i+currowsize,6,new QStandardItem(it.value().at(i).creatTime.toString("yyyy-MM-dd hh:mm:ss")));          
        }

        currowsize = currowsize + it.value().size();
    }
    //入库任务数据map
    for(auto it = Myconfig::GetInstance()->m_IntaskMap.begin(); it != Myconfig::GetInstance()->m_IntaskMap.end();++it)
    {
        int i = 0;
        for(auto item = it.value().begin(); item != it.value().end(); ++item)
        {
            model->setItem(i+currowsize,0,new QStandardItem(item.value().regCode));
            model->setItem(i+currowsize,1,new QStandardItem(item.value().taskMainType));
            model->setItem(i+currowsize,2,new QStandardItem(item.value().taskSonType));
            model->setItem(i+currowsize,3,new QStandardItem(item.value().boxNum));
            model->setItem(i+currowsize,4,new QStandardItem(item.value().fromLocationCode));
            model->setItem(i+currowsize,5,new QStandardItem(item.value().shelves));
            model->setItem(i+currowsize,6,new QStandardItem(item.value().creatTime.toString("yyyy-MM-dd hh:mm:ss")));         
            ++i;
        }
        currowsize = currowsize + it.value().size();
    }
}

void CurrentTask::pushBox(QString boxNum)
{
    int cache = 0;
    for(auto it = Myconfig::GetInstance()->m_RunnerMap[9].m_RunnerBoxCodeInfoMap.begin(); it != Myconfig::GetInstance()->m_RunnerMap[9].m_RunnerBoxCodeInfoMap.end(); it++)
    {
        if(it.value() == boxNum)
        {
            cache = it.key();
            break;
        }
    }
    if(cache != 0)
    {
        Myconfig::GetInstance()->m_cacheRunerMap[cache] = false;
        qDebug()<<"cache["<<cache<<"] = false";
        putBox(cache);
    }
    else {

         putBox(8);
         Myconfig::GetInstance()->m_cacheRunerMap[8] = false;
         qDebug()<<"没有找到料箱号对应的缓存位";
    }
}

bool CurrentTask::TaskSWtoCutTask(TaskInfoStru t)
{
    bool typecheck = false;
    if(t.taskMainType == "出库")
    {
        QMutexLocker locker(&Myconfig::GetInstance()->m_task_mutex);
        //qDebug()<<"添加出库任务";
        //新增数据任务插入方式
        int nbr = t.shelves.toInt() ;
        m_beforeInlist.clear();
       for(auto it = Myconfig::GetInstance()->m_OuttaskMap.begin(); it != Myconfig::GetInstance()->m_OuttaskMap.end();++it)
       {
           for(int i = 0; i < it.value().size(); ++i)
           {
               m_beforeInlist.append(it.value().at(i).boxNum);
           }
       }
        if(m_beforeInlist.contains(t.boxNum))
        {
            return  false;
        }
        else{
            m_beforeInlist.append(t.boxNum);
        }
        if(Myconfig::GetInstance()->m_OuttaskMap.contains(nbr))
        {
            Myconfig::GetInstance()->m_OuttaskMap[nbr].enqueue(t);
        }
        else{
            QQueue<TaskInfoStru> queue;
            queue.append(t);
            Myconfig::GetInstance()->m_OuttaskMap.insert(nbr,queue);
        }
        typecheck = true;
    }
    else if(t.taskMainType == "入库")
    {
        QMutexLocker(&Myconfig::GetInstance()->m_in_task_mutex);
        Myconfig::GetInstance()->m_in_taskMap.insert(t.boxNum,t);
        //新增数据任务插入方式
        int frromnbr = t.fromLocationCode.toInt();
        if(Myconfig::GetInstance()->m_IntaskMap.contains(frromnbr))
        {
            Myconfig::GetInstance()->m_IntaskMap[frromnbr].insert(t.boxNum,t);
        }
        else
        {
            QMap<QString,TaskInfoStru> intask;
            intask.insert(t.boxNum,t);
            Myconfig::GetInstance()->m_IntaskMap.insert(frromnbr,intask);
        }
        //qDebug()<<"添加入库任务";
        typecheck = true;
    }
    else if(t.taskMainType == "取消")
    {
        //任务取消，进行任务取消待定
        // typecheck =
    }
    return typecheck;
}

bool CurrentTask::verificationData(TaskInfoStru &t)
{
    //去除空格
    t.abnormalLocationCode.remove(QRegExp("\\s"));
    t.boxNum.remove(QRegExp("\\s"));
    // t.carNum.remove(QRegExp("\\s"));
    t.creatTime.toString().remove(QRegExp("\\s"));
    t.end.remove(QRegExp("\\s"));
    t.from.remove(QRegExp("\\s"));
    t.fromLocationCode.remove(QRegExp("\\s"));
    t.inbToLocationCode.remove(QRegExp("\\s"));
    t.interfaceType.remove(QRegExp("\\s"));
    t.nowlocationCode.remove(QRegExp("\\s"));
    t.receiptNo.remove(QRegExp("\\s"));
    t.receiptNoType.remove(QRegExp("\\s"));
    t.regCode.remove(QRegExp("\\s"));
    t.requestTimes.remove(QRegExp("\\s"));
    t.responseMessage.remove(QRegExp("\\s"));
    t.shelves.remove(QRegExp("\\s"));
    t.status.remove(QRegExp("\\s"));
    t.taskMainType.remove(QRegExp("\\s"));
    t.taskNum.remove(QRegExp("\\s"));
    t.taskResult.remove(QRegExp("\\s"));
    t.taskSonType.remove(QRegExp("\\s"));
    //校验是否为空
    if(t.abnormalLocationCode.isEmpty())
        return false;
    if(t.boxNum.isEmpty())
        return false;
    if(t.creatTime.toString().isEmpty())
        return false;
    if(t.end.isEmpty())
        return false;
    if(t.from.isEmpty())
        return false;
    if(t.fromLocationCode.isEmpty())
        return false;
    if(t.inbToLocationCode.isEmpty())
        return false;
    if(t.interfaceType.isEmpty())
        return false;
    if(t.nowlocationCode.isEmpty())
        return false;
    if(t.receiptNo.isEmpty())
        return false;
    if(t.receiptNoType.isEmpty())
        return false;
    if(t.regCode.isEmpty())
        return false;
    if(t.requestTimes.isEmpty())
        return false;
    if(t.responseMessage.isEmpty())
        return false;
    if(t.shelves.isEmpty())
        return false;
    if(t.status.isEmpty())
        return false;
    if(t.taskMainType.isEmpty())
        return false;
    if(t.taskNum.isEmpty())
        return false;
    if(t.taskResult.isEmpty())
        return false;
    if(t.taskSonType.isEmpty())
        return false;
    return true;

}

void CurrentTask::onTableViewClick(const QPoint &pos)
{
    auto index = m_table_view->indexAt(pos);
    if (index.isValid())
    {
        int row=m_table_view->currentIndex().row();

        QAbstractItemModel *model = m_table_view->model ();
        QModelIndex index = model->index(row,1);//选中行第一列的内容
        QVariant data = model->data(index);
        if(data.toString() == "出库")
        {
            index = model->index(row,5);
            m_stationNum = model->data(index).toInt();
        }
         else if(data.toString() == "入库")
        {
            index = model->index(row,4);
            m_stationNum = model->data(index).toInt();
        }
         index = model->index(row,0);
         m_regCode = model->data(index).toString();
        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    }
}

void CurrentTask::handelHttpTask(QString reply)
{
    //存入redis log数据
    m_redisob->GetRedisBaseModuleOb()->insert_log(reply);
    TaskInfoStru t = ParsingWMSFrame(reply);
    //    if(!verificationData(t))
    //        return;
    //t.regCode,reply; 存入多段哈系表
    //saveTaskToDB(t);
    if(!TaskSWtoCutTask(t))
    {
        qDebug()<<"不是出库入库类型，请检查,任务重复";
        return ;
    }
    if(t.taskMainType == "入库")
    {
        //这里立即出发流道处理该料箱
        pushBox(t.boxNum); //测试时候注释掉
        //reply wms
        //reply2Wms(t);//测试的时候注释掉
        //qDebug()<<"添加入库任务";
    }
    QMutexLocker(&Myconfig::GetInstance()->m_mutex_taskMap);
    Myconfig::GetInstance()->m_taskMap.insert(t.taskNum,t); //存储的所有任务结构体
    QString errMsg = "";
    //    if(!CRUDBaseOperation::getInstance()->saveKBaseStruct("t_crrunt_task",t,errMsg))
    //        GetSystemLogObj()->writeLog("save current to dbbase failed! ->"+errMsg,2);
    // setTableViewValue();
    // qDebug()<<reply;
    //进行刷新界面
    SetTableData();
    //增加校验的数据部分
    if(!m_redisob->WriteTaskDatatoRedis(t))
    {
        qDebug()<<"值写入失败参数";
    }
}

void CurrentTask::onDeleteActionClicked()
{
    OperationTask o;
    o.removeTask(m_stationNum,m_regCode);
    this->refreshTable();
}

void CurrentTask::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
    //auto index = m_table_view->indexAt(event->pos());
    //if (index.isValid())
    {
        int row=m_table_view->currentIndex().row();

        QAbstractItemModel *model = m_table_view->model ();
        QModelIndex index = model->index(row,1);//选中行第一列的内容
        QVariant data = model->data(index);
        if(data.toString() == "出库")
        {
            index = model->index(row,5);
            m_stationNum = model->data(index).toInt();
        }
         else if(data.toString() == "入库")
        {
            index = model->index(row,4);
            m_stationNum = model->data(index).toInt();
        }
         index = model->index(row,0);
         m_regCode = model->data(index).toString();
        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    }
}
