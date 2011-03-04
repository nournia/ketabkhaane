#include "connector.h"

#include <QDebug>

QSqlDatabase Connector::connectDb(QString filename)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);

    if (! db.open())
        qDebug() << "reghaabat db connection error : " << db.lastError();

    return db;
}
