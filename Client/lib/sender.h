#ifndef SENDER_H
#define SENDER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

class Sender : public QObject
{
    Q_OBJECT

public:
    void send(QUrl url, QString& data);

private slots:
    void httpFinished();

private:
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
};

#endif // SENDER_H
