#ifndef MOBJECTS_H
#define MOBJECTS_H

#include <helper.h>

class MObjects
{
public:
    static bool get(QString objectId, StrMap& object);

    static QString getNewLabel(QString branch);

    static QList<StrPair> types()
    {
        QSqlQuery qry;
        qry.exec("select id, title from types");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> roots()
    {
        QSqlQuery qry;
        qry.exec("select id, title from roots order by id");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }

    static QList<StrPair> branches(QString root)
    {
        QSqlQuery qry;
        qry.exec("select id, title from branches where root_id = "+ root +" oreder by id");

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }
};

#endif // MOBJECTS_H
