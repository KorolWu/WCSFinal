#ifndef ALLTASK_H
#define ALLTASK_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QHeaderView>
#include <QTableView>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMutexLocker>
#include "labeldelegateV1.h"
#include "../basefrom.h"
#include "Myconfig.h"
#include "UnitClass/TablepageWgClass/spiltpagesbyqsqltablemodel.h"

class AllTask : public BaseFrom
{
    Q_OBJECT
public:
    AllTask(int width,int height,QWidget *parent);
    QPushButton *m_in_btn;
    QPushButton *m_out_btn;
    QPushButton *m_all_btn;
    QDateEdit   *m_start_date;
    QDateEdit   *m_end_date;
    QPushButton *m_query_btn;
    QTableView  *m_table_view;
    QPushButton *m_Refresh_btn;
    QSqlQueryModel *model;
    SpiltPagesByQSqlTableModel *m_pagewg;\

    QHBoxLayout *m_hbox;
    QVBoxLayout *m_vbox;

    void initTableView();
    int m_width;
    int m_height;
public slots:
    void onRefreshed();
private:
    void setTableStysle();
};

#endif // ALLTASK_H
