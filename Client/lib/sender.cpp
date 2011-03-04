#include "sender.h"

#include <QDebug>

void Sender::send(QUrl url, QString& data)
{
    QByteArray postData;
    postData.append("json="+ data);
//    postData.append("param1=alireza&");
//    postData.append("param2=me");

//    reply = qnam.get(QNetworkRequest(url));
    reply = qnam.post(QNetworkRequest(url), postData);
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
}

void Sender::httpFinished()
{
    qDebug() << reply->readAll();
}
