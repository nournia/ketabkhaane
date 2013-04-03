#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include <QDir>

#include <helper.h>
#include <connector.h>

bool change;
bool isBetween(QString& version, QString min, QString max)
{
    bool ok = (min <= version) && (version < max);
    if (ok) {
        version = max;
        qDebug() << QString("migrate %1 to %2").arg(min).arg(max);
    }
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

    if (isBetween(version, "0.9.5", "0.9.6")) {
        // clean logs
        ok &= qry.exec("delete from logs where table_name='objects' and length(row_data) < 15");
        ok &= qry.exec("delete from logs where table_name in ('users', 'matches', 'branches') and row_data is null");

        // transactions table
        ok &= qry.exec("delete from logs where table_name = 'payments'");

        qry.exec("select id, created_at from transactions where id not in (select row_id from logs where table_name = 'transactions')");
        while (qry.next())
            insertLog("transactions", "insert", qry.value(0), master, qry.value(1).toDateTime());

        insertLog("branches", "delete", 110);
        insertLog("roots", "delete", 1);
    }

    if (isBetween(version, "0.9.6", "0.9.7")) {

        ok &= qry.exec("update library set image='100001.jpg'");

        // that strange id
        ok &= qry.exec("update users set national_id = null where id = 2535");

        // update ids
        QStringList tables = QStringList() << "users" << "authors" << "publications" << "roots" << "branches" << "objects" << "matches" << "files" << "questions";
        for (int i = 0; i < tables.length(); i++) {
            ok &= qry.exec("update "+ tables[i] +" set id = 100000+(id%100000)");
            qDebug() << tables[i] << ok;
        }

        // entities
        ok &= qry.exec("update branches set root_id = 100000+(root_id%100000)");
        ok &= qry.exec("update objects set branch_id = 100000+(branch_id%100000), author_id = 100000+(author_id%100000), publication_id = 100000+(publication_id%100000)");
        ok &= qry.exec("update matches set object_id = 100000+(object_id%100000), designer_id = 100000+(designer_id%100000)");
        ok &= qry.exec("update questions set match_id = 100000+(match_id%100000)");

        // events
        ok &= qry.exec("update answers set user_id = 100000+(user_id%100000), match_id = 100000+(match_id%100000)");
        ok &= qry.exec("update borrows set user_id = 100000+(user_id%100000), object_id = 100000+(object_id%100000)");
        ok &= qry.exec("update open_scores set user_id = 100000+(user_id%100000)");
        ok &= qry.exec("update permissions set user_id = 100000+(user_id%100000)");
        ok &= qry.exec("update supports set match_id = 100000+(match_id%100000), corrector_id = 100000+(corrector_id%100000)");
        ok &= qry.exec("update transactions set user_id = 100000+(user_id%100000)");

        // transaction hints
        ok &= qry.exec("update transactions set description = 'mid:' || (cast(substr(description, 5) as integer)%100000 + 100000) where substr(description, 1, 3) = 'mid'");

        // update filenames
        QString files = dataFolder() + "/files/", newName;
        foreach(QString name, QDir(files).entryList(QDir::Files)) {
            newName = QString("%1.jpg").arg(name.mid(0, name.indexOf('.')).toInt() % 100000 + 100000);
            QFile::rename(files + name, files + newName);
            ok &= qry.exec(QString("update matches set content = replace(content,'\"%1\"', '\"%2\"') where content is not null").arg(name, newName));
        }

        // separate objects table
        ok &= qry.exec("CREATE TABLE belongs (id integer not null primary key autoincrement, object_id integer not null references objects(id) on update cascade, branch_id integer not null references branches(id) on update cascade, label varchar(50) not null, cnt int not null default 0)");
        ok &= qry.exec("insert into belongs (object_id, branch_id, label, cnt) select id, branch_id, label, cnt from objects");

        ok &= qry.exec("ALTER TABLE objects RENAME TO _temp_table");
        ok &= qry.exec("CREATE TABLE objects (id integer not null primary key autoincrement, author_id integer null default null references authors(id) on update cascade, publication_id integer null default null references publications(id) on update cascade, type_id tinyint(4) not null references types(id) on update cascade, title varchar(255) not null)");
        ok &= qry.exec("INSERT INTO objects (id,author_id,publication_id,type_id,title) SELECT id,author_id,publication_id,type_id,title FROM _temp_table");
        ok &= qry.exec("DROP TABLE _temp_table");

        // log update
        ok &= qry.exec("update logs set row_id = 100000+(row_id%100000) where table_name in ('users', 'authors', 'publications', 'roots', 'branches', 'matches', 'files', 'questions')");
        ok &= qry.exec("update logs set user_id = 100000+(user_id%100000)");
        ok &= qry.exec("delete from logs where table_name == 'objects' or table_name == 'resources' or table_name == 'scores'");
        ok &= qry.exec("delete from logs where row_op = 'delete' or row_op = 'update'");
        ok &= qry.exec("delete from logs where table_name = 'questions' and row_id not in (select id from questions)");
        ok &= qry.exec("delete from logs where table_name = 'borrows' and row_id not in (select id from borrows)");

        qDebug() << "logs";
        QString table, id;
        qryTmp2.prepare("update logs set row_data = ? where table_name = ? and row_id = ?");
        qry.exec("select table_name, row_id from logs where row_op = 'insert'");
        for(int i = 0; qry.next(); i++) {
            table = qry.value(0).toString();
            id = qry.value(1).toString();
            qryTmp.exec(QString("select * from %1 where id = %2").arg(table, id));
            if (qryTmp.next()) {
                qryTmp2.addBindValue(getRecordJSON(qryTmp));
                qryTmp2.addBindValue(table);
                qryTmp2.addBindValue(id);
                if (!qryTmp2.exec())
                    qDebug() << "update" << table << id << qryTmp2.lastError();
            } else {
                if (!qryTmp.exec(QString("delete from logs where table_name = '%1' and row_id = %2").arg(table, id)))
                    qDebug() << "delete" << table << id << qryTmp.lastError();
            }

            if (i % 1000 == 0)
                qDebug() << i;
        }

        // log objects and belongs
        qDebug() << "objects";
        qry.exec("select id from objects");
        while (qry.next())
            insertLog("objects", "insert", qry.value(0), master);

        qry.exec("select id from belongs");
        while (qry.next())
            insertLog("belongs", "insert", qry.value(0), master);
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
