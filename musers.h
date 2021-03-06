#ifndef MUSERS_H
#define MUSERS_H

#include <QCryptographicHash>

#include <helper.h>

class MUsers
{
public:
    static bool get(QString userId, StrMap& user);
    static QString getAgeClassCase(QString userId);
    static QString getAgeClass(QString userId);
    static QString getNewLabel();
    static QString getGenderCondition();
    static QString getUsersQuery();
    static int getScore(QString userId);
    static int getPayment(QString userId);

    static QString set(QString userId, StrMap& data);
    static bool login(QString userId, QString password, StrMap& user);
    static QString changePassword(QString userId, QString oldPass, QString newPass, QString rePass);
    static QString setPassword(QString userId, QString newPass);
    static QString setPermission(QString userId, QString permission);
    static QString pay(QString userId, int score);


    static QList<StrPair> accounts()
    {
        QSqlQuery qry;
        qry.exec("select id, title from accounts");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }
};

#endif // MUSERS_H
