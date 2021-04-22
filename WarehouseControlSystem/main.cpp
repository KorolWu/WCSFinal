#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QSplashScreen>
#include <QThread>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
//    QCoreApplication::addLibraryPath("./sdk");
    //加载并显示启动画面
    QSplashScreen splash(QPixmap(":/resouse/Image/init.png"));
    splash.setDisabled(true); //禁用用户的输入事件响应
    splash.show();

    QEventLoop loop;
    QTimer::singleShot(100, &loop, SLOT(quit()));
    loop.exec();

    splash.setCursor(Qt::BlankCursor);
    QFont ft("Times", 12);
    splash.setFont(ft);
    splash.showMessage(QObject::tr("请稍后    ....\r\n 配置文件读取中....\r\n Mysql链接中....\r\n Redis数据读取中...."), Qt::AlignCenter, Qt::black);

    MainWindow w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
