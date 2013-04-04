#include "connector.h"

#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QDir>

#include "helper.h"

QSqlDatabase Connector::connectDb()
{
    if (! QSqlDatabase::contains())
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dataFolder() + "/reghaabat.dat");

        if (! db.open())
            qDebug() << "reghaabat db connection error : " << db.lastError();

        if (!db.tables().length())
            buildDb();

        return db;
    } else
        return QSqlDatabase::database();
}

bool Connector::buildDb()
{
    QSqlQuery qry;

    QFile file(":/resources/client.sql");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream input(&file);
    input.setCodec("UTF-8");

    foreach (QString q, input.readAll().split(";"))
    if (q.contains("create", Qt::CaseInsensitive) || q.contains("insert", Qt::CaseInsensitive) || q.contains("update", Qt::CaseInsensitive))
        if (! qry.exec(q)) {
            qDebug() << q;
            qDebug() << "sql file error: " << qry.lastError();
            return false;
        }
    return true;
}

QSqlDatabase Connector::connectLibrary(bool& ok)
{
    QSqlDatabase db;
    ok = false;

    QSettings settings("Sobhe", "Reghaabat");
    QString library = settings.value("LibraryAddress", "").toString();

    if (library.isEmpty() || ! QFile::exists(library))
        return db;

    if (! QSqlDatabase::contains("Library"))
    {
        db = QSqlDatabase::addDatabase("QODBC", "Library");
        db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb)};DSN='';DBQ=" + library);
        db.setPassword("abrdmkazhdpkzsrst");

        if (! db.open())
            qDebug() << "library db connection error : " << db.lastError();
        else
            ok = true;
    } else {
        db = QSqlDatabase::database("Library");
        ok = true;
    }

    return db;
}
