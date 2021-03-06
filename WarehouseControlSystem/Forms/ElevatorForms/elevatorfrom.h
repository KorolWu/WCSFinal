#ifndef ELEVATORFROM_H
#define ELEVATORFROM_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <Forms/basefrom.h>
#include <QDebug>
#include <QtGui>
#include <QSpinBox>
#include <QStandardItemModel>
#include "Forms/ElevatorForms/addelevatorform.h"
#include "Forms/CarForms/commsetwidget.h"
#include "Myconfig.h"
#include "datastructure.h"
#include "MysqlDataBase/crudbaseoperation.h"
#include "Forms/ElevatorForms/elevatorstatus.h"
/**
 *
 * @author korol
 * @email wuwenpeng@cowain.com.cn
 * @date 2020-06-01 16:02:59
 * use 小车硬件信息显示
 */
class ElevatorFrom : public BaseFrom
{
    Q_OBJECT
public:
    explicit ElevatorFrom(int width, int height,QWidget *parent = nullptr);
    void setTableViewValue();
signals:

public slots:
    void onAddClicked();
    void onOperationClicked();
    void onEditClicked();
    void refreshTable();
    void onDeleteClicked();
private:
    QTableView *p_table_view;
    QPushButton *p_add_btn;
    QPushButton *p_delete_btn;
    QPushButton *p_export_btn;
    QPushButton *p_query_btn;
    QLineEdit *p_input_text;
    QStandardItemModel *model;
    QPushButton *p_operation_btn;
    QPushButton *p_edit_btn;
    void addControlForTableView();
    int m_width;
    int m_height;

};

#endif // ELEVATORFROM_H
