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

    qry.exec(QString("select id from borrows where user_id = %1 and object_id = %2 and received_at is null").arg(userId).arg(objectId));
    if (qry.next())
        return QObject::tr("You borrowed this object.");

    if (! qry.exec(QString("insert into borrows (user_id, object_id) values (%1, %2)").arg(userId).arg(objectId)))
        return qry.lastError().text();

    insertLog("borrows", "insert", qry.lastInsertId());

    return "";
}
