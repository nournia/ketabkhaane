#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

class Connector
{
public:
    static QSqlDatabase connectDb(QString filename = "reghaabat.db");
};

#endif // CONNECTOR_H
