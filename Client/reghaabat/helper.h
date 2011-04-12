#ifndef SQLMAP_H
#define SQLMAP_H

#include <QSqlQuery>
#include <QSqlRecord>
#include <QMap>
#include <QVariant>
#include <QSqlError>
#include <QDateTime>

typedef QMap<QString, QVariant> StrMap;
typedef QPair<QString, QString> StrPair;

QString refineText(QString text);

StrMap getRecord(QSqlQuery& query);

QString getReplaceQuery(QString table, StrMap data, QString id);

#endif // SQLMAP_H
