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
    QString url = Reghaabat::instance()->serverUrl;
    if (file)
        url += "files.php?q=";
    else
        url += "data.php?i="+ Reghaabat::instance()->libraryId +"&";
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
