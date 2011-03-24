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
    QMap<QString, QList<QPair<QString, QString> > > dependents;

    QSqlQuery qry;

    QMap<QString, QString> conflicted;

    void updateId(QString& table, QString& lastId, QString& newId)
    {
        if (conflicted.contains(lastId))
            lastId =  conflicted[lastId];

        if (qry.exec("update " + table + " set id = " + newId + " where id = " + lastId))
        {
            // two times faster than foreign keys
            foreach (TablePair dependant, dependents[table])
                qry.exec("update "+ dependant.first +" set "+ dependant.second +" = " + newId + " where "+ dependant.second +" = " + lastId);
        } else
        {
            // handle primary key conflict
            if (qry.lastError().text().startsWith("PRIMARY KEY"))
            {
                // get first valid id
                qry.exec("select max(id)+1 from " + table);
                qry.next();
                QString validId = qry.value(0).toString();

                // update newId to first vaild id
                updateId(table, newId, validId);
                conflicted[newId] = validId;

                // update lastId to newId
                updateId(table, lastId, newId);
            }
            else qDebug() << qry.lastError();
        }
    }

    void appendDependent(QString table, QString dependant, QString column)
    {
        TablePair tmp; tmp.first = dependant; tmp.second = column;

        if (! dependents.contains(table))
        {
            QList<QPair<QString, QString> > list;
            list.append(tmp);
            dependents[table] = list;
        } else
            dependents[table].append(tmp);
    }

public:
    Updater()
    {
        qry = QSqlQuery();

        appendDependent("users", "matches", "designer_id");
        appendDependent("users", "answers", "user_id");
        appendDependent("users", "payments", "user_id");
        appendDependent("users", "open_scores", "user_id");
        appendDependent("users", "supports", "corrector_id");
        appendDependent("matches", "questions", "match_id");
        appendDependent("matches", "answers", "match_id");
        appendDependent("matches", "supports", "match_id");
        appendDependent("authors", "resources", "author_id");
        appendDependent("publications", "resources", "publication_id");
        appendDependent("resources", "matches", "resource_id");
    }

    void updateClientIds(QString response)
    {
        QSqlDatabase db = Connector::connectDb();

        //qry.exec("pragma foreign_keys = on");

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
