#ifndef MMATCHES_H
#define MMATCHES_H

#include <helper.h>

class MMatches
{
public:
    static void receive(QString userId, QString matchId)
    {
        QSqlQuery qry;
        qry.prepare("update answers set received_at = ? where user_id = ? and match_id = ?");
        qry.addBindValue(QDateTime::currentDateTime());
        qry.addBindValue(userId);
        qry.addBindValue(matchId);
        qry.exec();
    }

    static void deliver(QString userId, QString matchId)
    {
        QSqlQuery qry;
        qry.prepare("insert into answers (user_id, match_id) values (?, ?)");
        qry.addBindValue(userId);
        qry.addBindValue(matchId);
        qry.exec();
    }
};

#endif // MMATCHES_H
