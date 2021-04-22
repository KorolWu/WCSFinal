#ifndef DISPATCHWIDGET_H
#define DISPATCHWIDGET_H

#include <QList>
#include <QTimer>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QDateTime>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Myconfig.h"
#include "../../ControlClass/basedatainfooperate.h"
#include <ControlClass/Instructions/httpclien_k.h>
#include <ControlClass/Instructions/scancodeinstruction.h>
#include <ControlClass/Instructions/abstructinstruction.h>
#include <ControlClass/Instructions/runnerinstruction.h>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QQuickWidget>
#include <QQuickItem>

class DispatchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DispatchWidget(int width,int height,QWidget *parent = nullptr);
    ~DispatchWidget();

signals:

public slots:
    void onBoxClicked();
    void onGetBoxClicked();
    //void onScanCode();
    void updateUi();
   // void onPost();
    void onCurrentTextChange(QString current_index);
private:
    void initUI();
    void initLayerUI();
    int getHoldresRegestValue(int address);
    int m_width;
    int m_height;
    void initRightW();
    void box_in();
    void execRunnerInstruction(int startAddress, int value);
    bool checkBoxNum(const QString &str,QString &nbr);
    QWidget *m_prightW;
    QWidget *m_pLayerW;
    QTimer *m_ptimer;
    QPushButton* m_box_list[8];
    QLabel *m_layer_list[8];

    QLabel *m_layerPut_1;
    QLabel *m_scan_1;
    QLabel *m_scan_2;
    QLabel *m_layerPut_2;

    //test
    qint64 m_time;

   // QPushButton *m_pScan_1;
    //QPushButton *m_pScan_2;
   // QPushButton *m_clearBtn;
    QVBoxLayout *m_pvbox_rightw;
    QWidget *m_poutWidget;
    QPushButton *m_poutButton;
    QLineEdit *m_plineBoxNum;
   // QLineEdit *m_plineCacheIn;
  //  QLineEdit *m_plineCacheOut;
    QTextEdit *m_ptextLine_log;
   // QLineEdit *m_phttpUrl;
   // QPushButton *m_http_btn;
   // QLabel *m_status_lab;
    QLabel *m_in_task_count;
    QLabel *m_out_task_count;
    QComboBox *m_pInOutCombox;
    QHBoxLayout *m_hbox;
    QVBoxLayout *m_vbox;
    void appendInfo(QString str);


    QMap<int,QObject*> m_qmlitem; //qml 对象
    void InitLayerUI();
    void UpDateLayerState(int layer,QString carid,int state);
    void UpdateLayerUI();
    QLineEdit *m_StorenbrLineEdit;//货架对象
    bool CheckOutIsOK();
    bool CheckInIsOK();
    QPushButton *m_ClearcacheBtn;
};

#endif // DISPATCHWIDGET_H
