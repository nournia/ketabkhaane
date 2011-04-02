#ifndef MUSERS_H
#define MUSERS_H

#include <QCryptographicHash>

#include <mainwindow.h>
#include <helper.h>

class MUsers
{
public:
    static bool get(QString userId, StrMap user)
    {
        QSqlQuery qry;
        qry.exec("select * from users where id = "+ userId);
        if (! qry.next()) return false;

        user = getRecord(qry);
        return true;
    }

    static QString set(QString userId, StrMap user)
    {
        QSqlQuery qry;
        if (! qry.exec(getReplaceQuery("users", user, userId)))
            return qry.lastError().text();
        return "";
    }

    static bool login(QString userId, QString password)
    {
        QSqlQuery qry;
        QString upassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
        QString query = "select users.id, firstname || ' ' || lastname, permission from users inner join permissions where users.id = "+ userId +" and upassword = '"+ upassword +"'";
        qry.exec(query);

        if (qry.next())
        {
            Reghaabat::instance()->userId = qry.value(0).toString();
            Reghaabat::instance()->userName = qry.value(1).toString();
            Reghaabat::instance()->userPermission = qry.value(1).toString();

            return true;
        }
        else return false;
    }
};


#endif // MUSERS_H
