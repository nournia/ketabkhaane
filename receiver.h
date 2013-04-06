#ifndef RECEIVER_H
#define RECEIVER_H

#include <QNetworkAccessManager>

#include <helper.h>

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QObject *parent = 0);
    
    QStringList queue;
    QNetworkReply* reply;
    QNetworkAccessManager qnam;

    void popUrl();
    void queueUrl(QString args, bool file = false);
    void get(QString args);
    void storeRows(QString table, QVariant rows);

signals:
    void received(QVariantMap data);

public slots:
    void received();
};

#endif // RECEIVER_H
