#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap splash(":/robot.png");
     QSplashScreen splashScreen(splash);
    splashScreen.show();
    a.processEvents();
    //QThread::sleep(1);
    MainWindow w;
    w.show();
    splashScreen.finish(&w);
    return a.exec();

}
