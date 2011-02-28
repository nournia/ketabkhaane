#ifndef REGHAABATCONNECT_H
#define REGHAABATCONNECT_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

QSqlDatabase connectDb(QString filename = "reghaabat.db")
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);

    if (! db.open())
        qDebug() << "reghaabat db connection error : " << db.lastError();

    return db;
}


#endif // REGHAABATCONNECT_H
