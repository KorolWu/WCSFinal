#include "storeinfowidget.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>
#include <QMessageBox>
#include <QEventLoop>
#include <QTimer>
#include <MysqlDataBase/crudbaseoperation.h>
#include <QFileDialog>
#include "UnitClass/ImportExportFile/localfileoperate.h"
#include "editstorenbrinfodialog.h"
#include  "pagewg/tablepagebyabstractwg.h"

const QString stylestr =  "QPushButton{font:12px;background-color:rgb(150,150,150)}"
                          "QLabel{font:12px;}"
                          "QPushButton:hover{background-color:rgba(220,220,220,110);}"
                          "QPushButton:pressed{background-color:rgba(85,170,255,150);}"
                          "QTableView{font:12px;selection-background-color:rgba(68,69,71,100);alternate-background-color: rgb(220, 220, 220);}"
                          "QMenu::item:selected {background-color:rgba(0,190,230,100);}";
#include <QTextCodec>
StoreInfoWidget::StoreInfoWidget(QWidget *parent):QWidget(parent)
{
    //Init data
    m_stroreposmap.storeposInfoMap.clear();
    m_editrow = 0;
    //读取数据库的内容映射到内存中
    //Init UI
    QVBoxLayout *pmainlayout = new QVBoxLayout;
    setLayout(pmainlayout);
    QLabel *pshowlabeltitle = new QLabel(tr("当前位置：WCS系统<<货架管理..."));
    pmainlayout->addWidget(pshowlabeltitle);
    pmainlayout->setSpacing(10);
    QHBoxLayout *pbtnLayout = new QHBoxLayout();
    m_pAddBtn = new QPushButton(tr("添加"),this);
    m_pAddBtn->setIcon(QIcon(":/resouse/Image/add.png"));
    m_pAddBtn->setLayoutDirection(Qt::LayoutDirectionAuto);
    pbtnLayout->addWidget(m_pAddBtn);
    connect(m_pAddBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotaddnbrinfo);

    m_pBatchDelBtn = new QPushButton(tr("批量删除"),this);
    m_pBatchDelBtn->setIcon(QIcon(":/resouse/Image/delete.png"));
    pbtnLayout->addWidget(m_pBatchDelBtn);
    connect(m_pBatchDelBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotBatchDelnbrinfo);

    m_pnbrlineEdit = new QLineEdit(this);
    QAction *pnbrAction = new QAction( m_pnbrlineEdit);
    pnbrAction->setIcon(QIcon(":/resouse/Image/Search.png"));
    m_pnbrlineEdit->addAction(pnbrAction,QLineEdit::LeadingPosition);
    m_pnbrlineEdit->setMaxLength(64);
    pbtnLayout->addWidget(m_pnbrlineEdit);
    connect(m_pnbrlineEdit,&QLineEdit::returnPressed,this,&StoreInfoWidget::slotquenbrinfo);

    m_pquenbrBtn = new QPushButton(tr("查询仓位编号"),this);
    m_pquenbrBtn->setIcon(QIcon(":/resouse/Image/Search.png"));
    m_pquenbrBtn->setMinimumWidth(100);
    pbtnLayout->addWidget(m_pquenbrBtn);
    connect(m_pquenbrBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotquenbrinfo);
    //m_pquenbrBtn->setShortcut(Qt::Key_Enter);//将字母区回车键查询按钮绑定在一起
    //m_pquenbrBtn->setShortcut(Qt::Key_Return);//将小键盘回车键与查询按钮按钮绑定在一起

    m_pImportBtn = new QPushButton(tr("导入"),this);
    m_pImportBtn->setIcon(QIcon(":/resouse/Image/import.png"));
    pbtnLayout->addWidget(m_pImportBtn);
    connect(m_pImportBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotImportnbrinfo);

    m_pExportBtn = new QPushButton(tr("导出"),this);
    m_pExportBtn->setIcon(QIcon(":/resouse/Image/exportfile.png"));
    pbtnLayout->addWidget(m_pExportBtn);
    connect(m_pExportBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotExportnbrinfo);
    pbtnLayout->addStretch();
    m_pRefreshBtn = new QPushButton(tr("刷新"),this);
    m_pRefreshBtn->setIcon(QIcon(":/resouse/Image/Refresh.png"));
    pbtnLayout->addWidget(m_pRefreshBtn);
    connect(m_pRefreshBtn,&QPushButton::clicked,this,&StoreInfoWidget::slotRefreshDataBase);
    //pbtnLayout->addStretch();

    //添加table表头
    m_ptableview = new StorenbrTableView();
    m_ptableview->resize(parent->width(),parent->height());

    QVBoxLayout *pBodyLayout = new QVBoxLayout();
    pBodyLayout->addLayout(pbtnLayout);

    QGroupBox *pbtnMainGroup = new QGroupBox();
    pbtnMainGroup->setLayout(pBodyLayout);
    pmainlayout->addWidget(pbtnMainGroup);

    pmainlayout->addWidget(m_ptableview);
    //   pmainlayout->setStretchFactor(pshowlabeltitle,1);
    //   pmainlayout->setStretchFactor(pbtnLayout,1);

    connect(this,&StoreInfoWidget::signalfindinfo,m_ptableview,&StorenbrTableView::SlotFindNbrinfo);
    connect(m_ptableview,&StorenbrTableView::signalDelRowData,this,&StoreInfoWidget::SlotDelSinglerow);//表格中的单行操作
    connect(this,&StoreInfoWidget::signalUpdatetable,m_ptableview,&StorenbrTableView::SlotEditInfo);
    connect(m_ptableview,&StorenbrTableView::signalEditRowData,this,&StoreInfoWidget::slottableeditbtn);
    connect(m_ptableview,&StorenbrTableView::signalTableDataSizechange,this,&StoreInfoWidget::SlotupdatePageUI);
    Dataselectfromdatabase();
    //数据假测试进行测试
    QStringList strTypeList;
    strTypeList<<tr("选择")<</*tr("序号")<<*/tr("仓位编号")
              <<tr("类型")<<tr("X坐标")<<tr("Y坐标")<<tr("Z坐标") << tr("方向状态")<<tr("料箱编号")<<tr("仓位状态")<<tr("优先级")<<tr("备注")<<tr("操作");
    m_ptableview->SetTableHeaderData(strTypeList,strTypeList.size());


    //page ob 功能实现方式
    m_ptableview->GetStoretablemodel()->m_setpagerowsize =20;
    int setrecordsize =  m_ptableview->GetStoretablemodel()->m_setpagerowsize;
    m_pagewg = new TablepagebyabstractWg(this);
    m_pagewg->SetParam( m_stroreposmap.storeposInfoMap.size(),"t_storeposinfo",setrecordsize);
    m_pagewg->InitpagefunWg();
    pmainlayout->addWidget(m_pagewg);
    connect(m_pagewg,&TablepagebyabstractWg::SetShowTableRecord,this,&StoreInfoWidget::slotSetCurPageData);

    this->resize(parent->size());
    this->setStyleSheet(stylestr);
}
///
/// \brief StoreInfoWidget::~StoreInfoWidget
///
StoreInfoWidget::~StoreInfoWidget()
{
    if(m_ptableview)
    {
        delete m_pAddBtn;
        delete m_pBatchDelBtn;
        delete m_pnbrlineEdit;
        delete m_pquenbrBtn;
        delete m_pImportBtn;
        delete m_pExportBtn;
        delete m_ptableview;
        delete m_pagewg;
        delete m_pRefreshBtn;
    }
}
///
/// \brief StoreInfoWidget::slotaddnbrinfo
/// 添加编号信息数据框
void StoreInfoWidget::slotaddnbrinfo()
{
    DelDialogBaseob();
    QStringList strTypeList;
    strTypeList<<tr("仓位编号")
              <<tr("类型")<<tr("X坐标")<<tr("Y坐标")<<tr("Z坐标")<<tr("料箱编号") << tr("方向状态")<<tr("仓位状态")<<tr("优先级")<<tr("备注");
    QString flag;
    flag = "add";
    EditStorenbrinfoDialog *adddialog = new EditStorenbrinfoDialog (strTypeList,flag,this);
    adddialog->show();
    connect(adddialog,&EditStorenbrinfoDialog::signalAckAdd,m_ptableview,&StorenbrTableView::SlotaddNbrInfo);

}
///
/// \brief StoreInfoWidget::slotBatchDelnbrinfo
/// 批量删除数据库内容根据编号信息进行删除，不可退回
void StoreInfoWidget::slotBatchDelnbrinfo()
{
    int ret = QMessageBox::warning(nullptr, tr("提示"),
                                   tr("删除后,不可恢复,请谨慎操作!"),
                                   QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
    if(ret == QMessageBox::No)
        return;
    //先进行表格数据，查看checkbox的状态
    //获取表格选中状态
    QList<QVariant> list =   m_ptableview->GetBatchDellist();
    if(list.size() > 0 )
    {
        if(DelDataBaseInfo(list))
        {
            m_ptableview->BatchDeltableInfo();
        }
    }
}
///
/// \brief StoreInfoWidget::slotquenbrinfo
///查询 编号的信息进行显示
void StoreInfoWidget::slotquenbrinfo()
{
    QString text  = m_pnbrlineEdit->text();
    emit signalfindinfo(text,1);
    //    if(m_pagewg->m_totalRecrodCount <=0)
    //        return;
    //    m_pagewg->m_totalRecrodCount = m_ptableview->m_findtotalnum;//更新记录数目更新界面数据
    //    m_pagewg->m_totalPage = m_ptableview->GetStoretablemodel()->GetPageSize();//总页数计算更新
    //    m_pagewg->updateParam();
    //    m_ptableview->GetStoretablemodel()->SetCurPage(0);
}
///
/// \brief StoreInfoWidget::slotImportnbrinfo
///导入csv 或者EXCEL进行处理表格进行数据库的读写
void StoreInfoWidget::slotImportnbrinfo()
{
    ///1.读取数据进行插入
    /// 2.保存到数据库 内存数据与数据库保持 一致
    /// 3.改变内存的方式
    QMessageBox::information(this, tr("导入数据"),tr("会覆盖原有数据"), tr("确定"));
    QFileDialog* fd = new QFileDialog(this);
    QString fileName = fd->getOpenFileName(this,tr("Open File"),"/home",tr("Excel(*.csv)"));
    if(fileName == "")
        return;
    LocalFileOperate fileob;
    QList<QStringList> listdata;
    listdata =  fileob.ReadFileData(fileName);
    if(listdata.size() <= 1)
    {
        QMessageBox::warning(this, tr("数据警告"),tr("导入数据失败，请检查"), tr("确定"));
        return ;
    }
    QMap<QString,StorePosInfoStru> datamap;
    QStringList boxnbr;
    bool datatype = true;
    for(int i = 1; i < listdata.size(); ++i) //从第二行开始屏蔽头数据
    {
        QStringList listrow =listdata[i] ;
        if(listrow.size() >= 9)
        {
            if(listrow[6]!= "")
            {
                if(boxnbr.contains(listrow[6]))
                {
                    datatype = false;
                    break;
                }
            }
            boxnbr.append(listrow[6]);
            StorePosInfoStru stru;
            strncpy(stru.boxnbr,listrow[6].toStdString().c_str(),64);
            strncpy(stru.idnbr,listrow[0].toStdString().c_str(),64);
            stru.coordx = listrow[2].toDouble();
            stru.coordy =listrow[3].toDouble();
            stru.coordz = listrow[4].toDouble();
            stru.directionstate = listrow[5].toInt();
            stru.storestat = listrow[7].toInt();
            stru.storepri = listrow[8].toInt();
            stru.unused = listrow[9];
            if(!datamap.contains(listrow[0]))
            {
                datamap.insert(listrow[0],stru);
            }
            else{
                datamap[listrow[0]] = stru;
            }
        }
    }
    if(datamap.size() > 0 && datatype)
    {
        //数据库插入内容
        QString msg;
        if(!m_databaseopob.WriteInsertInfoDataBase(datamap,msg))
        {
            QMessageBox::warning(this, tr("数据警告"),msg, tr("确定"));
            return ;
        }
        else{ //更新成功
            Dataselectfromdatabase();
            SlotupdatePageUI(datamap.size());
        }
    }
    else {
        QMessageBox::warning(this, tr("数据警告"),tr("导入数据失败, 数组的箱子编号不可重复或者数据不能为空"), tr("确定"));
        return ;
    }
}
///
/// \brief StoreInfoWidget::slotExportnbrinfo
/// 数据库表格的数据进行导出操作
void StoreInfoWidget::slotExportnbrinfo()
{
    QString exportfileName = QFileDialog::getSaveFileName(this, tr("Excel file"), qApp->applicationDirPath (),
                                                          tr("Files (*.csv)"));
    if (exportfileName.isEmpty())
        return;
    QList<QStringList> datalist;
    //结构体数据相互转化
    for(auto it = m_stroreposmap.storeposInfoMap.begin(); it !=m_stroreposmap.storeposInfoMap.end(); ++it )
    {
        QStringList listinfo ;
        listinfo<<QString::fromUtf8(it.value().idnbr)<<QString::number(it.value().type)<<QString::number(it.value().coordx)\
               <<QString::number(it.value().coordy) <<QString::number(it.value().coordz)  << QString::number(it.value().directionstate)<<QString::fromUtf8(it.value().boxnbr) \
              <<QString::number(it.value().storestat) <<QString::number(it.value().storepri) << it.value().unused;
        datalist.append(listinfo);
    }
    //+
    QStringList tableheadlist;
    tableheadlist<<tr("仓位编号")<<tr("类型")<<tr("x坐标")<<tr("y坐标")<<tr("z坐标")<<tr("方向状态")<<tr("箱子编号")<<tr("仓位状态")<<tr("仓位优先级")<<tr("备用");
    LocalFileOperate fileob;
    if(fileob.WriteFileData(datalist,exportfileName,tableheadlist))
    {
        QMessageBox::information(this, tr("导出数据成功"), tr("信息已保存在%1！").arg(exportfileName), tr("确定"));
    }
}
///
/// \brief StoreInfoWidget::slotEditData
/// \param datalist
/// update 表格
void StoreInfoWidget::slotEditData(QStringList datalist)
{
    emit signalUpdatetable(datalist ,m_editrow);
}
///
/// \brief StoreInfoWidget::slottableeditbtn
/// 表格编辑按钮传输过来slot函数
void StoreInfoWidget::slottableeditbtn(QString nbrinfo ,int row)
{
    DelDialogBaseob();
    m_editrow = row;
    QList<QStringList> list;
    if(m_stroreposmap.storeposInfoMap.contains(nbrinfo))
    {
        QMap<QString,StorePosInfoStru> storeposInfoMap;
        storeposInfoMap.insert(nbrinfo,m_stroreposmap.storeposInfoMap[nbrinfo]);
        list =  GetdatalistFromstru(storeposInfoMap);
    }
    if(list.size() !=1)
        return;
    QStringList strTypeList;
    strTypeList<<tr("仓位编号")
              <<tr("类型")<<tr("X坐标")<<tr("Y坐标")<<tr("Z坐标")<<tr("状态方向")<<tr("料箱编号")<<tr("仓位状态")<<tr("优先级") <<tr("备用");
    QString flag = "update";
    //测试数据
    QStringList TEST;
    TEST.clear();
    TEST << "nbr005"<<"L"<<"234"<<"456"<<"789"<<"0"<<"box005"<<"1"<<"5" <<"unused";
    //测试
    TEST = list[0];
    list[0].removeFirst();
    EditStorenbrinfoDialog *adddialog = new EditStorenbrinfoDialog (strTypeList,flag,this);
    adddialog->move(m_ptableview->pos().x() +m_ptableview->width()/2 - adddialog->width()/2,m_ptableview->pos().y() + m_ptableview->height()/2-adddialog->height()/2);
    adddialog->setContent( list[0]);
    adddialog->show();
    connect(adddialog,&EditStorenbrinfoDialog::signalAckBtn,this,&StoreInfoWidget::slotEditData);
}

void StoreInfoWidget::slotSetCurPageData(int ipage)
{
    m_pagewg->m_totalRecrodCount = m_ptableview->GetStoretablemodel()->GetRowCount();//更新记录数目
    m_pagewg->m_totalPage = m_ptableview->GetStoretablemodel()->GetPageSize();//总页数计算更新
    m_ptableview->GetStoretablemodel()->SetCurPage(ipage);
}

void StoreInfoWidget::slotRefreshDataBase()
{
    Dataselectfromdatabase();//读数据库 刷新表格
    SlotupdatePageUI(m_ptableview->GetStoretablemodel()->GetRowCount());
   //测试南通 小车新仓库的数据坐标创建
//    for(int  i = 1; i <= 16;++i)
//    {
//        int k = 1;
//        k = 252*(i-1)+1;
       //WriteNTstoreposinfo(520,893,2123,690,k,i);//各层数写入方式
//    }

//   WriteNTstackerposdinfonew(5,22);坐标系更新新的方式
}
///
/// \brief StoreInfoWidget::Dataselectfromdatabase
///查询数据库的内容，得到信息 映射到表格中
void StoreInfoWidget::Dataselectfromdatabase()
{
    //读数据库表格
    m_stroreposmap.storeposInfoMap.clear();//添加 数据库的内容映射到结构体中
    QString msg;
    //    // write test
    //    QMap<QString,StorePosInfoStru> datamap;
    //    int k = 0;
    //    for(int y = 0; y < 3;++y)
    //    {
    //        int sizex = 28;
    //        if(y == 2)
    //        {
    //            sizex = 42;
    //        }

    //        for(int x = 0; x < sizex;++x)
    //        {
    //            if(x==26 )
    //            {
    //                continue;
    //            }
    //            k++;
    //            StorePosInfoStru stru;
    //            QString strid = QString("z_8idbbr%1 %2 %3").arg(x).arg(y).arg(k);
    //            memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
    //            stru.coordx = x;
    //            stru.coordy = y;
    //            stru.storepri = k;
    //            stru.coordz = 8;
    //            stru.directionstate = 0;
    //            QString box;
    //             box = QString("boxnbr%1 %2 %3").arg(x).arg(y).arg(k);
    //            memcpy(stru.boxnbr,box.toStdString().c_str(),box.size());
    //                    stru.storestat = 3;
    //            datamap.insert(strid,stru);
    //            StorePosInfoStru stru1;
    //            stru1.directionstate = 1;
    //            stru1.coordx = x;
    //            stru1.coordy = y;
    //            stru1.coordz = 8;
    //            k++;
    //            stru1.storepri = k;
    //              QString strid1;
    //              QString box1;
    //            strid1 = QString("z_8idbbr%1 %2 %3").arg(x).arg(y).arg(k);
    //             memcpy(stru1.idnbr,strid1.toStdString().c_str(),strid1.size());
    //            box1 = QString("boxnbr%1 %2 %3").arg(x).arg(y).arg(k);
    //             memcpy(stru1.boxnbr,box1.toStdString().c_str(),box1.size());
    //                       stru1.storestat = 0;
    //            datamap.insert(strid1,stru1);
    //        }
    //    }

    m_databaseopob.ReadStoreposinfoDataBase();
    QList<QStringList> list;
    m_stroreposmap.storeposInfoMap =  Myconfig::GetInstance()->m_storeinfoMap;
    //    for(auto it = Myconfig::GetInstance()->m_storeinfoMap.begin(); it != Myconfig::GetInstance()->m_storeinfoMap.end();++it)
    //    {
    //        QString boxnbr = QString::fromUtf8(it.value().boxnbr);

    //        QString str = boxnbr;
    //       str =  str.remove(QRegExp("\\s"));
    //       boxnbr.replace(" ","_");
    //        memcpy( it.value().boxnbr,boxnbr.toStdString().c_str(),boxnbr.size());
    //       qDebug()<<"str"<< boxnbr << str;
    //    }

    list =  GetdatalistFromstru(m_stroreposmap.storeposInfoMap);
    QStringList rowlist;
    rowlist<<"0"<<"nbr001"<<"1"<<"345"<<"678.0"<<"0"<<"444"<<"1"<<"2"<<"5"<<"未使用";
    rowlist.append(rowlist);
    rowlist.clear();
    rowlist<<"1"<<"10"<<"1"<<"345"<<"678.0"<<"0"<<"boxnbr"<<"1"<<"1"<<"5"<<"未使用";
    rowlist.append(rowlist);
    rowlist.clear();
    rowlist<<"0"<<"11"<<"1"<<"345"<<"678.0"<<"1"<<"boxnbr"<<"1"<<"0"<<"5"<<"未使用";
    if(list.size() == 0)
    {
        //若数据库打不开则插入测试数据
        list.append(rowlist);
    }
    m_ptableview->setModeldatalist(list);
}

bool StoreInfoWidget::DelDataBaseInfo(QList<QVariant> nbrlist)
{
    QString tablename = "t_storeposinfo";
    QString msg;
    QString id = "idNbr";
    if(!CRUDBaseOperation::getInstance()->ExcBatchDeleteDB(tablename,id,nbrlist,msg))
    {
        QMessageBox::warning(this, tr("数据警告"),msg, tr("确定"));
        return false;
    }
    else{
        for(int i = 0 ; i < nbrlist.size(); ++i)
        {
            QString key = nbrlist[i].toString();
            if(m_stroreposmap.storeposInfoMap.contains(key))
            {
                m_stroreposmap.storeposInfoMap.remove(key);
            }
            //调用数据库进行删除操作 ，删除的操作语句
        }
        return true;
    }
}
///
/// \brief StoreInfoWidget::GetdatalistFromstru
/// \param infoMap
/// \return
///
QList<QStringList> StoreInfoWidget::GetdatalistFromstru(QMap<QString, StorePosInfoStru> infoMap)
{
    QList<QStringList> list;
    for(auto it = infoMap.begin(); it!= infoMap.end(); ++it )
    {
        QStringList columnlist;
        columnlist.append("0");
        columnlist.append(QString::fromUtf8(it.value().idnbr));
        columnlist.append(QString::number(it.value().type));
        columnlist.append(QString::number(it.value().coordx));
        columnlist.append(QString::number(it.value().coordy));
        columnlist.append(QString::number(it.value().coordz));
        columnlist.append(QString::number(it.value().directionstate));
        columnlist.append(QString::fromUtf8(it.value().boxnbr));
        columnlist.append(QString::number(it.value().storestat));
        columnlist.append(QString::number(it.value().storepri));
        columnlist.append(it.value().unused);
        list.append(columnlist);
    }
    return list;
}
///
/// \brief StoreInfoWidget::CheckBoxNbrInfo
/// \param boxnbr
/// \return
///检查是否有重复的boxnbrInfo
bool StoreInfoWidget::CheckBoxNbrInfo(QString boxnbr)
{
    for(auto it = m_stroreposmap.storeposInfoMap.begin(); it !=  m_stroreposmap.storeposInfoMap.end();++it)
    {
        if(QString::fromUtf8(it.value().boxnbr) ==boxnbr )
        {
            return  false;;
        }
    }
    return true;
}
///
/// \brief StoreInfoWidget::WriteNTstoreposinfo
///
void StoreInfoWidget::WriteNTstoreposinfo(int box_boxfixed , int roadx, int y, int Spacerpost,int k,double z)
{
    //A1 排 第 1 层 未知参数 相邻箱子的固定距离box_boxfixed 换向道的固定横向距离int roadx(中心位置到第一个箱子横向距离)  第二巷道的 int y 大间隔柱子相邻箱子的固定距离 int Spacerpost
//    int box_boxfixed = 20;
//    int roadx = 200;
//    int y = 600;
//    int Spacerpost  = 40;
//    int k = 1;
    //id nbr KS-10-A1-01-01
    QString  id = "NT-10-A1-01-01";
    // 第一排KS-10-A1-01-01～KS-10-A1-01-03
    QMap<QString,StorePosInfoStru> savemap;
    for(int i = 1;i <= 62;++i) //第一排
    {
         QString strid = "";
         QString layer = "";
         QString nbr = "";
         if(z >=1&& z<=9)
         {
             layer = QString("NT-10-A1-0%1-").arg(z);
         }
         else{
             layer = QString("NT-10-A1-%1-").arg(z);
         }
         if(i >= 1&& i <= 9)
         {
             nbr = QString("0%1").arg(i);
         }
          else if(i >= 10 && i <= 99) {
           nbr = QString("%1").arg(i);
         }
          strid = layer + nbr;
         QString box;
         if(k >= 1 && k <= 9)
         {
             box = QString("BOX000%1").arg(k);
         }
         else if(k>=10 && k<=99) {
             box = QString("BOX00%1").arg(k);
         }
         else if(k>=100 && k<=999) {
             box = QString("BOX0%1").arg(k);
         }
         else if(k>=1000 && k<=9999) {
             box = QString("BOX%1").arg(k);
         }
        StorePosInfoStru stru;
        memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
        memcpy(stru.boxnbr,box.toStdString().c_str(),box.size());
        stru.coordy = 0;
        stru.storepri = k;
        stru.coordz = z;
        double xcood = 0;
        if(i>=1 && i<= 3) //KS-10-A1-01-01～KS-10-A1-01-03
        {
          xcood = roadx+(i-1)*box_boxfixed;
        }
        else if(i >= 4 && i<= 28)//KS-10-A1-01-04～KS-10-A1-01-28
        {
          int j = (i-4)/5+1;
          xcood = roadx+((j-1)*4+2+i-(4+(j-1)*5))*box_boxfixed + j*Spacerpost;
        }
        else if(i >= 29 && i<= 58)//KS-10-A1-01-29～KS-10-A1-01-58
        {
            int j = (i-29)/5+1; //一共6组
            xcood = roadx+5*Spacerpost + 22*box_boxfixed + 2*roadx +((j-1)*4+i-(29+(j-1)*5))*box_boxfixed+(j-1)*Spacerpost;
        }
        else if(i >= 59 && i<= 62)//KS-10-A1-01-59～KS-10-A1-01-62
        {
            //59个坐标值roadx+5*Spacerpost + 22*box_boxfixed + 2*roadx +24*box_boxfixed+5*Spacerpost
           xcood =  roadx+5*Spacerpost + 22*box_boxfixed + 2*roadx +24*box_boxfixed+5*Spacerpost  + 2*roadx + (i-59)*box_boxfixed;
        }
        stru.coordx = -(xcood);
        savemap.insert(strid,stru);
        k++;
    }
//    //A2排 63-126
   // int m  = 1;
    for(int i = 1;i <= 64;++i) //第二排
    {
        QString strid = "";
        QString layer = "";
        QString nbr = "";
        if(z >=1&& z<=9)
        {
            layer = QString("NT-10-A2-0%1-").arg(z);
        }
        else{
            layer = QString("NT-10-A2-%1-").arg(z);
        }
        if(i >= 1&& i <= 9)
        {
            nbr = QString("0%1").arg(i);
        }
         else if(i >= 10 && i <= 99) {
          nbr = QString("%1").arg(i);
        }
         strid = layer + nbr;
         QString box;
         if(k >= 1 && k <= 9)
         {
             box = QString("BOX000%1").arg(k);
         }
         else if(k>=10 && k<=99) {
             box = QString("BOX00%1").arg(k);
         }
         else if(k>=100 && k<=999) {
             box = QString("BOX0%1").arg(k);
         }
         else if(k>=1000 && k<=9999) {
             box = QString("BOX%1").arg(k);
         }
        StorePosInfoStru stru;
        memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
        memcpy(stru.boxnbr,box.toStdString().c_str(),box.size());
        stru.coordy = 0;
        stru.storepri = k;
        stru.coordz = z;
        double xcood = 0;
        if(i>=1 && i<= 2) //KS-10-A2-01-01～KS-10-A2-01-02
        {
          xcood = roadx+ box_boxfixed - (i-1)*box_boxfixed;
        }
        else if(i >= 3 && i<= 5)//KS-10-A2-01-03～KS-10-A2-01-05
        {
        // int j = (i-4)/5+1;
          xcood = -(roadx+(i-3)*box_boxfixed);
        }
        else if(i >= 6 && i<= 30)//KS-10-A2-01-06～KS-10-A2-01-30
        {
            int j = (i-6)/5+1; //一共5组
            xcood = -(roadx+2*box_boxfixed + (i-(6+(j-1)*5))*box_boxfixed +(j-1)*4*box_boxfixed +(j)*Spacerpost);
        }
        else if(i >= 31 && i<= 60)//KS-10-A2-01-31～KS-10-A2-01-60 一共6组
        {
           int j = (i-31)/5+1; //一共6组
           xcood = -((roadx+2*box_boxfixed+20*box_boxfixed+ 5*Spacerpost)  + 2*roadx + (i-(31+(j-1)*5))*box_boxfixed +(j-1)*4*box_boxfixed + (j-1)*Spacerpost) ;
        }
        else if(i >= 61 && i<= 64)//KS-10-A2-01-61～KS-10-A2-01-64
        {
           xcood =  -((roadx+2*box_boxfixed+20*box_boxfixed+ 5*Spacerpost)+2*roadx + 24*box_boxfixed+5* Spacerpost  + 2*roadx + (i-61)*box_boxfixed);
        }
        stru.coordx = xcood;
        savemap.insert(strid,stru);
        k++;
    }
    //A3排
    for(int i = 1;i <= 64;++i) //第三排
    {
        QString strid = "";
        QString layer = "";
        QString nbr = "";
        if(z >=1&& z<=9)
        {
            layer = QString("NT-10-A3-0%1-").arg(z);
        }
        else{
            layer = QString("NT-10-A3-%1-").arg(z);
        }
        if(i >= 1&& i <= 9)
        {
            nbr = QString("0%1").arg(i);
        }
         else if(i >= 10 && i <= 99) {
          nbr = QString("%1").arg(i);
        }
         strid = layer + nbr;
         QString box;
         if(k >= 1 && k <= 9)
         {
             box = QString("BOX000%1").arg(k);
         }
         else if(k>=10 && k<=99) {
             box = QString("BOX00%1").arg(k);
         }
         else if(k>=100 && k<=999) {
             box = QString("BOX0%1").arg(k);
         }
         else if(k>=1000 && k<=9999) {
             box = QString("BOX%1").arg(k);
         }
        StorePosInfoStru stru;
        memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
        memcpy(stru.boxnbr,box.toStdString().c_str(),box.size());
        stru.coordy = y;
        stru.storepri = k;
        stru.coordz = z;
        double xcood = 0;
        if(i>=1 && i<= 2) //KS-10-A3-01-01～KS-10-A3-01-02
        {
          xcood = roadx+box_boxfixed-(i-1)*box_boxfixed;
        }
        else if(i >= 3 && i<= 5)//KS-10-A3-01-03～KS-10-A3-01-05
        {
        // int j = (i-4)/5+1;
          xcood = -(roadx+(i-3)*box_boxfixed);
        }
        else if(i >= 6 && i<= 30)//KS-10-A3-01-06～KS-10-A3-01-30
        {
            int j = (i-6)/5+1; //一共5组
            xcood = -(roadx+2*box_boxfixed + (i-(6+(j-1)*5))*box_boxfixed +(j-1)*4*box_boxfixed +(j)*Spacerpost);
        }
        else if(i >= 31 && i<= 60)//KS-10-A3-01-31～KS-10-A3-01-60 一共6组
        {
           int j = (i-31)/5+1; //一共6组
           xcood = -((roadx+2*box_boxfixed+20*box_boxfixed+ 5*Spacerpost)  + 2*roadx + (i-(31+(j-1)*5))*box_boxfixed +(j-1)*4*box_boxfixed + (j-1)*Spacerpost) ;
        }
        else if(i >= 61 && i<= 64)//KS-10-A3-01-61～KS-10-A3-01-64
        {
           xcood =  -((roadx+2*box_boxfixed+20*box_boxfixed+ 5*Spacerpost)+2*roadx + 24*box_boxfixed+5* Spacerpost  + 2*roadx + (i-61)*box_boxfixed);
        }
        stru.coordx = xcood;
        savemap.insert(strid,stru);
        k++;
    }
//   //A4排
    for(int i = 1;i <= 62;++i) //第一排
    {
        QString strid = "";
        QString layer = "";
        QString nbr = "";
        if(z >=1&& z<=9)
        {
            layer = QString("NT-10-A4-0%1-").arg(z);
        }
        else{
            layer = QString("NT-10-A4-%1-").arg(z);
        }
        if(i >= 1&& i <= 9)
        {
            nbr = QString("0%1").arg(i);
        }
         else if(i >= 10 && i <= 99) {
          nbr = QString("%1").arg(i);
        }
         strid = layer + nbr;
         QString box;
         if(k >= 1 && k <= 9)
         {
             box = QString("BOX000%1").arg(k);
         }
         else if(k>=10 && k<=99) {
             box = QString("BOX00%1").arg(k);
         }
         else if(k>=100 && k<=999) {
             box = QString("BOX0%1").arg(k);
         }
         else if(k>=1000 && k<=9999) {
             box = QString("BOX%1").arg(k);
         }
        StorePosInfoStru stru;
        memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
        memcpy(stru.boxnbr,box.toStdString().c_str(),box.size());
        stru.coordy = y;
        stru.storepri = k;
        stru.coordz = z;
        double xcood = 0;
        if(i >= 1 && i <= 3) //KS-10-A1-01-01～KS-10-A1-01-03
        {
          xcood = roadx+(i-1)*box_boxfixed;
        }
        else if(i >= 4 && i<= 28)//KS-10-A1-01-04～KS-10-A1-01-28
        {
          int j = (i-4)/5+1;
          xcood = roadx+((j-1)*4+2+i-(4+(j-1)*5))*box_boxfixed + j*Spacerpost;
        }
        else if(i >= 29 && i<= 58)//KS-10-A1-01-29～KS-10-A1-01-58
        {
            int j = (i-29)/5+1; //一共6组
            xcood = roadx+5*Spacerpost + 22*box_boxfixed + 2*roadx +((j-1)*4+i-(29+(j-1)*5))*box_boxfixed+(j-1)*Spacerpost;
        }
        else if(i >= 59 && i<= 62)//KS-10-A1-01-59～KS-10-A1-01-62
        {
            //59个坐标值roadx+5*Spacerpost + 22*box_boxfixed + 2*roadx +24*box_boxfixed+5*Spacerpost
           xcood =  roadx + 5*Spacerpost + 22*box_boxfixed + 2*roadx + 24*box_boxfixed + 5*Spacerpost  + 2*roadx + (i-59)*box_boxfixed;
        }
        stru.coordx = -(xcood);
        savemap.insert(strid,stru);
        k++;
    }
   //结构体存储完成重组之后进行写入到数据库
    QString msg = "";
    if(m_databaseopob.WriteNTStoreposinfotoDataBase(savemap,msg,"t_ntstoreposinfo"))
    {
        qDebug()<<"数据插入数据库的结果"<<msg;
    }
}

void StoreInfoWidget::WriteNTstackerposdinfoOld(int layermax,int colmunmax)
{
    //堆垛机数据内容
    int k = 0;
    QMap<QString ,StorePosInfoStru> savemap;
    for(int layer = 1;layer <= layermax;++layer )
    {
        for(int j = 1; j <= 2; ++j)
        {
            for(int i = 1;i <= colmunmax;++i)
            {
                QString strid = "";
                StorePosInfoStru stru;
                strid =  QString("NT-20-A%1-").arg(j);
                if(layer >= 1&& layer <= 9)
                {
                    strid = strid + QString("0%1-").arg(layer);
                }
                else if(layer >= 10 && layer <= 99) {
                    strid = strid + QString("%1-").arg(layer);
                }
                if(i >= 1 && i <= 9)
                {
                    strid = strid + QString("0%1").arg(i);
                }
                else if(i >= 10 && i <= 99) {
                    strid = strid + QString("%1").arg(i);;
                }
                stru.coordx = j;
                stru.storepri = k;
                memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
                stru.coordy = i;
                stru.coordz = layer;
                savemap.insert(strid,stru);
                k++;
            }
        }
    }
    //结构体存储完成重组之后进行写入到数据库
     QString msg = "";
     if(m_databaseopob.WriteNTStoreposinfotoDataBase(savemap,msg,"t_ntstackerposinfo"))
     {
         qDebug()<<"数据插入数据库的结果"<<msg;
     }
}

void StoreInfoWidget::WriteNTstackerposdinfonew(int layermax, int colmunmax)
{
    //堆垛机数据内容
    int k = 0;
    QMap<QString ,StorePosInfoStru> savemap;
    for(int layer = 1;layer <= layermax;++layer )
    {
        for(int j = 1; j <= 2; ++j)
        {
            for(int i = 1;i <= colmunmax;++i)
            {
                QString strid = "";
                StorePosInfoStru stru;
                strid =  QString("NT-20-A%1-").arg(j);
                if(layer >= 1&& layer <= 9)
                {
                    strid = strid + QString("0%1-").arg(layer);
                }
                else if(layer >= 10 && layer <= 99) {
                    strid = strid + QString("%1-").arg(layer);
                }
                if(i >= 1 && i <= 9)
                {
                    strid = strid + QString("0%1").arg(i);
                }
                else if(i >= 10 && i <= 99) {
                    strid = strid + QString("%1").arg(i);;
                }
                stru.coordx = i;
                stru.storepri = k;
                memcpy(stru.idnbr,strid.toStdString().c_str(),strid.size());
                stru.coordy = layer;
                stru.coordz = j;
                savemap.insert(strid,stru);
                k++;
            }
        }
    }
    //结构体存储完成重组之后进行写入到数据库
     QString msg = "";
     if(m_databaseopob.WriteNTStoreposinfotoDataBase(savemap,msg,"t_ntstackerposinfo"))
     {
         qDebug()<<"数据插入数据库的结果"<<msg;
     }
}

///
/// \brief StoreInfoWidget::SlotDelSinglerow
/// \param nbrinfo
///表格中删除按钮
void StoreInfoWidget::SlotDelSinglerow(QString nbrinfo,int row)
{
    QList<QVariant> list;
    list.append(nbrinfo);
    if(DelDataBaseInfo(list))
    {
        m_ptableview->Delsinglerow(row);
    }
}
///
/// \brief StoreInfoWidget::SlotBatchDelData
/// \param nbrlist
///批量删除数据库中编号信息
void StoreInfoWidget::SlotBatchDelData(  QList<QVariant> nbrlist)
{
    DelDataBaseInfo(nbrlist);

}
///更新page界面状态情况
void StoreInfoWidget::SlotupdatePageUI(int size)
{
    //根据记录数量重新计算总的页数
    m_pagewg->m_totalRecrodCount = size;
    m_pagewg->m_totalPage = m_ptableview->GetStoretablemodel()->GetPageSize();
    if(m_pagewg->m_currentPage >= m_pagewg->m_totalPage )//如果计算之后的页数小于当前页那么当前页更新0
    {
        m_pagewg->m_currentPage = 0;
    }
    m_pagewg->updateParam();
}
///
/// \brief StoreInfoWidget::DelDialogBaseob
///
void StoreInfoWidget::DelDialogBaseob()
{
    QList<EditStorenbrinfoDialog *> list = this->findChildren<EditStorenbrinfoDialog*>();
    if(list.size() == 0)
        return;
    foreach (EditStorenbrinfoDialog* w, list) {
        w->hide();
        w->deleteLater();
    }
}

