#ifndef MUSERS_H
#define MUSERS_H

#include <QCryptographicHash>
#include <QVariant>

#include <mainwindow.h>
#include <helper.h>

class MUsers
{
public:
    static bool get(QString userId, StrMap& user)
    {
        QSqlQuery qry;
        qry.exec("select * from users where id = "+ userId);
        if (! qry.next()) return false;

        user = getRecord(qry);
        return true;
    }

    static QString set(QString userId, StrMap user)
    {
        if (user["firstname"].toString().isEmpty() || user["lastname"].toString().isEmpty())
            return QObject::tr("User name is required.");
        if (user["national_id"].toString().isEmpty() || user["national_id"].toInt() == 0)
            return QObject::tr("National id is not valid.");
        if (! user["birth_date"].toDate().isValid())
            return QObject::tr("Birth date is not valid.");

        QSqlQuery qry;
        qry.exec("select id, firstname ||' '|| lastname from users where national_id = "+ user["national_id"].toString());
        if (qry.next())
            if (qry.value(0).toString() != userId)
                return QObject::tr("%1 has exact same national id.").arg(qry.value(1).toString());

        if (! qry.exec(getReplaceQuery("users", user, userId)))
            return qry.lastError().text();
        return "";
    }


    static bool login(QString userId, QString password, StrMap& user)
    {
        QSqlQuery qry;
        QString upassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
        QString query = "select users.id, firstname ||' '|| lastname as name, permission from users inner join permissions where users.id = "+ userId +" and upassword = '"+ upassword +"'";
        qry.exec(query);

        if (qry.next())
        {
            user = getRecord(qry);
            return true;
        }
        else
            return false;
    }

    static QString changePassword(QString userId, QString oldPass, QString newPass, QString rePass)
    {
        QSqlQuery qry;
        qry.exec("select upassword from users where id = "+ userId);
        if (! qry.next()) return false;

        if (qry.value(0).toString() != QCryptographicHash::hash(oldPass.toUtf8(), QCryptographicHash::Sha1).toHex())
            return QObject::tr("Invalid Old Password.");

        if (newPass != rePass)
            return QObject::tr("New password must exact matches with retyped one.");

        QString msg = setPassword(userId, newPass);
        if (! msg.isEmpty())
            return msg;

        return QObject::tr("Password Changed.");
    }

    static QString setPassword(QString userId, QString newPass)
    {
        QSqlQuery qry;

        if (! validPassword(newPass))
            return QObject::tr("Password phrase length must be greater than 6 characters and mustn't be a pure number.");

        QString password = QCryptographicHash::hash(newPass.toUtf8(), QCryptographicHash::Sha1).toHex();
        if (! qry.exec(QString("update users set upassword = '%1' where id = %2").arg(password).arg(userId)))
            return qry.lastError().text();

        return "";
    }

    static QString setPermission(QString userId, QString permission)
    {
        QSqlQuery qry;

        qry.exec("select permission from permissions where user_id = "+ userId);
        if (qry.next())
        {
            if (qry.value(0).toString() == "master")
            {
                qry.exec("select user_id from permissions where permission = 'master' and user_id != "+ userId);
                if (! qry.next())
                    return QObject::tr("You must have at least one master user.");
            }
        }

        if (! qry.exec(QString("update permissions set permission = '%1' where user_id = %2").arg(permission).arg(userId)))
            return qry.lastError().text();

        return "";
    }

    static bool validPassword(QString pass)
    {
        if (pass.length() < 6)
            return false;
        else if (pass.toInt() != 0)
            return false;
        return true;
    }

};


#endif // MUSERS_H
