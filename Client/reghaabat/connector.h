#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

class Connector
{
public:
    static QSqlDatabase connectDb();
    static QSqlDatabase connectLibrary(bool& ok);
};

#endif // CONNECTOR_H
