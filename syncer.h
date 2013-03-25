#ifndef SYNCER_H
#define SYNCER_H

#include <connector.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>


class Syncer : public QObject
{
    Q_OBJECT

    QNetworkReply* reply;
    QNetworkAccessManager qnam;

public:
    Syncer(QObject *parent = 0);
    void sync();
    void send(QString url, QMap<QString, QString>& posts, QStringList& files);
    bool getRecordsAndFiles(QDateTime& syncTime, QStringList& logs, QStringList& files);

private slots:
    void receive();
};

#endif // SYNCER_H
