#ifndef SQLMAP_H
#define SQLMAP_H

#include <QSqlQuery>
#include <QSqlRecord>
#include <QMap>
#include <QVariant>

typedef QMap<QString, QVariant> StrMap;
StrMap getRecord(QSqlQuery& query);

#endif // SQLMAP_H