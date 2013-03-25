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

    QUrl url;
    QNetworkReply* reply;
    QNetworkAccessManager qnam;
    QDateTime lastSync, syncTime;
    int maxRows;

public:
    Syncer(QObject *parent = 0);

    void send(QMap<QString, QString>& posts, QStringList& files);
    bool getLogsAndFiles(QStringList& logs, QStringList& files);
    bool setSyncTime();
    void sync();

private slots:
    void receive();
};

#endif // SYNCER_H
