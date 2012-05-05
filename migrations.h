#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include <helper.h>
#include <connector.h>

bool change;
bool isBetween(QString version, QString min, QString max)
{
    bool ok = (min <= version) && (version < max);
    if (ok)
        qDebug() << QString("migrate %1 to %2").arg(min).arg(max);
    change |= ok;
    return ok;
}

void migrate(QString newVersion)
{
    QSqlQuery qry, qryTmp;
    bool ok = true;
    change = false;

    // retreive application version
    QString version;
    qry.exec("select version from library");
    if (qry.next())
        version = qry.value(0).toString();
    if (version.isEmpty())
        version = "0.0.0";

    QString master = "1";
    qry.exec("select user_id from permissions where permission = 'master'");
    if (qry.next())
        master = qry.value(0).toString();

    QSqlDatabase db = Connector::connectDb();
    db.transaction();

    if (isBetween(version, "0.8.2", "0.8.5"))
    {
        // paymetns
        ok &= qry.exec("alter table payments add column payed_at timestamp null");

        qry.exec("select row_id, created_at from logs where table_name = 'payments'");
        while (qry.next())
            ok &= qryTmp.exec(QString("update payments set payed_at = '%1' where id = %2").arg(qry.value(1).toString()).arg(qry.value(0).toString()));

        ok &= qry.exec("delete from payments where payment = 0");
        ok &= qry.exec("delete from logs where table_name = 'payments'");

        qry.exec("select id, payed_at from payments");
        while (qry.next())
            insertLog("payments", "insert", qry.value(0), master, qry.value(1).toDateTime());

        // matches
        ok &= qry.exec("update matches set content = replace(content, 'img width=\"100%\"', 'img style=\"width: 858px; height: 650px; \"')");
        qry.exec("select id from matches where content is not null");
        while (qry.next())
            insertLog("matches", "update", qry.value(0), master);
    }

    if (change && ok)
        ok &= qry.exec(QString("update library set version = '%1'").arg(newVersion));

    qry.clear();
    qryTmp.clear();

    if (change && ok)
    {
        insertLog("library", "update", "1");
        if (db.commit())
            qDebug() << "migration complete";
    }
    else
        if (! db.rollback())
            qDebug() << db.lastError();
}

#endif // MIGRATIONS_H
