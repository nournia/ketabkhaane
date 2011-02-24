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
        QSqlDatabase accessDb, sqliteDb;

        accessDb = connectAccess(accessFilename);
        sqliteDb = connectSqlite(sqliteFilename);

        buildSqliteDb(sqliteDb);

        //        QStringList list = sqliteDb.tables(QSql::Tables);
        //        qDebug() << list.size();
        //        for(int i=0; i < list.size(); i++)
        //        {
        //            qDebug() << "Table names " << list.at(i) << endl;
        //        }

        QSqlQuery accessQry(accessDb), sqliteQry(sqliteDb);

        accessQry.exec("select * from users");

        QString fields[] = {"firstname", "lastname", "birth_date", "address", "phone", "gender", "created_at", "description", NULL};
        QString qry = getInsertQuery("users", fields);
        if (! sqliteQry.prepare(qry))
            qDebug() << sqliteDb.lastError();

        sqliteDb.transaction();
        while (accessQry.next())
        {
            sqliteQry.bindValue(":firstname", accessQry.value(1));
            sqliteQry.bindValue(":lastname", accessQry.value(2));
            sqliteQry.bindValue(":birth_date", getGregorianVariant(accessQry.value(3).toString()));
            sqliteQry.bindValue(":address", accessQry.value(4));
            sqliteQry.bindValue(":phone", accessQry.value(5));
            sqliteQry.bindValue(":gender", accessQry.value(6));
            sqliteQry.bindValue(":created_at", getGregorianVariant(accessQry.value(7).toString()));
            sqliteQry.bindValue(":description", accessQry.value(8));

            if (! sqliteQry.exec())
            {
                qDebug() << sqliteDb.lastError();
                break;
            }
        }
        sqliteDb.commit();

        qDebug() << "finish";
    }

private:

    QSqlDatabase& connectAccess(QString filename)
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "AccessDb");
        db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb)};DSN='';DBQ=" + filename);
        db.setPassword("abrdmkazhdpkzsrst");

        if (! db.open())
            qDebug() << db.lastError();

        return db;
    }

    QSqlDatabase& connectSqlite(QString filename)
    {
        QFile(filename).remove();

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "SqliteDb");
        db.setDatabaseName(filename);

        if (! db.open())
            qDebug() << db.lastError();

        return db;
    }

    bool buildSqliteDb(QSqlDatabase& db)
    {
        QSqlQuery query(db);
        QFile file("../sqlite.sql");
        if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        query.exec(QTextStream(&file).readAll());
        return true;
    }

    QString& getInsertQuery(char* table, QString* fields)
    {
        static QString qry = "INSERT INTO ";
        qry = qry + table + " (";
        QString params = ") VALUES (";

        for (int i = 0; fields[i] != NULL; i++)
        {
            qry += fields[i];
            params = params + ':' + fields[i];
            if (fields[i+1] != NULL)
            {
                qry += ',';
                params += ',';
            }
        }

        qry = qry + params + ")";
        return qry;
    }


    QVariant getGregorianVariant(QString jalali)
    {
        QString str = toGregorian(jalali);

        if (str != "")
            return str;
        else
            return NULL;
    }
};


#endif // ACCESSTOSQLITE_H
