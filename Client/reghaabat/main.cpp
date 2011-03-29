#include <QtGui/QApplication>
#include "mainwindow.h"

#include <connector.h>

int main(int argc, char *argv[])
{
    Connector::connectDb();

    QApplication a(argc, argv);

    a.setStyle("plastique");

    a.setApplicationName("Reghaabat");
    a.setApplicationVersion("0.1");
    a.setOrganizationName("Rooyesh");
    a.setOrganizationDomain("www.Reghaabat.ir");

    MainWindow w;
    w.show();

    return a.exec();
}
