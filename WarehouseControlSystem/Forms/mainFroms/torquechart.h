#ifndef TORQUECHART_H
#define TORQUECHART_H
#include <QWidget>
#include<QChar>
#include <QtCharts/QXYSeries>
#include<QtCharts/QChartView>
#include<QtCharts/QLineSeries>
#include<QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QSplineSeries>
#include <QDateEdit>
#include <QButtonGroup>
QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QChartView;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE
class TorqueChart : public QWidget
{
       Q_OBJECT
public:
    TorqueChart(int w, int h, QWidget *parent = 0);
private:
    QChart *m_pchart;
    QVBoxLayout *m_pHbox;
    QSplineSeries *m_plinseries;
    QChartView *m_pchatrview;
    //QTabWidget *m_ptabwidget;
    // QGridLayout *m_playout;
    QValueAxis *m_paxis;
    QValueAxis *m_pyaxis;
    QWidget *m_pmainWidget;
    QDateEdit   *m_choosedate;
     QButtonGroup  *m_group;
signals:
    void signalUpDateTime(QString datestr);
private slots:
    void slotUpDateLastDate();
    void slotoperatingButtonsToggled(int,bool);
public:
    void AddChartPoint(int x, int y);
    void ClearChart();
    ~TorqueChart();

};

#endif // TORQUECHART_H
