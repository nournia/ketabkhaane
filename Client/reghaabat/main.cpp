#include <QtGui/QApplication>
#include <QTranslator>
#include <QFile>
#include <QSettings>

#include <mainwindow.h>
#include <connector.h>
#include <migrations.h>

/** parameters
*   reghaabat.exe -data $FOLDER
*       if $FOLDER != "" set data folder to $FOLDER else $APPFOLDER/data
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion("0.8.5");

    // arguments
    if (argc == 3 && !strcmp(argv[1], "-data"))
    {
        QSettings settings("Rooyesh", "Reghaabat");

        QString filename = argv[2];
        if (filename.isEmpty())
            settings.setValue("DataFolder", getAbsoluteAddress("data"));
        else
            settings.setValue("DataFolder", filename);
    }

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
    a.setStyleSheet(qss.readAll());
    qss.close();

    // db connection
    Connector::connectDb();
    migrate(a.applicationVersion());

    // execute
    MainWindow w;
    w.show();

    return a.exec();
}



