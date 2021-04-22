#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QString>
#include <QMap>
#include <QVector>
#include <QSize>
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <definitioncomdataframe.h>
typedef struct KBaseStruct_{
   virtual QStringList getNameList(){QStringList list;list.clear();return list;}
   virtual QList<QVariant> getValueList(){QList<QVariant> list;list.clear();return list;}
   virtual ~KBaseStruct_(){ }
}KBaseStruct;
typedef struct KPosition_{
    KPosition_(int x_,int y_,int z_,int state_):x(x_),y(y_),z(z_),state(state_){}
    KPosition_(){}
    int x;
    int y;
    int z;
    int state = 0;
}KPosition;
typedef union
{
    struct {
        int8_t
            berror:1, // bit0 值“1”穿梭车在自动状态下发生故障
            belectricity:1, // bit1 值“1”穿梭车电量低
            bready:1,//bit2 值“1”穿梭车就绪,可以接收指令
            bruning:1, //bit3 值“1”穿梭车正在执行指令中
            bunready:1,//bit4值“1”未就绪
            bcalibrating:1,//bit5 值“1”自校准中
            bundefined:2;//6-15 未定义的
    };
    int8_t carstatusinfo;
}CarstatusInfostru;//小车信息结构体
//传感器状态信息
typedef union
{
    struct {
        int8_t
        bLhavegoods:1, // bit0 0位 = 值“1”穿梭车左侧有货
            bRhavegoods:1, // bit1   穿梭车右侧有货
            bloadhavegoods:1,//bit2 值“ 1穿梭车载货台内有货
            undefined:5;//bit3-bit7 值“ 未定义
    };
    int8_t carsensorstat;
}CarSensorgoodsInfostru;//小车货传感器货物信息

typedef struct _Car_status
{
    //电量
    int batter = 20;//值是1 代表电量低 0 代表电量状态正常
    KPosition carCurrentPosion;
    //是否启用 int working warrn online  disonline
    bool enable = false;
    // 1 stand by, 2 working,
    int status = 3; //1 正在执行 2 异常 3 指令完成 小车的动作指令状态 2020/08/24解释状态
    int readonlystatus = 0;
    //locking 在执行多段任务过程中需要锁定小车
    bool isLocking = false; //锁定小车，在一个主任务没有完成之前一直被锁定
    //error
    int err_code = 0;
    // 1手动 2自动
    int model = 1;
    //0 左右都没有 1左有货 2右有货 3顶上有货
    char box_status = 0;
    // 到位信号,任务完成后小车会发送完成信号
    bool inp = false;
    //是否在线
    bool isOnline = false;
    //目标坐标
    KPosition carEndPosion;
    //新增数据结构状态信息状态 和 传感器状态信息的结构体2020 08 24
    CarstatusInfostru statusinfodstru;
    CarSensorgoodsInfostru senorgoodsstru;
}Car_status;
enum Order{
    Y = 0,                  //向Y方向移动
    X = 1,                  //向X方向移动
    ChangeWhell = 2,        //换轮子
    Left_Pickup = 3,        //左取货
    Right_Pickup = 4,       //右取货
    Call = 5,               //呼叫换层电梯
    Elevator_Near = 6,      //电梯临近点
    Elevator_In = 7,        //小车进换层电梯
    Elevator_Out = 8,       //小车出换层电梯  unlock_elevator
    Left_WorkBin = 9,       //把料箱放在出货口电梯
    Right_WorkBin = 10,     //把料箱从入库口电梯取出
    Call_WorkBin_Out = 11,  //呼叫出库电梯
    Call_WorkBin_In = 12 ,  //呼叫入库电梯
    call_Runner_Putbox = 13, //呼叫流道放货到入库电梯
    Left_Putinto = 14,       //左放货
    Right_Putinto = 15,      //右放货
    Unlock_layer = 16,       //将小车锁定的层解锁（小车进电梯时）
    Scan_Code    = 17,       //
    Runner_Start = 18,       //流道启动
    Runner_Stop = 19,        //流道停止
    Runner_Scancode1_Out = 20, //出库扫码完成
    Runner_Scancode2_In = 21,// 入库扫码完成
    Runner_CarPickFinished = 22,//取货完成
    Runner_CarputFinished = 23,//放货完成 常规出库位置
    Runner_CarputcheckFinished = 24,//放货完成 常规出库位置
    Runner_InPushBox = 25,     //入库出箱指令
    In_elevator_call = 26,      //在电梯里面地调度电梯
    Stacker_Carry  = 27 ,
};
typedef struct _HttpParameter
{
    QString url;
    QByteArray body;
}HttpParameter;
//子任务的结构 任务类型，值
typedef struct _OrderStru
{
    //order type car elevator runer
    QString type = "";//硬件类型
    QString strData = ""; //字符型参数  比如扫码抢
    int16_t childtype = 5 ;  /*同一个对象不同的数据帧类型
    小车TCP协议中：5代表请求详细数据类型 6代表请求简易数据类型;
    modbus协议 数据类型值等于5代表请求读其他代表请求写  流道协议代表状态请求指令*/
    Order order;
    double value;
    int layer_y;
    QString box_num;
    QString shelves;
    int z;
   //新增数据结构2020-08-29
    int startaddress = 0;//modbus协议请求的读写的开始地址
    int numberOfEntries = 1;//modbus协议请求读写的变量的数量
    int Datatype = 1;// 1：io 只读 2：线圈读写 3：寄存器只读 二个字节 4：保存寄存器二个字节 可读写
    QVector<int> values;//modbus协议请求读写的值
    HttpParameter http;
    QVector<QString> Rcmdstackernames;
    QMap<QString,QVariant> WcmdstackerValues;
    //为了堆垛机新增   托盘类型 在指令里面写死
    KPosition sourcePos;     //源列
    KPosition destinationPos;//目标列
}OrderStru;
typedef struct _CarInfoStru
{
    //小车编号
    int carId = -1;
    QString deviceNum = "";
    QString deviceIp = "";
    int port = 0;
    QString deviceType = "";
    QString remarks = "";
    //被任务预定，锁定不被其他任务分配
    bool isLockByTask = false;
    Car_status deveceStatus;
    //记录正在执行的任务
    KPosition task_position;

    _CarInfoStru& operator = (const _CarInfoStru &other)
    {
        carId = other.carId;
        deviceNum = other.deviceNum;
        deviceIp = other.deviceIp;
        port = other.port;
        deviceType = other.deviceType;
        remarks = other.remarks;
        deveceStatus.batter = other.deveceStatus.batter;
        deveceStatus.enable = other.deveceStatus.enable;
        deveceStatus.status = other.deveceStatus.status;
        deveceStatus.isOnline = other.deveceStatus.isOnline;
        deveceStatus.carCurrentPosion.x = other.deveceStatus.carCurrentPosion.x;
        deveceStatus.carCurrentPosion.y = other.deveceStatus.carCurrentPosion.y;
        deveceStatus.carCurrentPosion.z = other.deveceStatus.carCurrentPosion.z;
        deveceStatus.carEndPosion.x = other.deveceStatus.carEndPosion.x;
        deveceStatus.carEndPosion.y = other.deveceStatus.carEndPosion.y;
        deveceStatus.carEndPosion.z = other.deveceStatus.carEndPosion.z;
        deveceStatus.isLocking = other.deveceStatus.isLocking;
        deveceStatus.inp = other.deveceStatus.inp;
        return *this;
    }

}CarInfoStru;
//数据库配置参数
typedef struct _DataBaseInfoStru
{
    QString sqlName;
    QString databaseName;
    QString ip;
    int port;
    QString userName;
    QString passWord;
    _DataBaseInfoStru()
    {
        sqlName = "";
        databaseName = "";
        ip = "";
        port = 3306;
        userName = "";
        passWord = "";
    }

}DataBaseInfoStru;
typedef struct _RedisInfoStru
{
    QString ip = "";
    int port = 6379;
    uint8_t state = 99;
}RedisInfoStru;
// log struct
typedef struct _LogInfoStru
{
    QString from;
    int level;
    QString info;
    _LogInfoStru()
    {
        from = "";
        level = 0;
        info = "";
    }
}LogInfoStru;
typedef struct _ElevatorStatus
{
    int curruntLayer;
    int curachelayer;
    bool isLock;
    //是否可以呼叫
    bool usable;
    bool hasCar;
    bool isOnline;
}ElevatorStatus;
typedef struct _ElevatorInfoStru
{
    //"编号"<<"类型"<<"通讯地址"<<"端口"<<"靠近点位置"<<"位置"<<"remarks"
    int elevatorId;
    ElevatorStatus status;
    QString elevatorNum;
    QString elevatorType;
    QString elevatorIp;
    int     elevatorPort;
    QString elevatorNearPos;
    QString elevatorPos;
    QString remarks;
    double  near_x;
    double  near_y;
    double  elevator_x;
    double  elevator_y;
    _ElevatorInfoStru()
    {
        elevatorNum = "";
        elevatorType = "";
        elevatorIp = "";
        elevatorPort = 0;
        elevatorNearPos = "";
        elevatorPos = "";
        remarks = "";
    }
}ElevatorInfoStru;
//新增流道信息结构体 2020 09 02
typedef struct _RunnerInfoStru
{
    QMap<int,int> holdresMap; //地址 后面地址对应的值 输出寄存器 D0-D22
    QMap<int,int> coilMap; //地址 后面地址对应的值 bit 物理线圈
    int connectresult = 0; //== 1 connected
}RunnerInfoStru;
typedef struct _SerialportInfo
{
    QString SerialName;
    int     SerialPort;
    int     BaudRate;
    int     DataBit;
    int     Parity;
    int     StopBit;
}SerialportInfo;
typedef struct _ScanInfoStru
{
    int            deviceId;
    SerialportInfo hwInfo;
    bool           isOnline=false;
    QString scaninfo = "";
    bool scanok = false;

}ScanInfoStru;

//由WCS发过来的任务数据
typedef struct _TaskInfoStru : public KBaseStruct
{
    QString regCode;//唯一标识码
    QString interfaceType;//接口类型
    QString taskMainType; //任务主类型
    QString taskSonType;  //任务子类型
    QString taskNum;  //任务ID
    QString receiptNo;//wms单号
    QString receiptNoType;//wms单号类型
    QString shelves;//目标库位
    QString status;
    QString boxNum; //料箱号
    QString fromLocationCode;//起始库位
   //QString toLocationCode;//目标库位
    QString taskResult; //是否异常
    QString nowlocationCode; //当前库位
    QString abnormalLocationCode;//发生异常库位
    QString inbToLocationCode;//入库更改目的库位
    QString responseMessage;//  异常描述
    QString requestTimes;//请求次数
    int pripty;
    QString from; //接口调用方
    QString end;  //接口提供方
    QString carNum;
    QDateTime creatTime;
    _TaskInfoStru()
    {
        taskNum = "";
        status = "";
        boxNum = "";
        pripty = 1;
        from = "";
        end = "";
        carNum = "";
        creatTime = QDateTime::currentDateTime();
    }

    // KBaseStruct interface
public:
    QStringList getNameList(){QStringList list;list <<"taskNum"<<"taskStatus"<<"boxNum"<<"priority"<<"origin"<<"target"<<"beginTime";return list;}
    QList<QVariant> getValueList()
    {
        QList<QVariant> list;
        list.append(this->taskNum);
        list.append(this->status);
        list.append(this->boxNum);
        list.append(this->pripty);
        list.append(this->from);
        list.append(this->end);
        list.append(this->creatTime.toString("yyyy-MM-dd hh:mm:ss"));
        return list;
    }
}TaskInfoStru;

//流道
typedef union {
    struct {
        uint8_t
        bbit0:1,// bit0
        bbit1:1,// bit1
        bbit2:1,//bit2
        bbit3:1,//bit3
        bbit4:1,//bit4
        bbit5:1,//bit5
        bbit6:1,//bit6
        bbit7:1;//bit7
    };
    uint8_t statebit = 0;
}STATEBITMAPSTRU;
//第一个字节的bit位定义
//状态位图:bit 0 = 1 可取
//​bit 1 = 1 可放
//​bit 2 = 1 入库请求扫码
//​bit 3 = 1 出库请求扫码
//第二个字节每个bit代表出库缓存有无货的状态
//第三个字节代表入库缓存区 有无货状态
//第四个字节 代表自盘位置有无货的状态
typedef struct _RunerStru
{
    QString deviceNum = "";
    QString deviceIp = "";
    int port = 0;
    bool isOnline = false;
    uint8_t cache_in_current = 0;//入库缓存区域
    uint8_t cache_in_max = 0;
    uint8_t cache_out_max = 0;
    uint8_t cache_out_current = 0; //出库缓存
    uint8_t state = 0;//1= 启动，2= 停止 3= 故障
    QString request_in = "";
    QString response_in = "";
    QString request_out = "";
    QString response_out = "";
    RunnerInfoStru runneratastru;
    STATEBITMAPSTRU bitstate[8];//
    QString  Incodeinfo = "";
    QString Outcodeinfo = "";
    bool Inscanok = false;
    //出库扫码状态信息结构 2021 02 16
    QMap<int,QString> m_RunnerBoxCodeInfoMap; //key 代表A1-A8 value 代表箱子的条码信息
    bool m_outscanok = false;
    _RunerStru& operator = (const _RunerStru &other)
    {
        deviceNum = other.deviceNum;
        deviceIp = other.deviceIp;
        port = other.port;
        cache_in_max = other.cache_in_max;
        cache_out_max = other.cache_out_max;
        request_in = other.request_in;
        response_in = other.response_in;
        request_out = other.request_out;
        response_out = other.response_out;
        state = other.state;
        memcpy(bitstate,other.bitstate,8);
        Incodeinfo = other.Incodeinfo;
        Outcodeinfo = other.Outcodeinfo;
        Inscanok = other.Inscanok;
        return *this;
    }
}RunerStru;
enum LayerState{
    UndefinedLayerState = 0,
    Free =  1,//无车状态
    ReadyInLayer  = 2,//即将有车到达
    InLayer = 3//当前楼层有车且锁定
};
// 关于层的一个结构体，用来判断当前层是否锁住，以及锁定的小车的ip
typedef struct _LayerStru
{
    //bool isLocked = false;
    LayerState layerstate;
    QString CarIP = "";
    int     carId = 9999;

}LayerStru;
// Data Union
union Kint64
{
    qint64 v;
    char   c[8];
};
union kint16
{
    qint16 v;
    char   c[2];
};
union kint32
{
    qint32 v;
    char   c[4];
};
union Kbyte2
{
    QByteArray buff;
    char c[2];
};

//堆垛机数据结构
typedef struct _StackerRead
{
    qint16 handShake = 0; //srm与wcs的握手信号，0/1跳变
    qint16 srmNo = 0;     //srm No
    qint16 mode = 0;      //=1自动模式 =2手动模式 =3半自动模式 =4维修模式 =0关机模式
    qint16 alarm = 0;     //=1 srm处于报警状态
    qint16 liftFull = 0;  //=1 装载台有货
    qint16 posX = 0;      //
    qint16 posY = 0;
    qint16 posZ = 0;      //srm货叉位置 =0在原位 =1在左侧 =2在右侧
    qint32 posXmm = 0;    //当前列坐标单位mm
    qint32 posYmm = 0;    //当前层坐标单位mm
    qint32 posZmm = 0;    //当前货叉坐标单位mm
    qint16 pickFinish = 0;//=1取货完成
    qint16 deliveryFinish = 0;//=1 放货完成
    qint16 taskFinish = 0; //=1 srm任务完成
    qint16 Alarm_number = 0;//报警代码
    qint16 state = 0;//srm当前任务执行状态 =0空闲 =1取货定位中 =2请求取货 =3取货中 =4取货完成放货定位中 =5请求放货 =6放货中 =99报警
    qint16 ActualLine = 0;//当前巷道
    qint32 TaskNo = 0;//任务号
    qint16 ack = 0;//应答位(wcs只读)
}StackerRead;
typedef struct _StackerWrite
{
    qint16 handShake = 0; //srm与wcs的握手信号，0/1跳变
    qint16 srmNo = 0;     //srm No
    qint16 palletType = 0;//托盘类型
    qint16 sourcePosX = 0;//源列
    qint16 sourcePosY = 0;//源层
    qint16 sourcePosZ = 0;//源排
    qint16 destinationPosX = 0;//目标列
    qint16 destinationPosY = 0;//目标层
    qint16 destinationPosZ = 0;// 目标排
    qint16 command = 0;//=0无  =1搬运  =2单方  =3移动  =4取消
    qint16 emergencyStop = 0;// =1srm紧急停止
    qint16 alarmAck = 0;     //=1srm报警解除
    qint16 taskFinishAck = 0;//=1srm任务完成确认
    qint16 reserve1 = 0;
    qint16 reserve2 = 0;
    qint16 stb = 0;//wcs通讯位
    qint16 ack = 0;//应答位(wcs只读)
    qint32 taskNo = 0;//task number;
}StackerWrite;
typedef struct _StackerInfo
{
    StackerRead Stacker_r;
    StackerWrite Stacker_w;
    bool online = false;
}StackerInfo;



//用户登录信息结构体 1字节对齐
#pragma pack(1)
typedef struct  _LoginInfoStru {
    char username[32];//登录用户名字
    char prilevel;//用户权限级别等级0 管理员高级模式  1 工程师模式  2 普通员工模式  3游客模式，只能查看页面，可以点击登录按钮
    char passwd[32];//用户密码
    _LoginInfoStru(){
        memset(username,0,32);
        prilevel = 3;
        memset(passwd,0,32);
    }
    _LoginInfoStru& operator=(const _LoginInfoStru &other)
    {
        strncpy(username,other.username,32);
        strncpy(passwd,other.passwd,32);
        return *this;
    }
}LoginInfoStru;
typedef struct  _MapLoginInfoStru {
    QMap<QString,LoginInfoStru> loginInfoMap;
    _MapLoginInfoStru(){
        loginInfoMap.clear();
    }
}MapLoginInfoStru;
//用户登录信息----------------------------------------2020/06/01
//货架信息结构体描述指料箱所放置的位置-------------------------------2020/06/02
typedef struct  _StorePosInfoStru{
    char idnbr[64];//仓位编号信息
    char type; //仓位类型
    double coordx;//仓位的x坐标信息
    double coordy;//仓位的y坐标信息
    double coordz;//仓位的z坐标信息
    char boxnbr[64];//仓位对应料箱信息编号
    char storestat; //0 未使用 1 有 使用中  2 锁定状态 针对维修的情况  3
    int storepri; //仓位优先级
    uint8_t directionstate = 0; //  新增 方向状态字段
    QString unused ="";//  新增 备用字段
    bool update = false;
    _StorePosInfoStru(){
        memset(idnbr,0,64);
        type = 0;
        coordx = 0.0;
        coordy = 0.0;
        coordz =0.0;
        memset(boxnbr,0,64);
        storestat = 0;
        storepri = 0;
    }
    _StorePosInfoStru& operator=(const _StorePosInfoStru &other)
    {
        strncpy(idnbr,other.idnbr,64);
        strncpy(boxnbr,other.boxnbr,64);
        return *this;
    }
}StorePosInfoStru;
typedef struct  _MapStorePosInfoStru {
    QMap<QString,StorePosInfoStru> storeposInfoMap;
    _MapStorePosInfoStru(){
        storeposInfoMap.clear();
    }
}MapStorePosInfoStru;
//仓位状态的几种定义
enum STORESTATE{
    ENBLED = 0,
    USING = 1,
    LOCKING =2,
    UNDEFINED  =3
};
//货架信息结构体描述指料箱所放置位置信息表------------------------------2020/06/02
//报警信息结构体定义-----------------------------------------2020/07/15
typedef struct  _ALARMINFOSTRU {
    uint8_t alarmlevel = 1;
    QString deviceid =  "";
    uint8_t errortype = 1;
    int errorcode = 0;  //1=小车故障 2=电梯故障 3=流道故障 4=流程故障 5=wms通讯 6=数据存储故障  7=其他故障
    uint8_t Operatestate = 1;
    QString cartaskid = "";
    QString wmsTaskid = "";
    QString boxnumber= "";
    QString alarminfo= "";
    double carcoordx= 0;
    double carcoordy = 0;
    double carcoordz = 0;
    QString unused= "";
}ALARMINFOSTRU;
//alarmlevel 报警级别的定义
enum ALARMLEVEL{
    NONE = 0,
    ESPSERIOUS = 1, //一级是特别严重，颜色红色
    SERIOUS = 2,//严重，一般用橙色
    MORSERIOUS = 3,// 较重，一般用黄色
    COMSERIOUS  = 4,//一般，用蓝色表示
};
//故障类型定义
enum ERRORTYPE{
    COMM = 1,//通迅类型故障，小车通迅中断或者Wms通迅中断
    HARDWARE = 2,//小车故障，其他硬件故障
};
//操作状态定义
enum OPERATIONSTATUS{
    AUTO = 1,
    MANUAL = 2,
    OTHER = 3,
};
//报警信息结构体定义-----------------------------------------2020/07/15
#pragma pack()



#endif // DATASTRUCTURE_H
