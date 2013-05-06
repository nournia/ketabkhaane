#include "receiver.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

Receiver::Receiver(QObject *parent) :
    QObject(parent)
{
}

void Receiver::queueUrl(QString args, bool file)
{
    QString url = App::instance()->serverUrl;
    if (file)
        url += "files.php?q=";
    else
        url += "data.php?i="+ App::instance()->libraryId +"&";
    queue.append(url + args);
}

void Receiver::popUrl()
{
    if (queue.length() > 0) {
        QNetworkRequest request(QUrl(queue.takeFirst()));
        reply = qnam.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(received()));
    }
}

void Receiver::get(QString args)
{
    this->queueUrl(args);
    this->popUrl();
}

void Receiver::received()
{
    QString url = reply->url().toString();
    QString fileId = "files.php?q=";

    // save file
    if (url.indexOf(fileId) > 0) {
        QString filename = url.mid(url.indexOf(fileId)+fileId.length());
        QFile file(QString("%1/files/%2").arg(dataFolder()).arg(filename));
        if (file.open(QIODevice::WriteOnly))
            file.write(reply->readAll());

        popUrl();
        return;
    }

    // read data
    QString data = reply->readAll();
    emit received(QJsonDocument::fromJson(data.toUtf8()).object().toVariantMap());
}

void Receiver::storeRows(QString table, QVariant rows)
{
    QSqlQuery qry;
    QStringList marks;
    QVariantList fields;

    foreach(QVariant row, rows.toList()) {
        fields = row.toList();
        marks.clear();
        for(int i = 0; i < fields.length(); i++)
            marks.append("?");

        qry.prepare(QString("insert into %1 values (%2)").arg(table, marks.join(",")));

        foreach(QVariant field, fields) {
            if (field.isNull())
                qry.addBindValue(field);
            else
                qry.addBindValue(field.toString());
        }

        if (qry.exec()) {
            // supports and belongs table
            if (table == "matches") {
                if (qry.exec(QString("insert into supports (match_id, corrector_id, current_state, score) values (%1, %2, 'imported', 0)").arg(fields[0].toString(), App::instance()->userId)))
                    insertLog("supports", "insert", qry.lastInsertId());
            } else if (table == "objects") {
                if (qry.exec(QString("insert into belongs (object_id) values (%1)").arg(fields[0].toString())))
                    insertLog("belongs", "insert", qry.lastInsertId());
            }
        }
    }
}

