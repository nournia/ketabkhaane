#include "syncer.h"

#include <helper.h>

#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QCryptographicHash>

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{
    allLogs = uploadedLogs = 0;
    maxRows = 500;
    url = Reghaabat::instance()->serverUrl + "backend.php";
}

bool Syncer::setSyncTime()
{
    QSqlQuery qry;

    int rows = 0;
    qry.exec("select date(created_at) as ct, count(row_id) from logs where ct > '" + formatDateTime(lastSync) + "' group by ct order by ct");
    while (qry.next()) {
        rows += qry.value(1).toInt();
        if (rows >= maxRows) {
            syncTime = qry.value(0).toDateTime();
            return false;
        }
    }

    // unsynced rows are less than limit
    syncTime = QDateTime::currentDateTime();
    return true;
}

bool Syncer::getLogsAndFiles(QStringList& logs, QStringList& files)
{
    QSqlQuery qry;
    bool finished = setSyncTime();
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

void Syncer::send(QMap<QString, QString>& posts, QStringList& files)
{
    QNetworkRequest request(QUrl(url+""));
    QHttpMultiPart* parts = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QMapIterator<QString, QString> i(posts);
    while (i.hasNext()) {
        i.next();
        QHttpPart* part = new QHttpPart();
        part->setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+ i.key() +"\""));
        part->setBody(i.value().toUtf8());
        parts->append(*part);
    }

    foreach (QString filename, files) {
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
    QString data = reply->readAll();
    QFile file("response.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << data;
        file.close();
    }

    QSqlQuery qry;
    QVariantMap response = QJsonDocument::fromJson(data.toUtf8()).object().toVariantMap();
    if (!response.keys().length()) {
        allLogs = 0;
        emit finished(tr("Sync Error."));
        return;
    }
    if (response["state"] == "error") {
        allLogs = 0;
        emit finished(tr("Error") +": "+ response["message"].toString());
        return;
    }

    if (response["command"] == "register") {
        qry.exec(QString("update library set id = %1, license = '%2'").arg(response["id"].toString(), response["license"].toString()));
        insertLog("library", "update", response["id"]);
        lastSync.setDate(QDate(1900, 01, 01));
    }

    if (response["command"] == "query") {
        lastSync = response["synced_at"].toDateTime();
    }

    if (response["command"] == "store") {
        lastSync = response["synced_at"].toDateTime();
        if (!lastSync.isValid()) {
            allLogs = 0;
            emit finished(tr("Sync Error."));
            return;
        }

        uploadedLogs += response["count"].toInt();
        qry.exec(QString("update library set synced_at = '%1'").arg(response["synced_at"].toString()));

        if (allLogs > 0)
            emit progress(100 * uploadedLogs / allLogs);
    }

    sync();
}

void Syncer::sync()
{
    QSqlQuery qry;
    qry.exec("select id, license, synced_at from library");
    qry.next();

    QStringList logs, files;
    QMap<QString, QString> posts;
    posts["id"] = qry.value(0).toString();
    posts["key"] = QCryptographicHash::hash(QString(qry.value(0).toString() + "|x|" + qry.value(1).toString()).toUtf8(), QCryptographicHash::Sha1).toHex();

    if (qry.value(1).toString().isEmpty())
        posts["command"] = "register";
    else if (!lastSync.isValid()) {
        posts["command"] = "query";
        posts["query"] = "synced_at";
    } else {
        posts["command"] = "store";
        posts["finished"] = QString("%1").arg(getLogsAndFiles(logs, files));
        posts["synced_at"] = formatDateTime(syncTime);
        posts["count"] = QString("%1").arg(logs.length());
        posts["xlogs"] = logs.join("|-|"); // to be last in posted data

        // complete
        if (logs.length() == 0) {
            allLogs = uploadedLogs = 0;
            emit progress(100);
            emit finished(tr("Sync complete."));
            return;
        }

        // init progress
        if (allLogs == 0) {
            qry.exec(QString("select count(row_id) from logs where created_at > '%1'").arg(formatDateTime(lastSync)));
            qry.next();
            allLogs = qry.value(0).toInt();
            uploadedLogs = 0;
        }
    }

    send(posts, files);
}
