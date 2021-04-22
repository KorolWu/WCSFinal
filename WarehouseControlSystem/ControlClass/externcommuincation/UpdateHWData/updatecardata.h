#ifndef UPDATECARDATA_H
#define UPDATECARDATA_H

#include <QObject>
#include <datastructure.h>
#include <Myconfig.h>
enum UpdateCarStatusrole
{
    RBposinfo =1,//条码位置信息
    Opermode =2,//自动/手动
    sensorstat  = 3,//左侧有货 右侧有货
    exestatus = 4,//穿梭车电量低 小车 自动状态下发生了故障 自动校准中 就绪中
    errorinfo = 5,// 故障信息图
    actioninfo = 6 ,// 动作指令反馈信息
    requestError = 7 ,//请求异常
    connectstate = 8 ,//通讯状态
    setframeindex  = 9,//索引号更新
    Errorcode = 10 ,//ERROR State
    Updateposinfo = 11
};
enum CarFramType{
    detailframe,
    actionframe
};
///
/// \brief The UpdateCarData class
/// 提供了所有需要更新部分的内容值
class UpdateCarData:public QObject
{
     Q_OBJECT
public:
    UpdateCarData();
    ~UpdateCarData();
    int SetCarParamByFrame(QByteArray dataframe, int ID,bool high);
    void Updateconnectstate(int id,int value);
    int16_t GetSendFrameIndex(int ID,int frametype);//用于取得发送的索引编号
    void ReSetIndex(int ID,int frametype);
private:
     typedef void (*UpdateRole)(int ID,int Value);//声明函数指针
     UpdateRole func[10] = {nullptr};
     void Updatemodel(int ID,int Value);
     void UpdateSensorstate(int ID,int Value);
     void Register_callback_func(UpdateCarStatusrole role,UpdateRole fun)// 函数名字作为参数 传入数组
     {
         if(role < 10)
         {
             func[role] = fun;
         }
     }
     void UpdateTestcallbackData(int ID,int Value){
         for(int i = 0; i < 10;++i)
         {
             if(func[i] != nullptr)
             {
               func[i](ID,Value);
             }
         }
     }
    void ReadSingleFrame(QByteArray Data, int ID);
    void ReadSingleFrameVtest(QByteArray Data, int ID);
    void UpdateCarFunData(int ID,int role,int Value,int type=0);
    bool CheckIndexIsOK(int ID,int frametype,int16_t nbr);
    int GetErrorcode(QByteArray array);
    int GetErrorcodeByByte(ERRORCODESTATESTRU *errorpic);
    int GetDefineNbr(char bytepos,char bitpos);
    QList<int16_t> m_senddetailindexlist;
    QList<int16_t> m_sendactionindexlist;
    bool m_RequeErrorflag;
    uint16_t m_senddetailindex;
    uint16_t m_actiontindex;
    QVector<QMap<int,int>>m_ErrorNbr;
};

#endif // UPDATECARDATA_H
