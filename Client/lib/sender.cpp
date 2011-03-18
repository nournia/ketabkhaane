#include "sender.h"

#include <QDebug>
#include <QFile>

//Sender::Sender(QObject *parent)
//    :QObject(parent)
//{}

void Sender::send(QUrl url, QString& data)
{
//    QFile file("tmp.json");
//    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        QTextStream out(&file);
//        out.setCodec("UTF-8");
//        out << data;
//        file.close();
//    }

//    QFile file2("tmp.json");
//    if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QByteArray datas = data.toUtf8();
//    QNetworkRequest header;
//    header.setRawHeader("User-Agent", "Reghaabat");
//    header.setRawHeader("Host", "localhost");
//    header.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=gc0p4Jq0M2Yt08jU534c0p") );
//    header.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(datas.length()));
//    header.setUrl( QUrl("http://localhost/server.php") );
//    reply = qnam.post(QNetworkRequest(url), datas);


    QByteArray postData;
    postData.append("json=" + data.toUtf8()); // postData.append("param1=alireza&");  postData.append("param2=me");

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

    //qDebug() << reply->readAll();
    qDebug() << "finished";
}
