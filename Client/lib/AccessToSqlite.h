#ifndef ACCESSTOSQLITE_H
#define ACCESSTOSQLITE_H

#include <Jalali.h>

#include <QDate>
#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QFile>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>


class AccessToSqlite {
public:
    AccessToSqlite(QString accessFilename, QString sqliteFilename)
    {
        connectAccess(accessFilename);
        connectSqlite(sqliteFilename);
        buildSqliteDb();

        importTable("users", QStringList() << "id" << "firstname" << "lastname" << "birth_date" << "address" << "phone" << "gender" << "created_at" << "description");

        // matches
        if (! accessQry.exec("select id, author, publication from matches"))
            qDebug() << accessQry.lastError();

        while (accessQry.next())
        {
            getTitleId("authors", accessQry.value(1).toString());
            getTitleId("publications", accessQry.value(2).toString());
        }

        sqliteDb.transaction();


        sqliteDb.commit();
        qDebug() << "+ matches";

        qDebug() << "finished";
    }

    QVariant getTitleId(QString table, QString title)
    {
        if (title == "") return NULL;

        QSqlQuery tmp(sqliteDb);
        tmp.exec("select id from " + table + " where title = '"+ title +"'");

        if (tmp.next())
            return tmp.value(0);
        else
        {
            tmp.exec("insert into " + table + " (title) values ('" + title + "')");
            return tmp.lastInsertId();
        }
    }

private:
    QSqlDatabase accessDb, sqliteDb;
    QSqlQuery accessQry, sqliteQry;

    bool connectAccess(QString filename)
    {
        accessDb = QSqlDatabase::addDatabase("QODBC", "AccessDb");
        accessDb.setDatabaseName("Driver={Microsoft Access Driver (*.mdb)};DSN='';DBQ=" + filename);
        accessDb.setPassword("abrdmkazhdpkzsrst");

        if (! accessDb.open())
        {
            qDebug() << accessDb.lastError();
            return false;
        }

        accessQry = QSqlQuery(accessDb);
        return true;
    }

    bool connectSqlite(QString filename)
    {
        sqliteDb = QSqlDatabase::addDatabase("QSQLITE", "SqliteDb");
        sqliteDb.setDatabaseName(filename);

        if (! sqliteDb.open())
        {
            qDebug() << sqliteDb.lastError();
            return false;
        }

        sqliteQry = QSqlQuery (sqliteDb);
        return true;
    }

    bool buildSqliteDb()
    {
        QFile file("../sqlite.sql");
        if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        sqliteDb.transaction();
        foreach (QString table, sqliteDb.tables(QSql::Tables))
        {
            if (! table.startsWith("sqlite_") && ! sqliteQry.exec("drop table if exists " + table + ";"))
                qDebug() << sqliteQry.lastError();
        }

        foreach (QString q, QTextStream(&file).readAll().split(";"))
        {
            //qDebug() << q;
            if (q.contains("create", Qt::CaseInsensitive) || q.contains("insert", Qt::CaseInsensitive))
            if (! sqliteQry.exec(q))
            {
                qDebug() << sqliteQry.lastError();
                return false;
            }
        }
        sqliteDb.commit();

        return true;
    }

    QString getInsertQuery(QString table, QStringList fields)
    {
        QString params = ") VALUES (";

        for (int i = 0; i < fields.size(); i++)
        {
            params = params + ':' + fields[i];
            if (i < fields.size()-1)
                params += ',';
        }

        return "INSERT INTO " + table + " (" + fields.join(",") + params + ")";
    }

    bool importTable(QString table, QStringList fields)
    {
        accessQry.exec("select * from " + table);

        QString qry = getInsertQuery(table, fields);
        if (! sqliteQry.prepare(qry))
            qDebug() << sqliteDb.lastError();

        QVariant tmp;
        sqliteDb.transaction();
        while (accessQry.next())
        {
            for (int i = 0; i < fields.size(); i++)
            {
                if (! accessQry.record().fieldName(i).endsWith("Date"))
                    tmp = accessQry.value(i);
                else
                    tmp = getGregorianVariant(accessQry.value(i).toString());
                sqliteQry.bindValue(i, tmp);
            }

            if (! sqliteQry.exec())
            {
                qDebug() << sqliteDb.lastError();
                return false;
            }
        }
        sqliteDb.commit();

        qDebug() << "+ users";
        return true;
    }
};


#endif // ACCESSTOSQLITE_H
