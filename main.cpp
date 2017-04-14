#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include "syntaxcheck.h"
int main(int argc, char *argv[])
{
//    SyntaxCheck check;
//    check.checkFile(":/rc/foo2.txt");
//    return 0;
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
