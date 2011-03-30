#ifndef MUSERS_H
#define MUSERS_H

#include <QCryptographicHash>

#include <mainwindow.h>
#include <helper.h>

class MUsers
{
public:
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

    static StrMap get(QString userId)
    {
        QSqlQuery qry;
        qry.exec("select * from users where id = "+ userId);
        qry.next();
        return getRecord(qry);
    }
};


#endif // MUSERS_H
