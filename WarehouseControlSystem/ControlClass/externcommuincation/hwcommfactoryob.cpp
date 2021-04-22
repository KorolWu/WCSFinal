#include "hwcommfactoryob.h"
#include "tcommtcpclient.h"
#include "tcommtcpserver.h"
#include "tcommmodbustcpclient.h"
#include "tcommserialport.h"
#include "tcommmodbusserial.h"
#include "tcommmcmelsectcpclient.h"
#include "tcommfx3userialport.h"
#include "tcommopcuaclient.h"

HWCommFactoryOb::HWCommFactoryOb()
{

}

HWdeviceabstractInterface *HWCommFactoryOb::CreatorHWCommFactoryOb(int protype)
{
    HWdeviceabstractInterface *ob = nullptr;
    switch (protype)
    {
    case HWDEVICEPROTYPE::Tcpserver:
        ob = new TCommTCPserver();
        break;
    case HWDEVICEPROTYPE::KTcpClient:
        ob = new TCommTCPclient();
        break;
    case HWDEVICEPROTYPE::KModbusTcpClient:
        ob = new TCommModbusTcpClient();
        break;
    case HWDEVICEPROTYPE::KSerialPort:
        ob = new TcommSerialport();
        break;
    case HWDEVICEPROTYPE::KModbusSerialport:
        ob = new Tcommmodbusserial();
        break;
    case HWDEVICEPROTYPE:: kMCMelsecTCPClient:
        ob = new TCommMCMelsecTCPclient();
        break;
    case HWDEVICEPROTYPE:: KFx3uSerialport:
        ob = new TCommFx3uSerialport();
        break;
    case HWDEVICEPROTYPE:: KOPCUAClient:
        ob = new TCommOPCUAClient();
        break;
    default:
        break;
    }
    return ob;
}
