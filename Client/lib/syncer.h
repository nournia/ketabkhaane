#ifndef SYNCER_H
#define SYNCER_H

#include <sender.h>
#include <connector.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <QDebug>

class Syncer : public QObject
{
    Q_OBJECT
public:
//    Syncer(QObject *parent = 0);
    void syncDb();
    QByteArray getChunk(QDateTime& syncTime, bool& finished);
};

#endif // SYNCER_H
