 #ifndef MAIN
#define MAIN
#endif

#include "mainwindow.h"
#include "main.h"
#include "settings.h"
#include <QApplication>
#include <string>
#include <iostream>
#include <QStyleFactory>

using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed( false );
    app = &a;
    MainWindow w;
    mainwin = &w;
    w.setFixedSize(w.geometry().width(),w.geometry().height());
    w.show();
    QApplication::setQuitOnLastWindowClosed( true );

    return QApplication::exec();
}
