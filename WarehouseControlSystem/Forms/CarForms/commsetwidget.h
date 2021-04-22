#ifndef COMMSETWIDGET_H
#define COMMSETWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QComboBox>
#include <QDebug>
#include "../dialogabstractclass.h"
#include "addcar.h"
#include "../ElevatorForms/addelevatorform.h"
///
/// \brief The CommSetWidget class
///This class is used to set some common communication protocol parameters and save them to the database
///
class CommSetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommSetWidget(int index, QString operation, QWidget *parent = 0);

signals:

public slots:
private:
    QPoint mousePoint;
    bool mouse_press;
    int m_index;// communication type
    QString m_operation;// update or insert
    QWidget *m_pComContainer;// for communication widget
    QComboBox *m_pSelectTypeCombox;
   QPointer<DialogAbstractClass> m_pAbstrcutCommunication;
    DialogAbstractClass *CommunicationFactory();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};
#endif // COMMSETWIDGET_H
