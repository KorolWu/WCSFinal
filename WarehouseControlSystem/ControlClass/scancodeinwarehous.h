#ifndef SCANCODEINWAREHOUS_H
#define SCANCODEINWAREHOUS_H

#include <QObject>
#include <Myconfig.h>
///
/// \brief The ScanCodeInWarehous class
/// 处理入库扫码问题
class ScanCodeInWarehous : public QObject
{
    Q_OBJECT
public:
    ScanCodeInWarehous();
    void handleScanCode();
    void stopScanCode();
    bool m_flag = true;
};

#endif // SCANCODEINWAREHOUS_H
