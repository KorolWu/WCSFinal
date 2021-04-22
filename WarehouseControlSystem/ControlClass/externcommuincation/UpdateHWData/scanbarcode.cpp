#include "scanbarcode.h"
#include <QThread>
ScanBarCode::ScanBarCode()
{

}

int ScanBarCode::SetScanBarParamByFrame(QByteArray tempData, int ID)
{
     QMutexLocker locker(&Myconfig::GetInstance()->m_mutex);
    if( Myconfig::GetInstance()->m_ScanMap.contains(ID))
    {
        if((tempData[0] != (char)0x06) && (tempData[0] != 'N'))
        {
            QString code = "";
            tempData.chop(1);
            code = QString::fromUtf8(tempData);
            if(ID == 40)// 入库扫码
            {
                Myconfig::GetInstance()->m_boxNum_in = code;
                Myconfig::GetInstance()->m_RunnerMap[9].Incodeinfo = code;//
                Myconfig::GetInstance()->m_RunnerMap[9].Inscanok = true;
            }
            else if(ID == 41)// 出库扫码
            {
               Myconfig::GetInstance()->m_RunnerMap[9].Outcodeinfo = code;
               Myconfig::GetInstance()->m_RunnerMap[9].m_outscanok = true;
               qDebug()<<"收到出库条码信息："<<code << QDateTime::currentDateTime().toString();
            }
        }
    }
    return 1;
}

void ScanBarCode::Updateconnectstate(int ID, bool state)
{
    //扫码枪连接创建 串口部分
    if( Myconfig::GetInstance()->m_ScanMap.contains(ID))
    {
        if(Myconfig::GetInstance()->m_ScanMap[ID].isOnline  != state)
        {
            Myconfig::GetInstance()->m_ScanMap[ID].isOnline = state;
        }
    }
}
