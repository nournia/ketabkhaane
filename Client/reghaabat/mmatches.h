#ifndef MMATCHES_H
#define MMATCHES_H

#include <QVariant>
#include <QComboBox>

#include <helper.h>
#include <connector.h>

class MMatches
{
public:

    static bool get(QString matchId, StrMap& match, QList<StrPair>& questions);
    static QString set(QString matchId, StrMap data, QList<StrPair> questions);
    static QString getScoreSql();

    /* answers */

    static QString receive(QString userId, QString matchId);
    static QString deliver(QString userId, QString matchId);
    static QString correct(QString answerId, QString Score);

    /* data */

    static QList<StrPair> ageclasses(bool shortFormat = false)
    {
        QSqlQuery qry;
        QString description = !shortFormat ? "||' - '|| description" : "";
        qry.exec(QString("select id, title %1 from ageclasses").arg(description));

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> categories()
    {
        QSqlQuery qry;
        qry.exec("select id, title from categories");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> states()
    {
        QList<StrPair> tmp;
        tmp.append(qMakePair(QObject::tr("active"), QString("active")));
        tmp.append(qMakePair(QObject::tr("imported"), QString("imported")));
        tmp.append(qMakePair(QObject::tr("disabled"), QString("disabled")));
        return tmp;
    }
};

#endif // MMATCHES_H
