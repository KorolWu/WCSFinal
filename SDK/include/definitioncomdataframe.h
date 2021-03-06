#ifndef DEFINITIONCOMDATAFRAME_H
#define DEFINITIONCOMDATAFRAME_H
#include <QString>
#include <QMap>
//硬件类型 小车 流道 入库电梯 出库电梯  小车电梯 WMS
enum HWDEVICETYPE{
    RGVCAR = 1,
    ELEVATOR_IN = 2,
    ELEVATOR_OUT= 3,
    ELEVATOR_CAR = 4,
    RUNNER = 5,
    WMS = 6 ,
    BARCODE = 7,
    STACKER = 8
};
//通讯协议的类型定义 TCP 服务器 客户端 串口 modbustcp等
enum HWDEVICEPROTYPE{
    Tcpserver = 1,
    KTcpClient = 2,
    KSerialPort = 3,
    KModbusTcpClient = 4,
    KModbusSerialport = 5,
    kMCMelsecTCPClient = 6,
    KFx3uSerialport = 7,
    KOPCUAClient = 8,
    KHttpServer
};
//通讯参数配置部分结构体
struct hwcomBasestru{
    int ID = -1;
    int hwtype = 0;
    int childtype = 0;
    int protype =0;
    QString name = "";
    QString unused = "";
};

struct  TcpStru : public hwcomBasestru {

    int port = 0;
};
struct SerialPortstru:public hwcomBasestru{

    int BaudRate = 0;
    int DataBits = 0;
    int Parity = 0;
    int StopBits = 0;
    int bytesize = 0;
    int serveraddress = 1;
};
struct ModbusTcpClientstru:public hwcomBasestru{
    QString url_str;
    int serveraddress = 1;
};
struct HttpServerStru:public hwcomBasestru{

};
struct ComConfigStru{
    TcpStru hwTcpstru;
    SerialPortstru hwserialstru;
    ModbusTcpClientstru hwmodbustcpclistru;
    HttpServerStru hwhttpserverstru;
};
///
/// \brief The HWComallconfigStru struct
///所有的通讯硬件相关的配置参数的内容部分
struct HWComallconfigStru{
    QMap<int,TcpStru> hwTcpMap;
    QMap<int,SerialPortstru> hwSerialPortMap;
    QMap<int,ModbusTcpClientstru> hwmodbustcpcliMap;
    QMap<int,HttpServerStru> hwhttpserverMap;
};
union atoi64
{
    int8_t a[8];
    int64_t x;
};
union atoi32
{
    int8_t a[4];
    int32_t x;
};
union atou32
{
    int8_t a[4];
    uint x;
};
union atoi16
{
    int8_t a[2];
    int16_t x;
};
union atou16
{
    int8_t a[2];
    ushort x;
};
union atoi8
{
    int8_t a;
    int8_t x;
};
#pragma pack(1)
//  WCS发送给小车的指令长度 40个长度
struct SendCarCmdFrame{
    int16_t cmdnbr = 0;// 指令编号 WCS提供
    int16_t carnbr = 0;//小车编号
    int16_t cmdname = 0;//命令1 = 直行2 = 横行 3 = 取货 4 = 放货
    int16_t backup1 = 0;//预留1
    int backup2 = 0;//预留2
    int64_t Strdistance =0;//Straight distance 直行距离 直行方向需要行走的距离,直行距离为正,表示前行;直行距离为负,表示后 退
    int64_t Tradistance = 0;//Traverse distance 横行距离
    int backup3 = 0;//预留3
    int16_t pickputdirection = 0;//取放货方向 1.左取货 2.右取货 3.左放货 4.右放货
    int8_t undefined[6] = {0};//填满40个字节
};

// WCS的数据发送请求指令 40个字节长度
struct SendCarCmdReqestFrame{
    int16_t cmdnbr = 0;// 指令编号 该序号由WCS提供
    int16_t carnbr = 0;//小车编号穿梭车的唯一编号。
    int16_t cmdname = 0;//定义数据请求类型 命令5 = 详细数据请求(穿梭车详细的传感器状态以及故障信息) 6 = 简易数据请求(穿梭车当前的状态是否可以接收指令)
    int8_t undefined[34] = {0};//填满40个字节
};
typedef union
{
    struct {
        int16_t
        berror:1, // bit0 值“1”穿梭车在自动状态下发生故障
            belectricity:1, // bit1 值“1”穿梭车电量低
            bready:1,//bit2 值“1”穿梭车就绪,可以接收指令
            bruning:1, //bit3 值“1”穿梭车正在执行指令中
            bunready:1,//bit4值“1”未就绪
            bcalibrating:1,//bit5 值“1”自校准中
            bundefined:10;//6-15 未定义的
    };
    int16_t carinfo = 0;
}ReceCarinfostru;//小车信息结构体
//状态位图结构状态位图:告知必要的传感器状态信息
typedef union
{
    struct {
        int8_t
        bLhavegoods:1, // bit0 0位 = 值“1”穿梭车左侧有货
            bRhavegoods:1, // bit1   穿梭车右侧有货
            bloaplathavegoods:1;//bit2 值“ 1穿梭车载货台内有货
    };
    int8_t carsensorstat = 0;
}ReceCaSensorgoodsinfo;//小车货传感器货物信息
struct ReceCaSensorstatinfoStru {
    ReceCaSensorgoodsinfo goodsstate;
    int8_t backup[7] = {0};
};
typedef union {
    struct {
        int8_t
        bbit0:1,// bit0
        bbit1:1,// bit1
        bbit2:1,//bit2
        bbit3:1,//bit3
        bbit4:1,//bit4
        bbit5:1,//bit5
        bbit6:1,//bit6
        bbit7:1;//bit7
    };
    int8_t errcodestate = 0;
}ERRORCODESTATESTRU;
//WCS接收小车“RB”报文。报文长度为10字节,结
struct ReceCarRBFrame{
    int16_t cmdnbr = 0;// 指令编号 此处的指令编号则为固定值2000
    int16_t carnbr = 0;//穿梭车的唯一编号。
    char cmd[2] = {0};//命令 RB(唯一)。为固定值RB
    int posinfo = 0;//穿梭车扫描到的条码信息/位置信息
};
// WCS 数据总共74个字节 接收小车响应—详细数据响应发送到
struct ReceCarDetailFrame{
    int16_t cmdnbr = 0;// 指令编号 该序号由WCS提供
    int16_t carnbr = 0;//小车编号穿梭车的唯一编号。
    char cmd[2] = {0};//命令 SD(唯一)。
    int16_t state = 0;//状态 1自动 2 手动显示穿梭车当前是处于手动状态还是自动状态
    ReceCarinfostru info;//信息二个字节告知穿梭车必要的状态信息
    ReceCaSensorstatinfoStru statepic;// 状态位图
    int16_t backup1 = 0;//预留1告知WCS最后一条动作指令的编号
    int16_t backup2 = 0;//预留2暂空
    int16_t backup3 = 0;//预留3暂空
    int16_t backup4 = 0;//预留4暂空
    ERRORCODESTATESTRU errorpic[48] ;//故障位图告知所有的故障信息。值“0”表示没有故障,值“1”表示发生故障。故障位图中每一个bit的定义
};

//wcs主动接收小车发送帧格式部分数据 简易数据报文 10个字节信息
struct ReceCarcmdsimFrame{
    int16_t cmdnbr = 0;// 指令编号， 分为主动为1000 或者 为WCS编号
    int16_t carnbr = 0;//小车编号穿梭车的唯一编号。
    char cmdname[2] = {0};//命令ST(唯一)
    int16_t carstate = 0; // 小车状态显示穿梭车当前是处于手动状态还是自动状态 1 = 手动 2 = 自动
    ReceCarinfostru info;// 信息 0位 = 值“1”穿梭车在自动状态下发生故障 1位 = 值“1”穿梭车电量低
};
//动作指令执行报文穿梭车对应的指令状态如下 报文指令长度
/// 1. 正在执行
/// 2. 异常
/// 3. 执行完成
/// 在发生上述三种状态的变化时,穿梭车会主动发送当前状态信息给WCS
struct ReceCarcmdActionFrame{
    int16_t cmdnbr = 0;// 指令编号
    int16_t carnbr = 0;//小车编号
    int16_t cmdname = 0;//命令1 = 直行2 = 横行 3 = 取货 4 = 放货
    int16_t cmdstate = 0;//1 = 正在执行 2 = 异常 3 = 指令完成
    int16_t errornbr = 0;// 故障编号 暂空
};
//通讯数据包帧格式的定义
struct FrameBaseStru{
    int hwdevice;//操作的设备类型
    int hwprotype;//操作的协议类型
    QString frameId;
    int frameindex;
};
//RGV小车的结构接受帧格式结构体
struct RGVcarReceFrameStru :public FrameBaseStru
{
    int type;// 功能类型
    int childtype;//子类型
    int carstate;//小车状态
    double curvalue;// 小车的当前的数据
};
//RGV小车发送包帧的格式结构体
struct RGVcarSendFrameStru :public FrameBaseStru
{
    int  cmdtype;// 功能类型
    int cmdchildtype;//子类型
    double  value;//设置小车 数据
};
//流道通讯数据包格式结构体
struct RunnerSendFrameStru :public FrameBaseStru
{
    int  cmdtype;// 功能类型
    int cmdchildtype;//子类型
    double  setvalue;//设置数据
};
struct RunnerReceFrameStru :public FrameBaseStru
{
    int  cmdtype;// 功能类型
    int cmdchildtype;//子类型
    double  curvalue;//当前流道的状态
};
//流道通讯数据包通过树莓派转发协议格式结构体2021 01 21--------------------
//​3:出库扫码完成指令
//​4:入库扫码完成指令
//​5: WCS取货完成指令
//​6 :WCS常规放货完成指令  到出库位置           堆垛机放货流道完成之后发给流道指令
//​7 :WCS自盘放货完成指令  到盘点位置           堆垛机放货流道完成之后发给流道指令
struct WCSsendActiontoRunnerStru//WCS发给流道动作指令结构 40个字节
{
    uint16_t cmdnbr = 0;//指令编号
    uint16_t runnernbr = 0;//流道编号
    char cmdtype[2] = {0};//帧类型固定 "SA"
    uint16_t cmdname  = 0;//指令名字  堆垛机1: 开启流道2:关闭流道3:出库扫码完成4:入库扫码完成指令5:取完成6:WCS常规放货完成指令  到出库位置 7:WCS自盘放货完成指令
    int  cmdparam = 0;//参数值
    char boxinfo[10] = {0};
    int8_t backup1[4] = {0}; //预留项1
    int8_t backup2[14] = {0};//预留项2
};
struct WCSReceRunnersendActInfoStru//wcs接收到流道动作指令反馈信息 20个字节
{
    uint16_t cmdnbr = 0;//指令编号
    uint16_t runnernbr = 0;//流道编号
    char cmdtype[2] = {0};//帧类型固定 "SA"
    uint16_t cmdname  = 0;//指令名字
    uint16_t   checkinfo = 0;//信息格式正确固定为 0x06，格式错误为0x15.
    char codeInfo[10] = {0}; //条码信息
};

struct WCSsendStatetRequestoRunnerStru//WCS发给流道状态请求指令帧结构 8个字节
{
    uint16_t cmdnbr = 0;//指令编号
    uint16_t runnernbr = 0;//流道编号
    char cmdtype[2] = {0};//帧类型固定 "SD"
    uint16_t cmdname  = 0x05;//固定为 0x05
};
struct WCSReceStatetRequestoRunnerStru//WCS收到流道反馈的状态指令请求 20个字节
{
    uint16_t cmdnbr = 0;//指令编号
    uint16_t runnernbr = 0;//流道编号
    char cmdtype[2] = {0};//帧类型固定 "SD"
    uint16_t cmdname  = 0x05;//固定为 0x05
    uint8_t state = 0;//1= 启动，2= 停止 3= 故障
    uint8_t outcache = 0; //对应流道上检测到的出库未检的数量
    uint8_t incache = 0;//对应流道下层即将入库的料箱的数量
    uint8_t  statebitmap[8] = {0};
};
//流道通讯数据包通过树莓派转发协议格式结构体2020 01 21 ---------------------
//增加MC协议数据方式
struct MCFrameHeadstru{
    int16_t header = 80;// 80 --5000（16进制）默认发送的头; 208--D000（16进制） 接收的头;
    uint8_t netindex = 0;//网络编号 默认值为0
    uint8_t plcindex = 255;//255
    uint16_t targetIOindex = 1023;//请求的目标模块IO编号 默认FF03
    int8_t targetindex = 0;//请求目标模块站号默认0
    int16_t length = 12;//后边根数据长度 批量读数据长度为12
};
struct MCFrameDataDefinestru{
    int16_t type = 16;//cpu监视定时器
    int16_t cmdtype = 1025 ;//例如批量写入 //单个写入  批量写 0114  5121 ;批量读 0104 1025
    int16_t childcmdtype = 0;// 子指令类型 批量读和写都是0
    uint8_t startaddress[3]= {0}; //代表起地址
    int8_t  softcode = 168;//软元件类型 A8 代表 D0---
    int16_t softcount = 1; //软元件数量
};
//请求读数据帧的格式
struct ComFrameSendReadMCStru{
   MCFrameHeadstru headstru;
   MCFrameDataDefinestru defineData;
};
//请求写数据帧的格式
struct ComFrameSendWriteMCstru{
      MCFrameHeadstru headstru;
      MCFrameDataDefinestru defineData;
     // int16_t Datavalue[16] = {0};
};
//MC协议接收读数据包
struct ComframeReceReadStru{
     MCFrameHeadstru headstru;
     int16_t endread = 0;
     int16_t Datavalue[16] = {0};//返回多个数据
};
//MC协议接收写数据包
struct ComframeReceWriteStru{
     MCFrameHeadstru headstru;
     int16_t endread = 0;
};

#pragma pack()
//入库电梯
//通讯数据包帧格式的定义
struct ComFrameStru{
    RGVcarReceFrameStru carRecFramestru;
    RGVcarSendFrameStru carSendFramestru;
    RunnerSendFrameStru runnersendFramestru;
    RunnerReceFrameStru  runnerReceFramestru;
};





#endif // DEFINITIONCOMDATAFRAME_H
