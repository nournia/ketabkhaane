#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QSettings>

#include <mainwindow.h>
#include <connector.h>
#include <migrations.h>
#include <accesstosqlite.h>

/** parameters
*   ketabkhaane.exe --data $FOLDER
*       if $FOLDER != "" set data folder to $FOLDER else $APPFOLDER/data
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion("0.9.8");
    App::instance()->settings = new QSettings("Sobhe", "Ketabkhaane");

    // arguments
    if (argc >= 2) {
        QString command = argv[1];

        if (command == "--data") {
            QString folder;
            if (argc == 3)
                folder = argv[2];
            if (folder.isEmpty())
                folder = getAbsoluteAddress("data");

            App::instance()->settings->setValue("DataFolder", folder.replace("\\", "/"));
            exit(0);
        }
    }

    // set default folder
    if (App::instance()->settings->value("DataFolder").toString().isEmpty())
        App::instance()->settings->setValue("DataFolder", getAbsoluteAddress("data"));

    // translation
    QTranslator rTranslator;
    rTranslator.load("application_fa");
    a.installTranslator(& rTranslator);

    QTranslator qTranslator;
    qTranslator.load("qt_fa");
    a.installTranslator(& qTranslator);

    // style
     a.setStyle("fusion");
    QFile qss(":/resources/default.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();

    // db connection
    if (Connector::connectDb().isOpen())
        migrate(a.applicationVersion());

    // init cache db
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "cache");
    db.setDatabaseName(":memory:");
    if (! db.open()) qDebug() << "cache db error : " << db.lastError();

    // execute
    MainWindow w;
    w.show();

    return a.exec();
}
