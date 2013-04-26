#ifndef MOBJECTS_H
#define MOBJECTS_H

#include <helper.h>

class MObjects
{
public:

    static bool get(QString objectId, StrMap& object);
    static QString getNewLabel(QString branch);
    static QString getObjectsQuery();
    static QString set(QString objectId, StrMap data);

    /* branches */

    static QString setRoot(QString rootId, QString typeId, QString title);
    static QString setBranch(QString branchId, QString rootId, QString title, QString label);

    /* borrows */

    static int getFine(QString userId, QString objectId);
    static int getDebt(QString userId);

    static QString charge(QString userId, int fine, int discount, int money);
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

    static QList<StrPair> branches(QString type)
    {
        QSqlQuery qry;
        qry.exec(QString("select branches.id, case when branches.title != '' then roots.title ||' - '|| branches.title else roots.title end from branches inner join roots on branches.root_id = roots.id where roots.type_id = %1 order by branches.id").arg(type));

        QList<StrPair> tmp;
        while(qry.next())
            tmp.append(qMakePair(qry.value(1).toString(), qry.value(0).toString()));
        return tmp;
    }
};

#endif // MOBJECTS_H
