#include "syncer.h"

#include <mainwindow.h>

#include <QFile>

// reverse priority of tables
QStringList tables = QStringList() << "scores" << "permissions" << "answers" << "questions" << "payments" << "supports" << "open_scores" << "matches" << "resources" << "files" << "authors" << "publications" << "users";

bool setSyncBoundaries(int maxRows, QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    // prepare last sync time
    qry.exec("select synced_at from library");
    if (qry.next() && qry.value(0).toString() != "")
        lastSync = qry.value(0).toDateTime();
    else
        lastSync.setDate(QDate(1900, 01, 01));

    QString sql = "select updated_at, sum(cid) from (";
    for (int i = 0; i < tables.size(); i++)
    {
        sql += "select updated_at, count(id) as cid from "+ tables[i] + " where updated_at > '" + lastSync.toString("yyyy-MM-dd hh:mm:ss") + "' group by updated_at ";

        if (i != tables.size() - 1)
            sql += "union ";
    }
    sql += ") as t_all group by updated_at order by updated_at";

    if (! qry.exec(sql))
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
            return false;
    }

    // unsynced rows are less than limit
    syncTime = QDateTime::currentDateTime();
    return true;
}

// from qjson library
inline QString sanitizeString( QString str )
{
    str.replace('&', ','); // must change !!!
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

QString writeJson(QDateTime& lastSync, QDateTime& syncTime, QStringList& files)
{
    QSqlQuery qry;

    QString dtLast = "'" + lastSync.toString("yyyy-MM-dd hh:mm:ss") + "'", dtSync = "'" + syncTime.toString("yyyy-MM-dd hh:mm:ss") + "'";

    QString json = "{";
    for (int k = 0; k < 2; k++)
    {
        if (k == 0)
            json += "\"insert\":{"; // first key
        else
            json += ",\"replace\":{"; // second key

        bool firstTable = true;
        foreach (QString table, tables)
        {
            if (k == 0)
                qry.exec("select * from "+ table +" where created_at > "+ dtLast +" and created_at <= "+ dtSync);
            else
                qry.exec("select * from "+ table +" where updated_at > "+ dtLast +" and updated_at <= "+ dtSync +" and created_at < "+ dtLast);

            if (! qry.next()) continue;

            if (! firstTable) json += ','; else firstTable = false;
            json += '\"' + table + '\"' + ':' + '[';
            int cols = qry.record().count();
            bool firstRow = true;

            do {
                if (! firstRow) json += ','; else firstRow = false;

                json += '[';
                bool firstCol = true;
                for (int i = 0; i < cols-1; i++)
                {
                    if (! firstCol) json += ','; else firstCol = false;
                    json += getJsonValue(qry.value(i).toString());
                }
                json += ']'; // end of row
            } while (qry.next());

            json += ']'; // end of table
        }
        json += '}'; // end of insert or update
    }
    json += '}'; // end of file


    // extract new filenames
    qry.exec("select id ||'.'|| extension from files where updated_at > "+ dtLast +" and updated_at <= "+ dtSync);
    while (qry.next())
        files.append(Reghaabat::instance()->files + qry.value(0).toString());


//    QFile file("tmp.json");
//    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        QTextStream out(&file);
//        out.setCodec( "UTF-8" );
//        out << json;
//        file.close();
//    }

    return json;
}

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{}

QByteArray Syncer::getChunk(QDateTime& syncTime, bool& finished, QStringList& files)
{
    QDateTime lastSync;

    finished = setSyncBoundaries(1000000, lastSync, syncTime);

    return writeJson(lastSync, syncTime, files).toUtf8();
}

void Syncer::syncDb()
{
//    Sender s(this);
//    s.send(QUrl("http://localhost/server.php"), getChunk());

//    qDebug() << "finished";
}
