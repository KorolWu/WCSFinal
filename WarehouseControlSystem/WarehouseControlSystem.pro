
#-------------------------------------------------
#
# Project created by QtCreator 2020-05-26T13:13:47
#
#-------------------------------------------------

QT       += core gui sql network concurrent serialport serialbus charts opcua  qml quick quickwidgets
QTPLUGIN += qgif
QMAKE_LFLAGS += -no-pie
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WarehouseControlSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    UnitClass/jeson2object.cpp \
    UnitClass/c_systemlogmng.cpp \
    UnitClass/logininfowg.cpp \
    Forms/basefrom.cpp \
    Forms/CarForms/adddevice.cpp \
    Forms/CarForms/agvform.cpp \
    Forms/CarForms/CarStatusFrom.cpp \
    Forms/CarForms/labeldelegate.cpp \
    Forms/CarForms/progressbardelegate.cpp \
    Forms/ElevatorForms/elevatorfrom.cpp \
    Forms/StorePosInfo/buttondelegate.cpp \
    Forms/StorePosInfo/storeinfowidget.cpp \
    Forms/StorePosInfo/storenbrinfotablemodel.cpp \
    Forms/StorePosInfo/storenbrtableview.cpp \
    Forms/StorePosInfo/watingdialogwg.cpp\
    Forms/ElevatorForms/addelevatorform.cpp \
    Forms/dialogabstractclass.cpp \
    Forms/CarForms/addcar.cpp \
    Forms/TaskForms/currenttask.cpp \
    Forms/TaskForms/alltask.cpp \
    UnitClass/logmanager.cpp \
    UnitClass/ImportExportFile/localfileoperate.cpp \
    Forms/StorePosInfo/editstorenbrinfodialog.cpp \
    Forms/LogForms/logforms.cpp \
    Forms/TaskForms/labeldelegateV1.cpp \
    UnitClass/khttpserver.cpp \
    UnitClass/KCommunication.cpp \
    ControlClass/basedevice.cpp \
    Forms/CarForms/carlistform.cpp \
    ControlClass/observerbase.cpp \
    ControlClass/selectcar.cpp \
    UnitClass/TablepageWgClass/spiltpagesbyqsqltablemodel.cpp \
    Forms/StorePosInfo/pagewg/tablepagebyabstractwg.cpp \
    Forms/AlarmInfo/alarminfowg.cpp\
    ControlClass/kdispatch.cpp \
    ControlClass/dispatchcenter.cpp \
    ControlClass/AbstractClass/abstractorder.cpp \
    ControlClass/AbstractClass/rgvorder.cpp \
    Forms/TaskForms/subtask.cpp \
    MointorUI/storeitem.cpp \
    MointorUI/monitorview.cpp \
    MointorUI/monitorui.cpp \
    Forms/Runner/runerform.cpp \
    ControlClass/WarehousingIn/generateinputwarehousingorders.cpp \
    ControlClass/WarehousingIn/getouttrajectory.cpp \
    SimulationgenerationtaskUI/testcreateorderspath.cpp \
    ControlClass/externcommuincation/tcommtcpserver.cpp \
    ControlClass/externcommuincation/tcommtcpclient.cpp \
    ControlClass/externcommuincation/hwcommfactoryob.cpp \
    ControlClass/externcommuincation/tcommtransceivermanager.cpp \
    ControlClass/externcommuincation/tcommmodbustcpclient.cpp\
    ControlClass/Instructions/abstructinstruction.cpp \
    ControlClass/Instructions/carinstruction.cpp \
    ControlClass/Instructions/carelevatorinstruction.cpp \
    Forms/ElevatorForms/elevatorstatus.cpp \
    updaterealtimedataobject.cpp \
    ControlClass/Instructions/runnerinstruction.cpp \
    Forms/Dispatch/dispatchwidget.cpp \
    ControlClass/externcommuincation/tcommserialport.cpp \
    ControlClass/externcommuincation/tcommmodbusserial.cpp \
    ControlClass/Instructions/scancodeinstruction.cpp \
    Forms/mainFroms/mainwidget.cpp \
    Forms/mainFroms/cpiewidget.cpp \
    Forms/mainFroms/torquechart.cpp \
    Forms/mainFroms/warehouseprofileinfoshowwg.cpp \
    Forms/UserInfo/userinfowg.cpp \
    ControlClass/Instructions/httpclien_k.cpp \
    Forms/mainFroms/statusinfowidget.cpp \
    ControlClass/externcommuincation/tcommmcmelsectcpclient.cpp \
    ControlClass/externcommuincation/tcommfx3userialport.cpp \
    ControlClass/WarehousingIn/generatechargingtrajectory.cpp \
    Forms/CarForms/commsetwidget.cpp \
    ControlClass/externcommuincation/UpdateHWData/scanbarcode.cpp \
    ControlClass/externcommuincation/UpdateHWData/updatecardata.cpp \
    ControlClass/externcommuincation/UpdateHWData/updateelevatordata.cpp \
    ControlClass/externcommuincation/UpdateHWData/updaterunnerdata.cpp \
    UnitClass/redis/qReader.cpp \
    UnitClass/redis/qRedis.cpp \
    ControlClass/RunnerRun/excrunneroutcmd.cpp \
    ControlClass/externcommuincation/tcommopcuaclient.cpp \
    ControlClass/externcommuincation/UpdateHWData/updatestackerdata.cpp \
    ControlClass/scancodeinwarehous.cpp \
    UnitClass/operationtask.cpp \
    Forms/StackerWg/stackerwg.cpp \
    Forms/StackerWg/opcuamodel.cpp \
    Forms/StackerWg/treeitem.cpp \
    ControlClass/Instructions/stackerinstruction.cpp \
    Forms/Runner/stackerrunnerwg.cpp \
    ControlClass/stackdispatch.cpp \
    ControlClass/RunnerRun/excstackerrunnercmd.cpp







HEADERS  += mainwindow.h \
    UnitClass/jeson2object.h \
    UnitClass/c_systemlogmng.h \
    UnitClass/logininfowg.h \
    Forms/basefrom.h \
    Forms/CarForms/adddevice.h \
    Forms/CarForms/agvform.h \
    Forms/CarForms/CarStatusFrom.h \
    Forms/CarForms/labeldelegate.h \
    Forms/CarForms/progressbardelegate.h \
    Forms/ElevatorForms/elevatorfrom.h \
    Forms/StorePosInfo/buttondelegate.h \
    Forms/StorePosInfo/storeinfowidget.h \
    Forms/StorePosInfo/storenbrinfotablemodel.h \
    Forms/StorePosInfo/storenbrtableview.h \
    Forms/StorePosInfo/watingdialogwg.h \
    Forms/ElevatorForms/addelevatorform.h \
    Forms/dialogabstractclass.h \
    Forms/CarForms/addcar.h \
    Forms/TaskForms/currenttask.h \
    Forms/TaskForms/alltask.h \
    UnitClass/ImportExportFile/localfileoperate.h \
    Forms/StorePosInfo/editstorenbrinfodialog.h\
    UnitClass/myIniconfig.h \
    UnitClass/logmanager.h \
    Forms/LogForms/logforms.h \
    Forms/TaskForms/labeldelegateV1.h \
    UnitClass/khttpserver.h \
    UnitClass/KCommunication.h \
    ControlClass/basedevice.h \
    Forms/CarForms/carlistform.h \
    ControlClass/observerbase.h \
    ControlClass/selectcar.h \
    Forms/StorePosInfo/pagewg/tablepagebyabstractwg.h\
    UnitClass/TablepageWgClass/spiltpagesbyqsqltablemodel.h\
    Forms/AlarmInfo/alarminfowg.h\
    ControlClass/kdispatch.h \
    ControlClass/basedatainfooperate.h \
    ControlClass/dispatchcenter.h \
    ControlClass/AbstractClass/abstractorder.h \
    ControlClass/AbstractClass/rgvorder.h \
    Forms/TaskForms/subtask.h \
    MointorUI/storeitem.h \
    MointorUI/monitorview.h \
    MointorUI/monitorui.h \
    Forms/Runner/runerform.h \
    ControlClass/WarehousingIn/generateinputwarehousingorders.h \
    ControlClass/WarehousingIn/getouttrajectory.h \
    SimulationgenerationtaskUI/testcreateorderspath.h \
    ControlClass/externcommuincation/hwdeviceabstractinterface.h \
    ControlClass/externcommuincation/tcommtcpserver.h \
    ControlClass/externcommuincation/tcommtcpclient.h \
    ControlClass/externcommuincation/hwcommfactoryob.h \
    ControlClass/externcommuincation/tcommtransceivermanager.h \
    ControlClass/externcommuincation/tcommmodbustcpclient.h \
    ControlClass/Instructions/abstructinstruction.h \
    ControlClass/Instructions/carinstruction.h \
    ControlClass/Instructions/carelevatorinstruction.h \
    Forms/ElevatorForms/elevatorstatus.h \
    updaterealtimedataobject.h \
    ControlClass/Instructions/runnerinstruction.h \
    Forms/Dispatch/dispatchwidget.h \
    ControlClass/externcommuincation/tcommserialport.h \
    ControlClass/externcommuincation/tcommmodbusserial.h \
    ControlClass/Instructions/scancodeinstruction.h \
    Forms/mainFroms/mainwidget.h \
    Forms/mainFroms/cpiewidget.h \
    Forms/mainFroms/torquechart.h \
    Forms/mainFroms/warehouseprofileinfoshowwg.h \
    Forms/UserInfo/userinfowg.h \
    ControlClass/Instructions/httpclien_k.h \
    Forms/mainFroms/statusinfowidget.h \
    ControlClass/externcommuincation/tcommmcmelsectcpclient.h \
    ControlClass/externcommuincation/tcommfx3userialport.h \
    ControlClass/WarehousingIn/generatechargingtrajectory.h \
    ControlClass/externcommuincation/UpdateHWData/scanbarcode.h \
    ControlClass/externcommuincation/UpdateHWData/updatecardata.h \
    ControlClass/externcommuincation/UpdateHWData/updateelevatordata.h \
    ControlClass/externcommuincation/UpdateHWData/updaterunnerdata.h \
    Forms/CarForms/commsetwidget.h \
    UnitClass/redis/qReader.h \
    UnitClass/redis/qRedis.h \
    ControlClass/RunnerRun/excrunneroutcmd.h \
    ControlClass/externcommuincation/tcommopcuaclient.h \
    ControlClass/externcommuincation/UpdateHWData/updatestackerdata.h \
    ControlClass/scancodeinwarehous.h \
    UnitClass/operationtask.h \
    Forms/StackerWg/stackerwg.h \
    Forms/StackerWg/opcuamodel.h \
    Forms/StackerWg/treeitem.h \
    ControlClass/Instructions/stackerinstruction.h \
    Forms/Runner/stackerrunnerwg.h \
    ControlClass/stackdispatch.h \
    ControlClass/RunnerRun/excstackerrunnercmd.h



INCLUDEPATH += ../SDK/include/
include( $$PWD/../SDK/include/library/JQLibrary/JQLibrary.pri )
LIBS += -L../SDK/lib -lCRUDBaseOperation -lRedisBaseModule -lRWRedisInterfaceLib -lReadTableData
LIBS+= -L/usr/local/lib -lhiredis
FORMS    += mainwindow.ui

RESOURCES += \
    resousefile.qrc

DISTFILES += \
    Image/rgvcar.png
