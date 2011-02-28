#ifndef REGHAABATSYNC_H
#define REGHAABATSYNC_H

#include <ReghaabatConnect.h>

//#include <parser.h>

#include <QVariant>
#include <QDateTime>
#include <QFile>

// priority of tables
QStringList tables = QStringList() << "users" << "matches" << "questions";

bool setSyncBoundaries(int maxRows, QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    // prepare last sync time
    qry.exec("select sync_time from library");
    if (qry.next() && qry.value(0).toString() != "")
        lastSync = qry.value(0).toDateTime();
    else
        lastSync.setDate(QDate(1000, 01, 01));

    QString sql = "select update_time, sum(cid) from (";
    for (int i = 0; i < tables.size(); i++)
    {
        sql += "select update_time, count(id) as cid from "+ tables[i] +" where update_time > :sync_time group by update_time ";

        if (i != tables.size() - 1)
            sql += "union ";
    }
    sql += ") as t_all group by update_time order by update_time";

    qry.prepare(sql);
    qry.bindValue(":sync_time", lastSync);

    if (! qry.exec())
    {
        qDebug() << qry.lastError();
        return false;
    }

    int rows = 0;
    while (qry.next())
    {
        rows += qry.value(1).toInt();
        if (rows >= maxRows)
        {
            syncTime = qry.value(0).toDateTime();
            return true;
        }
    }
}

// from qjson library
inline QString sanitizeString( QString str )
{
    str.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
    str.replace( QLatin1String( "\"" ), QLatin1String( "\\\"" ) );
    str.replace( QLatin1String( "\b" ), QLatin1String( "\\b" ) );
    str.replace( QLatin1String( "\f" ), QLatin1String( "\\f" ) );
    str.replace( QLatin1String( "\n" ), QLatin1String( "\\n" ) );
    str.replace( QLatin1String( "\r" ), QLatin1String( "\\r" ) );
    str.replace( QLatin1String( "\t" ), QLatin1String( "\\t" ) );
    return QString( QLatin1String( "\"%1\"" ) ).arg( str );
}
inline QString getJsonValue(QVariant v)
{
    if (v.isNull())
        return "null";
    else {
        bool ok; v.toInt(&ok);
        if (ok)
            return v.toString();
        else
            return sanitizeString(v.toString());
    }
}

QString writeJson(QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    QString json = "{";
    foreach (QString table, tables)
    {
        qry.prepare("select * from "+ table +" where update_time > :last_sync and update_time <= :sync_time");
        qry.bindValue(":last_sync", lastSync);
        qry.bindValue(":sync_time", syncTime);
        qry.exec();
        int cols = qry.record().count();

        json += '\"' + table + '\"' + ':' + '[';
        bool firstRow = true, firstCol = true;
        while (qry.next())
        {
            if (! firstRow) json += ','; else firstRow = false;

            json += '[';
            for (int i = 0; i < cols-1; i++)
            {
                if (! firstCol) json += ','; else firstCol = false;
                json += getJsonValue(qry.value(i).toString());
            }
            json += ']';
        }

        json += ']';
    }
    json += '}';
    return json;
}

bool syncDb()
{
    connectDb();
    QDateTime lastSync, syncTime;

    setSyncBoundaries(200, lastSync, syncTime);
    QString json = writeJson(lastSync, syncTime);


    QFile file("tmp.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec( "UTF-8" );
        out << json;
        file.close();
    }

    qDebug() << "finished";

}

#endif // REGHAABATSYNC_H
