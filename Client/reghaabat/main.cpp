#include <QtGui/QApplication>
#include <QTranslator>

#include <mainwindow.h>
#include <connector.h>

int main(int argc, char *argv[])
{
    Connector::connectDb();

    QApplication a(argc, argv);

    // application info
    a.setApplicationName(QApplication::tr("Reghaabat"));
    a.setApplicationVersion("0.1");
    a.setOrganizationName(QApplication::tr("Rooyesh"));
    a.setOrganizationDomain("www.reghaabat.ir");

    // translation
    QTranslator rTranslator;
    rTranslator.load("reghaabat_fa");
    a.installTranslator(& rTranslator);

    QTranslator qTranslator;
    qTranslator.load("qt_fa");
    a.installTranslator(& qTranslator);

    // style
    a.setStyle("plastique"); // plastique, cleanlooks
    QFile qss(":/resources/default.qss");
    qss.open(QFile::ReadOnly);
//    a.setStyleSheet(qss.readAll());
    qss.close();

    MainWindow w;
    w.show();

    return a.exec();
}



