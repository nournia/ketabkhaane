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
    QSqlQuery qry, qryTmp, qryTmp2;
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

    if (isBetween(version, "0.9.0", "0.9.1")) {
        ok &= qry.exec("select min(id) as mid, user_id, object_id, delivered_at, count(id) as cnt from borrows group by user_id, object_id, delivered_at order by cnt desc");
        while (qry.next()) {
            if (qry.value(4).toInt() <= 1)
                break;
            qDebug() << qry.value(4).toString();

            ok &= qryTmp.exec(QString("select id from borrows where id != %1 and user_id = %2 and object_id = %3").arg(qry.value(0).toString(), qry.value(1).toString(), qry.value(2).toString()));
            while (qryTmp.next()) {
                ok &= qryTmp2.exec(QString("delete from borrows where id = %1").arg(qryTmp.value(0).toString()));
                insertLog("borrows", "delete", qryTmp.value(0), master);
            }
        }

        ok &= qry.exec("update branches set id = 138, root_id = 4, label = 138 where id = 110");
        ok &= qry.exec("delete from roots where id = 1");
        ok &= qry.exec("update objects set branch_id = 138 where branch_id = 110");
        ok &= qry.exec("update objects set label = replace(label, '110', '138') where branch_id = 138");
        ok &= qry.exec("delete from borrows where object_id = 1838");
        ok &= qry.exec("delete from objects where branch_id < 111");
        ok &= qry.exec("delete from objects where id = 677");
    }

    if (isBetween(version, "0.9.1", "0.9.5")) {
        // remove library extra fields
        qry.exec("select title from library");
        qry.next();
        QString title = qry.value(0).toString();

        ok &= qry.exec("drop table library");
        ok &= qry.exec("create table library (id integer null default null, title varchar(255) not null, description varchar(1000) null default null, started_at datetime not null, image varchar(50) null default null, version varchar(10) null, synced_at timestamp null default null, license varchar(255) null default null, options text null)");
        ok &= qry.exec("insert into library (id, title, image, started_at, version) values (1, '"+ title +"', '1.jpg', '2012-01-01', '0.9.1')");
        ok &= qry.exec("delete from logs where table_name = 'library'");
        insertLog("library", "insert", 1);

        // fix log density
        QStringList seconds = QStringList() << "43" << "44" << "45" << "46" << "47" << "48";

        for (int i = 0; i < 6; i++)
            ok &= qry.exec("update logs set created_at = '2011-07-02 16:08:"+ seconds[i] +"' where table_name='questions' and row_id in (select row_id from logs where table_name = 'questions' and created_at >= '2011-07-02 16:08:41' and created_at <= '2011-07-02 16:08:42' limit 1000) and created_at >= '2011-07-02 16:08:41' and created_at <= '2011-07-02 16:08:42'" );
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
