#ifndef MUSERS_H
#define MUSERS_H

#include <QCryptographicHash>

#include <helper.h>

class MUsers
{
public:
    static bool get(QString userId, StrMap& user);

    static QString getAgeClassQuery(QString userId);

    static QString getAgeClass(QString userId);

    static QString set(QString userId, StrMap user, QString importedId);

    static bool login(QString userId, QString password, StrMap& user);

    static QString changePassword(QString userId, QString oldPass, QString newPass, QString rePass);

    static QString setPassword(QString userId, QString newPass);

    static QString setPermission(QString userId, QString permission);

    static QString pay(QString userId, int score);
};

#endif // MUSERS_H
