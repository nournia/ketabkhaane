#include "sender.h"

#include <QDebug>
#include <QFile>

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
    QFile file("tmp.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        //out.setCodec( "UTF-8" );
        out << reply->readAll();
        file.close();
    }

    qDebug() << "sync finished";
}
