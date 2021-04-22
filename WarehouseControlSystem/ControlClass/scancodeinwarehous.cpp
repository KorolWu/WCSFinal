#include "scancodeinwarehous.h"

ScanCodeInWarehous::ScanCodeInWarehous()
{

}

void ScanCodeInWarehous::handleScanCode()
{
    while(Myconfig::GetInstance()->m_flag)
    {
        for (auto it = Myconfig::GetInstance()->m_RunnerMap.begin();it != Myconfig::GetInstance()->m_RunnerMap.end();it++) //不同流道同时检测
        {
            if(it.value().bitstate[0].bbit0 == 0 && it.value().bitstate[0].bbit2 == 1)//请求扫码 且2层不可取
            {
                //发送扫码请求

                //放行
            }
        }
    }
}

void ScanCodeInWarehous::stopScanCode()
{
    m_flag = false;
}
