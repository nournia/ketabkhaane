#ifndef SYNCER_H
#define SYNCER_H

#include <sender.h>
#include <connector.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QStringList>

class Syncer : public QObject
{
    Q_OBJECT
public:
    Syncer(QObject *parent = 0);
    bool getRecordsAndFiles(QDateTime& syncTime, QStringList& logs, QStringList& files);
};

#endif // SYNCER_H
