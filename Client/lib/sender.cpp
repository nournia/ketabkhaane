#include "sender.h"

#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <connector.h>
#include <QPair>

typedef QPair<QString, QString> TablePair;

class Updater
{
    // list of tables which must be updated after key table
    QMap<QString, QList<QPair<QString, QString> > > dependants;

    void updateId(QString& table, QString& lastId, QString& newId)
    {
        static QSqlQuery qry;

        if (! qry.exec("update " + table + " set id = " + newId + " where id = " + lastId))
            qDebug() << "update " + table + " set id = " + newId + " where id = " + lastId << qry.lastError();

//        foreach (TablePair dependant, dependants[table])
//            qry.exec("update "+ dependant.first +" set "+ dependant.second +" = " + newId + " where "+ dependant.second +" = " + lastId);
    }

    void appendDependat(QString table, QString dependant, QString column)
    {
        TablePair tmp; tmp.first = dependant; tmp.second = column;

        if (! dependants.contains(table))
        {
            QList<QPair<QString, QString> > list;
            list.append(tmp);
            dependants[table] = list;
        } else
            dependants[table].append(tmp);
    }

public:
    Updater()
    {
//        appendDependat("users", "answers", "user_id");
    }

    void updateClientIds(QString response)
    {
        QSqlDatabase db = Connector::connectDb();
        QSqlQuery qry;

        qry.exec("pragma foreign_keys = on");

        QString tablename;
        QStringList pair;

        foreach (QString table, response.split(",/"))
        if (table.contains('-'))
        {
            db.transaction();

            bool first = true;
            foreach (QString line, table.split(','))
            {
                if (first)
                {
                    tablename = line;
                    first = false;
                } else
                {
                    pair = line.split('-'); // lastId, newId
                    updateId(tablename, pair[0], pair[1]);
                }
            }

            db.commit();
            qDebug() << "+ " << tablename;
        }

        qry.exec("pragma foreign_keys = off");
    }
};

//Sender::Sender(QObject *parent)
//    :QObject(parent)
//{}

void Sender::send(QUrl url, QString& data)
{
    QSqlQuery qry;
    qry.exec("select id, group_id, license from library");
    qry.next();

    QByteArray postData;
    postData.append("id=" + qry.value(0).toString() + "&");
    postData.append("key=" + qry.value(1).toString() + "-" + qry.value(2).toString() + "&");
    postData.append("create=" + data.toUtf8());

//    reply = qnam.get(QNetworkRequest(url));
    reply = qnam.post(QNetworkRequest(url), postData);

    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
}


void Sender::httpFinished()
{
    QString response = reply->readAll();

    QFile file("tmp.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        //out.setCodec( "UTF-8" );
        out << response;
        file.close();
    }

    Updater updater;
    updater.updateClientIds(response);

    qDebug() << "sync finished";
}
