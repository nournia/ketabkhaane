#include "syncer.h"

#include <helper.h>

#include <QFileInfo>
#include <QHttpMultiPart>
#include <QCryptographicHash>

bool setSyncBoundaries(int maxRows, QDateTime &lastSync, QDateTime &syncTime)
{
    QSqlQuery qry;

    // prepare last sync time
    qry.exec("select synced_at from library");
    if (qry.next() && qry.value(0).toString() != "")
        lastSync = qry.value(0).toDateTime();
    else
        lastSync.setDate(QDate(1900, 01, 01));

    int rows = 0;
    qry.exec("select date(created_at) as ct, count(row_id) from logs where ct > '" + formatDateTime(lastSync) + "' group by ct order by ct");
    while (qry.next()) {
        rows += qry.value(1).toInt();
        syncTime = qry.value(0).toDateTime();

        if (rows >= maxRows)
            return false;
    }

    // unsynced rows are less than limit
    syncTime = QDateTime::currentDateTime();
    return true;
}

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{}

bool Syncer::getRecordsAndFiles(QDateTime& syncTime, QStringList& logs, QStringList& files)
{
    QSqlQuery qry;
    QDateTime lastSync;
    bool finished = setSyncBoundaries(5000, lastSync, syncTime);
    QString condition = QString("created_at > '%1' and created_at <= '%2'").arg(formatDateTime(lastSync), formatDateTime(syncTime));

    qry.exec("select table_name, row_op, row_id, user_id, created_at, row_data from logs where "+ condition);
    while (qry.next())
        logs.append(QString("%1,%2,%3,%4,%5|%6").arg(qry.value(0).toString(), qry.value(1).toString(), qry.value(2).toString(), qry.value(3).toString(), qry.value(4).toString(), qry.value(5).toString()));

    // extract new filenames
    qry.exec("select row_id, row_data from logs where table_name = 'files' and "+ condition);
    while (qry.next())
        files.append(QString("%1/files/").arg(dataFolder()) + qry.value(0).toString() +"."+ qry.value(1).toString().mid(1,3));

    return finished;
}

void Syncer::send(QString url, QMap<QString, QString>& posts, QStringList& files)
{
    QUrl qUrl(url);
    QNetworkRequest request(qUrl);
    QHttpMultiPart* parts = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QMapIterator<QString, QString> i(posts);
    while (i.hasNext()) {
        i.next();
        QHttpPart* part = new QHttpPart();
        part->setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+ i.key() +"\""));
        part->setBody(i.value().toUtf8());
        parts->append(*part);
    }

    foreach (QString filename, files)
    {
        QFileInfo finfo(filename);
        QFile* file = new QFile(finfo.absoluteFilePath());
        if (file->open(QIODevice::ReadOnly)) {
            QHttpPart* part = new QHttpPart();
            part->setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/"+ finfo.suffix().toLower() +""));
            part->setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; filename=\""+ finfo.fileName() +"\""));
            part->setBodyDevice(file);
            file->setParent(parts); // for delete time
            parts->append(*part);
        }
    }

    reply = qnam.post(request, parts);
    parts->setParent(reply); // for delete time
    connect(reply, SIGNAL(finished()), this, SLOT(receive()));
}

void Syncer::receive()
{
    QString response = reply->readAll();

    QFile file("response.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << response;
        file.close();
    }

    QSqlQuery qry;
    QStringList sections = response.split(" - ");
    if (sections[0] == "registered") {
        if (qry.exec(QString("update library set id = %1, license = '%2'").arg(sections[1], sections[2]))) {
            insertLog("library", "update", sections[1]);
        } else
            qDebug() << qry.lastError();
    }
}

void Syncer::sync()
{
    QString url = "http://127.0.0.1/reghaabat-server/backend.php";

    QSqlQuery qry;
    qry.exec("select id, license, synced_at from library");
    qry.next();

    QDateTime syncTime;
    QStringList logs, files;
    QMap<QString, QString> posts;

    if (qry.value(1).toString().isEmpty())
        posts["command"] = "register";
    else {
        posts["id"] = qry.value(0).toString();
        posts["key"] = QCryptographicHash::hash(QString(qry.value(0).toString() + "|x|" + qry.value(1).toString()).toUtf8(), QCryptographicHash::Sha1).toHex();
        posts["command"] = "store";
        posts["finished"] = getRecordsAndFiles(syncTime, logs, files);
        posts["synced_at"] = formatDateTime(syncTime);
        posts["count"] = QString("%1").arg(logs.length());
        posts["logs"] = logs.join("|-|");
    }

    if (posts["command"] == "register" || posts["count"].toInt() > 0)
        send(url, posts, files);
}
