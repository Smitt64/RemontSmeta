#include "mainwindow.h"
#include "globals.h"
#include "exceptionbase.h"
#include "loggingcategories.h"
#include <QApplication>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen splash(QPixmap("://images/splash.jpg"));
    splash.show();

    MainWindow w;
    try
    {
        Globals::inst()->init(&splash);
        w.show();
    }
    catch (ExceptionBase &e)
    {
        QMessageBox::critical(&splash, QObject::tr("Исключение"), e.what());
    }
    splash.finish(&w);
    return a.exec();
}
