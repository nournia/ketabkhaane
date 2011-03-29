#include "sender.h"

#include <syncer.h>
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <connector.h>
#include <QPair>
#include <QDateTime>
#include <QCryptographicHash>

typedef QPair<QString, QString> TablePair;

class Updater
{
    // list of tables which must be updated after key table
    QMap<QString, QList<QPair<QString, QString> > > dependents;

    QSqlQuery qry;

    QMap<QString, QString> conflicted;

    // updates lastId to newId in table even newId exists
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

    // appends one dependency to dependents array
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
    // fills dependents array
    Updater()
    {
        qry = QSqlQuery();

        appendDependent("users", "matches", "designer_id");
        appendDependent("users", "answers", "user_id");
        appendDependent("users", "payments", "user_id");
        appendDependent("users", "open_scores", "user_id");
        appendDependent("users", "supports", "corrector_id");
        appendDependent("users", "permissions", "user_id");
        appendDependent("matches", "questions", "match_id");
        appendDependent("matches", "answers", "match_id");
        appendDependent("matches", "supports", "match_id");
        appendDependent("authors", "resources", "author_id");
        appendDependent("publications", "resources", "publication_id");
        appendDependent("resources", "matches", "resource_id");
    }

    // main function for updating ids from server response
    bool updateClientIds(QString response)
    {
        QSqlDatabase db = Connector::connectDb();
        //qry.exec("pragma foreign_keys = on");
        db.transaction();

        QString tablename;
        QStringList pair;

        foreach (QString table, response.split(",/"))
        if (table.contains('error'))
        {
            qDebug() << table;
            break;
        }
        else if (table.contains(','))
        {
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
        } else if (table.startsWith('+') || table.startsWith('*'))
        {
            QString sTime = table.mid(1);
            QDateTime syncTime = QDateTime::fromString(sTime, "yyyy-MM-dd hh:mm:ss");

            if (syncTime.isValid())
            {
                if (! qry.exec("update library set synced_at = '" + sTime + "'"))
                    qDebug() << qry.lastError();

                qDebug() << syncTime.toString();

                // wait for response data processing
                db.commit();

                return table.startsWith('+'); // continue
            } else
                qDebug() << "error in sync process";
            return false;
        }

        //qry.exec("pragma foreign_keys = off");
    }
};

Sender::Sender(QObject *parent)
    :QObject(parent)
{}

void Sender::send(QUrl url, QMap<QString, QString> & posts)
{
    bool first = true;
    QByteArray postData;
    QMapIterator<QString, QString> i(posts);
    while (i.hasNext()) {
        i.next();
        postData.append((!first ? "&" : "") + i.key() + "=" + i.value());
        first = false;
    }

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
        out << response;
        file.close();
    }

    Updater updater;
    if (updater.updateClientIds(response)) // continue
        sync();
    else
        qDebug() << "sync finished";
}

void Sender::sync()
{
    QSqlQuery qry;
    qry.exec("select id, group_id, license from library");
    qry.next();

    Syncer syncer(this);
    QDateTime syncTime; bool finished;
    QMap<QString, QString> posts;
    posts["id"] = qry.value(0).toString();
    posts["key"] = QCryptographicHash::hash(QString(qry.value(1).toString() + "-" + qry.value(2).toString()).toUtf8(), QCryptographicHash::Sha1).toHex();
    posts["actions"] = syncer.getChunk(syncTime, finished);
    posts["time"] = syncTime.toString("yyyy-MM-dd hh:mm:ss");
    if (finished) posts["finished"] = "true";

    send(QUrl("http://localhost/server.php"), posts);
}
