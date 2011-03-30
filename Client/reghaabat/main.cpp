#include <QtGui/QApplication>
#include <QTranslator>
#include <QDir>
#include <QDebug>

#include "mainwindow.h"
#include <connector.h>

int main(int argc, char *argv[])
{
    Connector::connectDb();

    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("reghaabat_fa");
    a.installTranslator(& translator);

    a.setStyle("plastique");

    a.setApplicationName(QApplication::tr("Reghaabat"));
    a.setApplicationVersion("0.1");
    a.setOrganizationName(QApplication::tr("Rooyesh"));
    a.setOrganizationDomain("www.reghaabat.ir");

    MainWindow w;
    w.show();

    return a.exec();
}



