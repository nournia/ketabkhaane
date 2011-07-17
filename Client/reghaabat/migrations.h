#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include <helper.h>
#include <connector.h>

bool isBetween(QString version, QString min, QString max)
{
    bool ok = (min <= version) && (version < max);
    if (ok)
        qDebug() << QString("migrate %1 to %2").arg(min).arg(max);
    return ok;
}

void migrate(QString newVersion)
{
    QSqlQuery qry, qryTmp;
    bool ok = true;

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
        if (!qry.exec("alter table payments add column payed_at timestamp null"))
        {
            db.rollback();
            return;
        }

        qry.exec("select row_id, created_at from logs where table_name = 'payments'");
        while (qry.next())
            ok &= qryTmp.exec(QString("update payments set payed_at = '%1' where id = %2").arg(qry.value(1).toString()).arg(qry.value(0).toString()));

        ok &= qry.exec("delete from payments where payment = 0");
        ok &= qry.exec("delete from logs where table_name = 'payments'");

        qry.exec("select id, payed_at from payments");
        while (qry.next())
            insertLog("payments", "insert", qry.value(0), master, qry.value(1).toDateTime());
    }

    if (ok && qry.exec(QString("update library set version = '%1'").arg(newVersion)))
    {
        db.commit();
        qDebug() << "migration complete";
    }
    else
        db.rollback();
}

#endif // MIGRATIONS_H
