#ifndef MOBJECTS_H
#define MOBJECTS_H

#include <helper.h>

class MObjects
{
public:

    static bool get(QString objectId, StrMap& object);
    static QString getNewLabel(QString branch);
    static QString set(QString userId, StrMap object);

    /* borrows */

    static int getFine(QString userId, QString objectId);

    static QString receive(QString userId, QString objectId);
    static QString deliver(QString userId, QString objectId);
    static QString renew(QString userId, QString objectId);

    /* data */

    static QList<StrPair> types()
    {
        QSqlQuery qry;
        qry.exec("select id, title from types");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> roots(QString type)
    {
        QSqlQuery qry;
        qry.exec("select id, title from roots where type_id = "+ type +" order by id");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> branches(QString root)
    {
        QSqlQuery qry;
        qry.exec("select id, title from branches where root_id = "+ root +" order by id");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }
};

#endif // MOBJECTS_H
