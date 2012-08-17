#include "syncer.h"

#include <helper.h>
#include <sender.h>

#include <QFile>
#include <QDateTime>
#include <QCryptographicHash>

// http://reghaabat.ap01.aws.af.cm/backend.php
QUrl backendUrl("http://localhost/reghaabat-server/backend.php");

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{}

void Syncer::exec()
{
    QSqlQuery qry;
    qry.exec("select license from library");
    qry.next();

    // Register
    if (qry.value(0).toString().isEmpty())
        registerDb();
    else
        sendLogs();
}

void Syncer::registerDb()
{
    QMap<QString, QString> data;
    data["command"] = "register";

    Sender* sender = new Sender(this);
    sender->post(backendUrl, data);
    connect(sender, SIGNAL(received(QString)), this, SLOT(registerFinished(QString)));
    qDebug() << "register request sent.";
}

void Syncer::registerFinished(QString response)
{
    QStringList values = response.split(" - ");
    if (values.length() > 0 && values[0] == "ok") {
        QSqlQuery qry;
        qry.exec(QString("update library set id = %1, license = \"%2\", synced_at = null").arg(values[1], values[2]));
        insertLog("library", "update", values[1]);
        qDebug() << "library registered.";
    } else
        qDebug() << response;
}

QByteArray getLogsData(QDateTime& syncTime, int& count, bool& finished, QStringList& files);

void Syncer::sendLogs()
{
    QSqlQuery qry;
    qry.exec("select id, license from library");
    qry.next();
    QString id = qry.value(0).toString(), license = qry.value(1).toString();

    QMap<QString, QString> data;
    data["command"] = "receive";
    data["id"] = id;
    data["key"] = QCryptographicHash::hash(QString(id +"|x|"+ license).toUtf8(), QCryptographicHash::Sha1).toHex();

    QDateTime syncTime; QStringList files; bool finished; int count;
    data["logs"] = getLogsData(syncTime, count, finished, files);
    data["count"] = QString("%1").arg(count);
    data["finished"] = QString("%1").arg(finished);
    data["synced_at"] = formatDateTime(syncTime);

    Sender* sender = new Sender(this);
    sender->post(backendUrl, data);
    connect(sender, SIGNAL(received(QString)), this, SLOT(sendFinished(QString)));
    qDebug() << "data sent.";
}

void Syncer::sendFinished(QString response)
{
    QStringList values = response.split(" - ");
    if (values.length() > 0 && values[0] == "ok") {
        QSqlQuery qry;
        qry.exec(QString("update library set synced_at = \"%1\"").arg(values[1]));
        qDebug() << "data received.";
    } else
        qDebug() << response;
}

QByteArray getLogsData(QDateTime& syncTime, int& count, bool& finished, QStringList& files)
{
    QDateTime lastSync;
    //finished = setSyncBoundaries(1000000, lastSync, syncTime);
    syncTime = QDateTime::currentDateTime();
    lastSync.setDate(QDate(2012, 05, 01));
    finished = true;

    QString logs, row, delimeter = "|-|";
    QSqlQuery qry;
    QString condition = QString(" where created_at > \"%1\" and created_at <= \"%2\"").arg(formatDateTime(lastSync), formatDateTime(syncTime));
    qry.exec("select table_name, row_op, row_id, user_id, created_at, row_data from logs"+ condition);

    bool first = true; count = 0;
    for (int i; qry.next(); count++) {
        if (!first) logs += delimeter; else first = false;

        row = "";
        for (i = 0; i < 5; i++)
            row += qry.value(i).toString() +",";
        row += "|"+ qry.value(i).toString();
        logs += row.replace(delimeter, "");
    }

    // extract new filenames
//    qry.exec("select id ||'.'|| extension from files where updated_at > "+ dtLast +" and updated_at <= "+ dtSync);
//    while (qry.next())
//        files.append(QString("%1/files/").arg(dataFolder()) + qry.value(0).toString());

    return logs.toUtf8();
}
