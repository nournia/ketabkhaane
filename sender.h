#ifndef SENDER_H
#define SENDER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class Sender : public QObject
{
    Q_OBJECT

    QNetworkAccessManager qnam;
    QNetworkReply* reply;

public:
    Sender(QObject *parent = 0);
    void post(QUrl url, QMap<QString, QString>& data);
    void postMultiPart(QUrl url, QMap<QString, QString>& data, QStringList& files);

signals:
     void received(QString response);

private slots:
    void httpFinished();
};

#endif // SENDER_H
