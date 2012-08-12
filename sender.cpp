#include "sender.h"

#include <helper.h>

#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <QFileInfo>
#include <QUrl>

Sender::Sender(QObject *parent)
    :QObject(parent)
{}

void Sender::post(QUrl url, QMap<QString, QString>& data)
{
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    QByteArray bytes;

    QMapIterator<QString, QString> i(data);
    while (i.hasNext()) {
        i.next();
        bytes += QString("%1=%2&").arg(i.key(), i.value()).toAscii();
    }

    reply = qnam.post(req, bytes);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

void Sender::postMultiPart(QUrl url, QMap<QString, QString>& data, QStringList& files)
{
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    QByteArray bytes;
    QString bound="------AaB13x";

    QMapIterator<QString, QString> i(data);
    while (i.hasNext()) {
        i.next();
        bytes += QString("--" + bound + "\r\n").toAscii();
        bytes += QString("Content-Disposition: form-data; name=\"%1\"\r\n\r\n%2\r\n").arg(i.key()).arg(i.value());
    }

    foreach (QString filename, files)
    {
        QFileInfo finfo(filename);
        QFile file(finfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly))
        {
            bytes += QString("--" + bound + "\r\n").toAscii();
            bytes += QString("Content-Disposition: form-data; name=\"%1\"; filename=\"%1\"\r\n").arg(finfo.fileName());
            bytes += "Content-Type: image/"+ finfo.suffix().toLower() +"\r\n\r\n";
            bytes += file.readAll();
            bytes += "\r\n";
        }
    }

    bytes += QString("--" + bound + "--\r\n").toAscii();

    bytes += "\r\n";
    req.setRawHeader(QString("Accept-Encoding").toAscii(), QString("gzip,deflate").toAscii());
    req.setRawHeader(QString("Content-Type").toAscii(),QString("multipart/form-data; boundary=" + bound).toAscii());
    req.setRawHeader(QString("Content-Length").toAscii(), QString::number(bytes.length()).toAscii());

    reply = qnam.post(req, bytes);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

void Sender::httpFinished()
{
    QString response = reply->readAll();
    emit received(response);

    // store response
    QFile file("response.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << response;
        file.close();
    }
}
