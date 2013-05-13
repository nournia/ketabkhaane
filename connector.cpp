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
        db.setDatabaseName(dataFolder() + "/data.dat");

        if (! db.open())
            qDebug() << "db connection error: " << dataFolder() + "/data.dat" << db.lastError();

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

    // create data folder
    QDir dir(QString("%1/files/").arg(dataFolder()));
    if (!dir.exists())
        dir.mkpath(".");

    return true;
}
