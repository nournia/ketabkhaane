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

        qry.exec("update " + table + " set id = " + newId + " where id = " + lastId);

        foreach (TablePair dependant, dependants[table])
            qry.exec("update "+ dependant.first +" set "+ dependant.second +" = " + newId + " where "+ dependant.second +" = " + lastId);
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
        appendDependat("users", "answers", "user_id");
        appendDependat("users", "payments", "user_id");
        appendDependat("users", "open_scores", "user_id");
        appendDependat("users", "matches", "designer_id");
        appendDependat("users", "supports", "corrector_id");
    }

    void updateClientIds(QString response)
    {
        QSqlDatabase db = Connector::connectDb();
        db.transaction();

        QString tablename;
        QStringList pair;
        bool first = true;

        foreach (QString table, response.split("-/"))
        if (table.contains('-'))
            foreach (QString line, table.split('-'))
            {
                if (first)
                {
                    tablename = line;
                    first = false;
                } else
                {
                    pair = line.split(','); // lastId, newId
                    updateId(tablename, pair[0], pair[1]);
                }
            }
        db.commit();
    }
};

//Sender::Sender(QObject *parent)
//    :QObject(parent)
//{}

void Sender::send(QUrl url, QString& data)
{
    QByteArray postData;
    postData.append("create=" + data.toUtf8()); // postData.append("param1=alireza&");  postData.append("param2=me");

//    reply = qnam.get(QNetworkRequest(url));
    reply = qnam.post(QNetworkRequest(url), postData);

    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
}


void Sender::httpFinished()
{
    QString response = reply->readAll();

    Updater updater;
    updater.updateClientIds(response);

    /*
    QFile file("tmp.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        //out.setCodec( "UTF-8" );
        out << response;
        file.close();
    }
    */

    qDebug() << "sync finished";
}
