#ifndef SYNCER_H
#define SYNCER_H

#include <QObject>

class Syncer : public QObject
{
    Q_OBJECT
public:
    Syncer(QObject *parent = 0);
    void exec();
    void registerDb();
    void sendLogs();

public slots:
    void registerFinished(QString response);
    void sendFinished(QString response);
};

#endif // SYNCER_H
