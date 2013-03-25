#include "syncer.h"

#include <helper.h>

bool setSyncBoundaries(int maxRows, QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    // prepare last sync time
    qry.exec("select synced_at from library");
    if (qry.next() && qry.value(0).toString() != "")
        lastSync = qry.value(0).toDateTime();
    else
        lastSync.setDate(QDate(1900, 01, 01));

    int rows = 0;
    qry.exec("select date(created_at) as ct, count(row_id) from logs where ct > '" + formatDateTime(lastSync) + "' group by ct order by ct");
    while (qry.next()) {
        rows += qry.value(1).toInt();
        syncTime = qry.value(0).toDateTime();

        if (rows >= maxRows)
            return false;
    }

    // unsynced rows are less than limit
    syncTime = QDateTime::currentDateTime();
    return true;
}

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{}

bool Syncer::getRecordsAndFiles(QDateTime& syncTime, QStringList& logs, QStringList& files)
{
    QSqlQuery qry;
    QDateTime lastSync;
    bool finished = setSyncBoundaries(5000, lastSync, syncTime);
    QString condition = QString("created_at > '%1' and created_at <= '%2'").arg(formatDateTime(lastSync), formatDateTime(syncTime));

    qry.exec("select table_name, row_op, row_id, user_id, created_at, row_data from logs where "+ condition);
    while (qry.next())
        logs.append(QString("%1, %2, %3, %4, %5|%6").arg(qry.value(0).toString(), qry.value(1).toString(), qry.value(2).toString(), qry.value(3).toString(), qry.value(4).toString(), qry.value(5).toString()));

    // extract new filenames
    qry.exec("select row_id, row_data from logs where table_name = 'files' and "+ condition);
    while (qry.next())
        files.append(QString("%1/files/").arg(dataFolder()) + qry.value(0).toString() +"."+ qry.value(1).toString().mid(1,3));

    return finished;
}
