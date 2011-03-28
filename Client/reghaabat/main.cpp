#include <QtGui/QApplication>
#include "mainwindow.h"

#include <connector.h>

int main(int argc, char *argv[])
{
    Connector::connectDb();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
