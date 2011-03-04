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
    static QString syncDb();
};

#endif // SYNCER_H
