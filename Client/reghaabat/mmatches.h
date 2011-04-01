#ifndef MMATCHES_H
#define MMATCHES_H

#include <helper.h>
#include <QComboBox>

class MMatches
{
public:
    static StrMap get(QString matchId)
    {
        QSqlQuery qry;
        QString sql = "select matches.title, matches.ageclass, matches.category_id, matches.content, resources.kind, authors.title as author, publications.title as publication, supports.current_state, supports.score, users.firstname || ' ' || users.lastname as corrector "
                      "from matches left join resources on matches.resource_id = resources.id left join authors on resources.author_id = authors.id left join publications on resources.publication_id = publications.id left join supports on matches.id = supports.match_id left join users on supports.corrector_id = users.id where matches.id = " + matchId;
        qry.exec(sql);
        qry.next();
        return getRecord(qry);
    }

    static QSqlQuery getQuestions(QString matchId)
    {
        QSqlQuery qry;
        qry.exec("select question, answer from questions where match_id = "+ matchId);
        return qry;
    }

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
