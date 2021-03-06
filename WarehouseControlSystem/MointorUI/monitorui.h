#ifndef MONITORUI_H
#define MONITORUI_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include "ControlClass/basedatainfooperate.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include "MointorUI/monitorview.h"
#include "storeitem.h"
#include <QTimer>
using namespace StoreInfo;
class MonitorUI:public QWidget
{
public:
    MonitorUI(QWidget *parent = 0);
    ~MonitorUI();
private:
    QComboBox *m_laycombox;
    QStringList   m_laylist;
    QPushButton *m_refreshbtn;
    QGraphicsScene *m_curscene;
    QMap<double, QGraphicsScene *> m_cursceneMap;
    MonitorView *m_pview;
    double m_sizeW;
    double m_sizeH;
    double m_X;
    double m_Y;
    double m_minX;
    double m_minY;
    double k ;
    QTimer *m_timer;
    StoreItem *m_caritem;
    StoreItem *m_elevatorItem;
public slots:
    void SetCurLayUI(int index);
    void updateUIbyData();
    void updateCurSceneData();
private:
    void GetAllLayers();
    void GetstoreposSize();
    void SetSceneMapData();
    void SetUIDataItem();
    QString FindStorePos(char &state,QString &text, QMap<QString, StorePosInfoStru> laymap,char direction ,double x,double y);
private://地图轨迹规划部分
    void UpdateCarPosPathVec();
    void UpdateCarPosInScene();
    void UpDateElevatorlayer();
    KPosition m_carpos;
    QVector<double> m_xpathppos;
    QVector<double> m_ypathppos;
    KPosition GetCarPos(int index);
    double m_coefficient;
    bool m_running;
    QTimer *m_scanPathtimer;
    double m_curelevatorLayer;
private slots:
    void slotCarPathsimulation();



};

#endif // MONITORUI_H
