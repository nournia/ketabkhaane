#include "sender.h"

#include <helper.h>
#include <syncer.h>

#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QUrl>
#include <QHttpMultiPart>

Sender::Sender(QObject *parent)
    :QObject(parent)
{}

void Sender::send(QString url, QMap<QString, QString>& posts, QStringList& files)
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

    parts->setParent(reply); // for delete time
    reply = qnam.post(request, parts);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

void Sender::httpFinished()
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
            this->sync();
        } else
            qDebug() << qry.lastError();
    }
}

void Sender::sync()
{
    QString url = "http://127.0.0.1/reghaabat-server/backend.php";

    QSqlQuery qry;
    qry.exec("select id, license, synced_at from library");
    qry.next();

    QStringList files;
    QMap<QString, QString> posts;
    posts["id"] = qry.value(0).toString();
    posts["key"] = QCryptographicHash::hash(QString(qry.value(0).toString() + "-" + qry.value(1).toString()).toUtf8(), QCryptographicHash::Sha1).toHex();
    posts["synced_at"] = qry.value(2).toString();

    if (qry.value(1).toString().isEmpty())
        posts["command"] = "register";

    send(url, posts, files);
}
