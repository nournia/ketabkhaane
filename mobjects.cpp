#include "mobjects.h"

bool MObjects::get(QString objectId, StrMap& object)
{
    QSqlQuery qry;
    qry.exec("select objects.*, authors.title as author, publications.title as publication, branches.root_id "
             "from objects left join authors on objects.author_id = authors.id left join publications on objects.publication_id = publications.id left join branches on objects.branch_id = branches.id where objects.id = "+ objectId);
    if (! qry.next()) return false;

    object = getRecord(qry);
    return true;
}

QString MObjects::getNewLabel(QString branch)
{
    QSqlQuery qry;

    QString label;
    qry.exec("select label from branches where id = "+ branch);
    if (qry.next())
        label = qry.value(0).toString();

    int number = 0;
    qry.exec("select label from objects where branch_id = "+ branch +" order by label desc limit 1");
    if (qry.next())
        if (qry.value(0).toString().left(label.length()) == label)
            number = qry.value(0).toString().mid(label.length()+1).toInt();

    QString result;
    result.sprintf("%3d", number + 1);
    result = label +"-"+ result.replace(" ", "0");
    return result;
}

QString MObjects::set(QString objectId, StrMap object)
{
    QSqlQuery qry;

    // validate
    if (object["title"].toString().trimmed().isEmpty())
        return QObject::tr("Title is required.");

    // set label
    if (objectId.isEmpty())
        object["label"] = getNewLabel(object["branch_id"].toString());

    // new author & publication
    if (! object["author_id"].toString().isEmpty() && object["author_id"].toInt() == 0)
        object["author_id"] = insertTitleEntry("authors", object["author_id"].toString());
    if (! object["publication_id"].toString().isEmpty() && object["publication_id"].toInt() == 0)
        object["publication_id"] = insertTitleEntry("publications", object["publication_id"].toString());

    // store
    if (! qry.exec(getReplaceQuery("objects", object, objectId)))
        return qry.lastError().text();

    if (objectId.isEmpty())
    {
        objectId = qry.lastInsertId().toString();
        insertLog("objects", "insert", objectId);
    }
    else
        insertLog("objects", "update", objectId);

    object["id"] = objectId;
    return "";
}

QString MObjects::receive(QString userId, QString objectId)
{
    QSqlQuery qry;
    qry.prepare("update borrows set received_at = ? where user_id = ? and object_id = ?");
    qry.addBindValue(formatDateTime(QDateTime::currentDateTime()));
    qry.addBindValue(userId);
    qry.addBindValue(objectId);
    if (! qry.exec())
        return qry.lastError().text();

    qry.exec(QString("select id from borrows where user_id = %1 and object_id = %2").arg(userId).arg(objectId));
    if (qry.next())
        insertLog("borrows", "update", qry.value(0));

    return "";
}

QString MObjects::deliver(QString userId, QString objectId)
{
    QSqlQuery qry;

    // check object in users bag
    qry.exec(QString("select id from borrows where user_id = %1 and object_id = %2 and received_at is null").arg(userId).arg(objectId));
    if (qry.next())
        return QObject::tr("You borrowed this object.");

    // check object count in library
    int cnt = 0;
    qry.exec(QString("select cnt from objects where id = %1").arg(objectId));
    if (qry.next())
        cnt = qry.value(0).toInt();

    QStringList users;
    qry.exec(QString("select user_id from borrows where object_id = %1 and received_at is null").arg(objectId));
    while (qry.next())
        users << qry.value(0).toString();

    if (cnt - users.count() <= 0)
        return QObject::tr("We've got no more instance of this object. Following users currently borrowed it: %1").arg(users.join(" ,"));

    // deliver
    if (! qry.exec(QString("insert into borrows (user_id, object_id) values (%1, %2)").arg(userId).arg(objectId)))
        return qry.lastError().text();

    insertLog("borrows", "insert", qry.lastInsertId());

    return "";
}

QString MObjects::renew(QString userId, QString objectId)
{
    QSqlQuery qry;
    qry.prepare("update borrows set renewed_at = ? where user_id = ? and object_id = ?");
    qry.addBindValue(formatDateTime(QDateTime::currentDateTime()));
    qry.addBindValue(userId);
    qry.addBindValue(objectId);
    if (! qry.exec())
        return qry.lastError().text();

    qry.exec(QString("select id from borrows where user_id = %1 and object_id = %2").arg(userId).arg(objectId));
    if (qry.next())
        insertLog("borrows", "update", qry.value(0));

    return "";
}

int MObjects::getDebt(QString userId)
{
    QSqlQuery qry;

    qry.exec(QString("select -1 * sum(score) from transactions where user_id = %1 and (description = 'fin' or description = 'dis' or description = 'chg')").arg(userId));
    qry.next();
    return qry.value(0).toInt();
}

int MObjects::getFine(QString userId, QString objectId)
{
    int days = 0;

    QSqlQuery qry;
    qry.exec(QString("select julianday(date('now')) - julianday(date(ifnull(renewed_at, delivered_at))) as days from borrows where user_id = %1 and object_id = %2 and received_at is null").arg(userId).arg(objectId));
    if (qry.next())
        days = qry.value(0).toInt();

    int fine = 0;
    bool book;
    qry.exec(QString("select bookfine, cdfine, (select type_id from objects where id = %1) from accounts inner join users on users.account = accounts.id where users.id = %2").arg(objectId).arg(userId));
    qry.next();
    book = qry.value(2).toInt() == 0;
    fine = qry.value(qry.value(2).toInt()).toInt();

    if (book) days -= options()["BookBorrowDays"].toInt();

    if (days > 0)
        return fine * days;
    return 0;
}

QString MObjects::charge(QString userId, int fine, int discount, int money)
{
    QSqlQuery qry;

    if ((money + discount > 0) and (money + discount > fine + getDebt(userId)))
        return QObject::tr("Your score goes down zero.");

    // insert fine
    if (fine > 0)
    {
        if (qry.exec(QString("insert into transactions (user_id, score, description) values (%1, %2, 'fin')").arg(userId).arg(-1*fine)))
            insertLog("transactions", "insert", qry.lastInsertId());
        else
            return qry.lastError().text();
    }

    // insert discount
    if (discount > 0)
    {
        if (qry.exec(QString("insert into transactions (user_id, score, description) values (%1, %2, 'dis')").arg(userId).arg(discount)))
            insertLog("transactions", "insert", qry.lastInsertId());
        else
            return qry.lastError().text();
    }

    // insert charge
    if (money > 0)
    {
        if (qry.exec(QString("insert into transactions (user_id, score, description) values (%1, %2, 'chg')").arg(userId).arg(money)))
            insertLog("transactions", "insert", qry.lastInsertId());
        else
            return qry.lastError().text();
    }

    return "";
}
