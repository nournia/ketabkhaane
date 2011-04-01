#ifndef MMATCHES_H
#define MMATCHES_H

#include <helper.h>
#include <QComboBox>

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

    static void fillAgeClassCombo(QComboBox* cb)
    {
        QSqlQuery qry;
        qry.exec("select id, title || ' - ' || description from ageclasses");

        cb->clear();
        while(qry.next())
            cb->addItem(qry.value(1).toString(), qry.value(0));
    }

    static void fillCategoryCombo(QComboBox* cb)
    {
        QSqlQuery qry;
        qry.exec("select id, title from categories");

        cb->clear();
        while(qry.next())
            cb->addItem(qry.value(1).toString(), qry.value(0));
    }
};

#endif // MMATCHES_H
