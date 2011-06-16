#ifndef SQLMAP_H
#define SQLMAP_H

#include <QSqlQuery>
#include <QSqlRecord>
#include <QMap>
#include <QVariant>
#include <QSqlError>
#include <QDateTime>
#include <QComboBox>

typedef QMap<QString, QVariant> StrMap;
typedef QPair<QString, QString> StrPair;

void fillComboBox(QComboBox* combobox, QList<StrPair> data);

QString refineText(QString text);

StrMap getRecord(QSqlQuery& query);
QString getReplaceQuery(QString table, StrMap data, QString id);

QString getAbsoluteAddress(QString address);
QString dataFolder();

#endif // SQLMAP_H
