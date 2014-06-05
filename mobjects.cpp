#include "mobjects.h"

bool MObjects::get(QString objectId, StrMap& object)
{
    QSqlQuery qry;
    qry.exec("select objects.*, belongs.branch_id, belongs.label, belongs.cnt, authors.title as author, publications.title as publication, branches.root_id "
             "from objects inner join belongs on objects.id = belongs.object_id left join authors on objects.author_id = authors.id left join publications on objects.publication_id = publications.id left join branches on belongs.branch_id = branches.id where objects.id = "+ objectId);
    if (! qry.next()) return false;

    object = getRecord(qry);
    return true;
}

QString MObjects::getNewLabel(QString branch)
{
    if (branch.isEmpty())
        return "";

    QSqlQuery qry;

    QString label;
    qry.exec("select label from branches where id = "+ branch);
    if (qry.next())
        label = qry.value(0).toString();

    int number = 0;
    qry.exec("select label from belongs where branch_id = "+ branch +" order by label desc limit 1");
    if (qry.next())
        if (qry.value(0).toString().left(label.length()) == label)
            number = qry.value(0).toString().mid(label.length()+1).toInt();

    QString result;
    result.sprintf("%3d", number + 1);
    result = label +"-"+ result.replace(" ", "0");
    return result;
}

QString MObjects::getObjectsQuery()
{
    return "select objects.id as cid, belongs.label as clabel, objects.title as ctitle from objects inner join belongs on objects.id = belongs.object_id";
}

QString MObjects::set(QString objectId, StrMap data)
{
    QSqlQuery qry;

    // validate
    if (data["title"].toString().trimmed().isEmpty())
        return QObject::tr("Title is required.");

    // new author, publication and branch
    if (! data["author_id"].toString().isEmpty() && data["author_id"].toInt() == 0)
        data["author_id"] = insertTitleEntry("authors", data["author_id"].toString());
    if (! data["publication_id"].toString().isEmpty() && data["publication_id"].toInt() == 0)
        data["publication_id"] = insertTitleEntry("publications", data["publication_id"].toString());
    if (! data["branch_id"].toString().isEmpty() && data["branch_id"].toInt() == 0)
        data["branch_id"] = getBranchId(data["branch_id"].toString());

    // set label
    if (objectId.isEmpty())
        data["label"] = getNewLabel(data["branch_id"].toString());

    // store object
    StrMap object;
    object["title"] = data["title"];
    object["author_id"] = data["author_id"];
    object["publication_id"] = data["publication_id"];
    object["type_id"] = data["type_id"];

    bool create = objectId.isEmpty();
    if (! qry.exec(getReplaceQuery("objects", object, objectId)))
        return qry.lastError().text();

    if (create)
        insertLog("objects", "insert", objectId);
    else
        insertLog("objects", "update", objectId);

    data["id"] = objectId;

    // store belong
    QString belongId;
    StrMap belong;
    qry.exec("select id from belongs where object_id = "+ objectId);
    if (qry.next())
        belongId = qry.value(0).toString();

    belong["object_id"] = objectId;
    belong["branch_id"] = data["branch_id"];
    belong["label"] = data["label"];
    belong["cnt"] = data["cnt"];

    if (! qry.exec(getReplaceQuery("belongs", belong, belongId)))
        return qry.lastError().text();

    if (belongId.isEmpty()) {
        belongId = qry.lastInsertId().toString();
        insertLog("belongs", "insert", belongId);
    }
    else
        insertLog("belongs", "update", belongId);

    return "";
}

QString MObjects::setRoot(QString rootId, QString typeId, QString title)
{
    QSqlQuery qry;

    StrMap data;
    data["type_id"] = typeId;
    data["title"] = title;

    bool create = rootId.isEmpty();
    if (! qry.exec(getReplaceQuery("roots", data, rootId)))
        return qry.lastError().text();

    insertLog("roots", create ? "insert" : "update", rootId);

    return rootId;
}

QString MObjects::setBranch(QString branchId, QString rootId, QString title, QString label, QString& hint)
{
    QSqlQuery qry;

    StrMap data;
    data["root_id"] = rootId;
    data["title"] = title;
    data["label"] = label;

    bool create = branchId.isEmpty();

    QString oldLabel;
    if (!create) {
        qry.exec("select label from branches where id = "+ branchId);
        if (qry.next())
            oldLabel = qry.value(0).toString();

        // check duplicate label
        qry.exec(QString("select id from branches where id != %1 and label = '%2'").arg(branchId, label));
        if (qry.next())
            return QObject::tr("There is another branch with this label!");
    }

    if (! qry.exec(getReplaceQuery("branches", data, branchId).replace("null", "''")))
        return qry.lastError().text();

    insertLog("branches", create ? "insert" : "update", branchId);

    // update object labels
    if (oldLabel != label) {
        if (! qry.exec(QString("update belongs set label = '%2-'||substr(label, 5, 7) where substr(label, 1, 3) == '%1'").arg(oldLabel, label)))
            return qry.lastError().text();

        int i = 0;
        qry.exec(QString("select id from belongs where substr(label, 1, 3) = '%1'").arg(label));
        for(; qry.next(); i++)
            insertLog("belongs", "update", qry.value(0).toString());

        if (i > 0)
            hint = QObject::tr("Label of %1 objects changed.").arg(i);
    }

    return branchId;
}

QString MObjects::getNewLabel(QString typeId, QString rootId) {
    QSqlQuery qry;

    // create root
    if (rootId.isEmpty()) {
        qry.exec("select count(id) from roots where type_id = "+ typeId);
        qry.next();

        if (typeId == "0")
            return "1" + qry.value(0).toString() + "0";
        else if (typeId == "1")
            return "2" + qry.value(0).toString() + "0";
        else
            return "";
    }
    // create branch
    else {
        qry.exec("select max(label) from branches where root_id = "+ rootId);
        qry.next();

        // todo: fix overflow bug
        return QString::number(qry.value(0).toInt() + 1);
    }
}

QString MObjects::getBranchId(QString title) {
    QStringList values = title.split(" - ");
    QString root = values.value(0, "").trimmed();
    QString branch = values.value(1, "").trimmed();

    QString rootId, branchId;
    QSqlQuery tmp;
    StrMap entity;

    tmp.exec("select id from roots where title = '"+ root +"'");
    if (tmp.next())
        rootId = tmp.value(0).toString();
    else {
        entity["title"] = root;
        entity["type_id"] = "0";
        QString label = getNewLabel("0", "");
        if (tmp.exec(getReplaceQuery("roots", entity, rootId))) {
            insertLog("roots", "insert", rootId);

            // insert root branch
            entity.clear();
            entity["title"] = "";
            entity["root_id"] = rootId;
            entity["label"] = label;
            if (tmp.exec(getReplaceQuery("branches", entity, branchId).replace("null", "''")))
                insertLog("branches", "insert", branchId);
            else
                qDebug() << tmp.lastError().text();
        } else
            return "";
    }

    tmp.exec("select id from branches where title = '"+ branch +"' and root_id = "+ rootId);
    if (tmp.next())
        return tmp.value(0).toString();
    else {
        branchId = "";
        entity.clear();
        entity["title"] = branch;
        entity["root_id"] = rootId;
        entity["label"] = getNewLabel("0", rootId);
        if (tmp.exec(getReplaceQuery("branches", entity, branchId))) {
            insertLog("branches", "insert", branchId);
            return branchId;
        } else
            qDebug() << tmp.lastError().text();
    }

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
    qry.exec(QString("select cnt from belongs where object_id = %1").arg(objectId));
    if (qry.next())
        cnt = qry.value(0).toInt();

    QStringList users;
    qry.exec(QString("select users.firstname||' '||users.lastname from borrows inner join users on borrows.user_id = users.id where object_id = %1 and received_at is null").arg(objectId));
    while (qry.next())
        users << qry.value(0).toString();

    if (cnt - users.count() <= 0) {
        if (users.count() == 0)
            return QObject::tr("We've got no more instance of this object.");

        QString list;
        if (users.count() > 1)
            list = QObject::tr(" and ") + users.takeLast();
        list = users.join(QObject::tr(", ")) + list;
        return QObject::tr("We've got no more instance of this object. Following users currently borrowed it: %1").arg(list);
    }

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
    qry.exec(QString("select bookfine, cdfine, (select type_id from objects where id = %1) from accounts where id = (select account_id from permissions where user_id = %2)").arg(objectId).arg(userId));
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
