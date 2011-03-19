#include "syncer.h"

#include <QFile>

// priority of tables
QStringList tables = QStringList() << /* "users" << "matches" << */ "questions";

bool setSyncBoundaries(int maxRows, QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    // prepare last sync time
    qry.exec("select sync_time from library");
    if (qry.next() && qry.value(0).toString() != "")
        lastSync = qry.value(0).toDateTime();
    else
        lastSync.setDate(QDate(1000, 01, 01));

    QString sql = "select updated_at, sum(cid) from (";
    for (int i = 0; i < tables.size(); i++)
    {
        sql += "select updated_at, count(id) as cid from "+ tables[i] +" where updated_at > :sync_time group by updated_at ";

        if (i != tables.size() - 1)
            sql += "union ";
    }
    sql += ") as t_all group by updated_at order by updated_at";

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
        syncTime = qry.value(0).toDateTime();

        if (rows >= maxRows)
            return true;
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
        bool ok; int i = v.toInt(&ok);
        if (ok)
            return QString::number(i);
        else
            return sanitizeString(v.toString());
    }
}

QString writeJson(QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    QString json = "{";
    bool firstTable = true;
    foreach (QString table, tables)
    {
        qry.prepare("select * from "+ table +" where updated_at > :last_sync and updated_at <= :sync_time");
        qry.bindValue(":last_sync", lastSync);
        qry.bindValue(":sync_time", syncTime);
        qry.exec();
        int cols = qry.record().count();

        if (! firstTable) json += ','; else firstTable = false;
        json += '\"' + table + '\"' + ':' + '[';
        bool firstRow = true;
        while (qry.next())
        {
            if (! firstRow) json += ','; else firstRow = false;

            json += '[';
            bool firstCol = true;
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



    QFile file("tmp.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec( "UTF-8" );
        out << json;
        file.close();
    }


    return json;
}

//Syncer::Syncer(QObject *parent)
//    :QObject(parent)
//{}

QString Syncer::getChunk()
{
    Connector::connectDb();
    QDateTime lastSync, syncTime;

    setSyncBoundaries(200, lastSync, syncTime);

    return writeJson(lastSync, syncTime);
}

void Syncer::syncDb()
{

//    Sender s(this);
//    s.send(QUrl("http://localhost/server.php"), getChunk());

//    qDebug() << "finished";
}
