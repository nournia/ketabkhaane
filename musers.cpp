
#include <musers.h>

bool MUsers::get(QString userId, StrMap& user)
{
    QSqlQuery qry;
    qry.exec("select users.*, permissions.label, permissions.account_id from users inner join permissions on users.id = permissions.user_id where users.id = "+ userId);
    if (! qry.next()) return false;

    user = getRecord(qry);
    return true;
}

QString MUsers::getAgeClassCase(QString when)
{
    QSqlQuery qry;
    qry.exec("select id, beginage, endage from ageclasses order by id");

    QString field = when + " - users.birth_date";
    QString caseSt = "case";
    while (qry.next())
        caseSt += QString(" when %1 between %2 and %3 then %4").arg(field).arg(qry.value(1).toString()).arg(qry.value(2).toString()).arg(qry.value(0).toString());
    caseSt += " else -10 end";

    return caseSt;
}

QString MUsers::getAgeClass(QString userId)
{
    QSqlQuery qry;
    qry.exec(QString("select %1 as ageclass from users where id = %2").arg(getAgeClassCase("current_date")).arg(userId));
    qry.next();
    return qry.value(0).toString();
}

QString MUsers::getNewLabel()
{
    QSqlQuery qry;
    qry.exec("select max(label) from permissions");
    if (qry.next() && qry.value(0).toInt())
        return QString("%1").arg(qry.value(0).toInt() + 1);
    else
        return "1111";
}

QString MUsers::getGenderCondition()
{
    if (! Reghaabat::hasAccess("master"))
        return QString(" and gender = '%1' ").arg(Reghaabat::instance()->userGender);
    return "";
}

QString MUsers::getUsersQuery()
{
    return "select users.id as cid, label as clabel, firstname||' '||lastname as ctitle from users inner join permissions on users.id = permissions.user_id where 1" + getGenderCondition();
}

QString MUsers::set(QString userId, StrMap& data)
{
    QSqlQuery qry;

    // validation

    // basic
    if (data["firstname"].toString().isEmpty() || data["lastname"].toString().isEmpty())
        return QObject::tr("User name is required.");
    if (! data["national_id"].toString().isEmpty() && data["national_id"].toInt() == 0)
        return QObject::tr("National id is not valid.");
    if (! data["birth_date"].toDate().isValid())
        return QObject::tr("Birth date is not valid.");

    /*
    // used name
    user["firstname"] = refineText(user["firstname"].toString().trimmed());
    user["lastname"] = refineText(user["lastname"].toString().trimmed());
    qry.exec(QString("select id, firstname ||' '|| lastname as name from users where name = '%1'").arg(data["firstname"].toString() +" "+ data["lastname"].toString()));
    if (qry.next())
        if (qry.value(0).toString() != userId)
            return QObject::tr("There is another user with this name.");
    */

    // used national id
    qry.exec("select id, firstname ||' '|| lastname from users where national_id = "+ data["national_id"].toString());
    if (qry.next())
        if (qry.value(0).toString() != userId)
            return QObject::tr("%1 has exact same national id.").arg(qry.value(1).toString());

    // set user label
    if (userId.isEmpty())
        data["label"] = getNewLabel();

    // store
    bool create = userId.isEmpty();
    StrMap permission;
    permission["account_id"] = data.take("account_id");
    permission["label"] = data.take("label");

    if (! qry.exec(getReplaceQuery("users", data, userId)))
        return qry.lastError().text();

    if (create)
        insertLog("users", "insert", userId);
    else
        insertLog("users", "update", userId);

    data["id"] = userId;

    // store permission
    permission["user_id"] = userId;
    if (create)
        permission["permission"] = "user";

    QString permissionId;
    qry.exec("select id from permissions where user_id = "+ userId);
    if (qry.next())
        permissionId = qry.value(0).toString();

    if (! qry.exec(getReplaceQuery("permissions", permission, permissionId)))
        return qry.lastError().text();

    if (permissionId.isEmpty()) {
        permissionId = qry.lastInsertId().toString();
        insertLog("permissions", "insert", permissionId);
    }
    else
        insertLog("permissions", "update", permissionId);

    return "";
}

bool MUsers::login(QString userId, QString password, StrMap& user)
{
    QSqlQuery qry;
    QString upassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
    QString query = QString("select users.id, firstname||' '||lastname as name, gender, permission from users inner join permissions on users.id = permissions.user_id where users.id = %1 and upassword = '%2'").arg(userId).arg(upassword);
    qry.exec(query);

    if (qry.next())
    {
        user = getRecord(qry);
        return true;
    }
    else
        return false;
}

QString MUsers::changePassword(QString userId, QString oldPass, QString newPass, QString rePass)
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

bool validPassword(QString pass)
{
    if (pass.length() < 6)
        return false;
    else if (pass.toInt() != 0)
        return false;
    return true;
}

QString MUsers::setPassword(QString userId, QString newPass)
{
    QSqlQuery qry;

    if (newPass.trimmed().isEmpty())
        return QObject::tr("Password is required.");

//        if (! validPassword(newPass))
//            return QObject::tr("Password phrase length must be greater than 6 characters and mustn't be a pure number.");

    QString password = QCryptographicHash::hash(newPass.toUtf8(), QCryptographicHash::Sha1).toHex();
    if (! qry.exec(QString("update users set upassword = '%1' where id = %2").arg(password).arg(userId)))
        return qry.lastError().text();

    insertLog("users", "update", userId);

    return "";
}

QString MUsers::setPermission(QString userId, QString permission)
{
    QSqlQuery qry;

    // validation
    if (permission != "master")
    {
        qry.exec("select user_id from permissions where permission = 'master' and user_id != "+ userId);
        if (! qry.next())
        {
            qry.exec("select id from permissions");
            if (! qry.next())
                permission = "master";
            else
                return QObject::tr("You must have at least one master user.");
        }
    }

    qry.exec("select id, permission from permissions where user_id = "+ userId);
    if (qry.next())
    {
        QString id = qry.value(0).toString();

        if (! qry.exec(QString("update permissions set permission = '%1' where id = %2").arg(permission).arg(id)))
            return qry.lastError().text();

        insertLog("permissions", "update", id);
    }
    else
    {
        if (! qry.exec(QString("insert into permissions (user_id, permission) values (%1, '%2')").arg(userId).arg(permission)))
            return qry.lastError().text();

        insertLog("permissions", "insert", qry.lastInsertId());
    }

    return "";
}

int MUsers::getScore(QString userId)
{
    QSqlQuery qry;
    qry.exec(QString("select sum(score) from transactions where user_id = %1 and substr(description, 1, 3) = 'mid' and created_at > (select started_at from library)").arg(userId));
    if (qry.next())
        return qry.value(0).toInt();
    return 0;
}

int MUsers::getPayment(QString userId)
{
    QSqlQuery qry;
    qry.exec(QString("select -1 * sum(score) from transactions where user_id = %1 and description = 'pay' and created_at > (select started_at from library)").arg(userId));
    if (qry.next())
        return qry.value(0).toInt();
    return 0;
}

QString MUsers::pay(QString userId, int score)
{
    QSqlQuery qry;

    if ((getScore(userId) - getPayment(userId)) - score < 0)
        return QObject::tr("You have not sufficent score.");

    if (!qry.exec(QString("insert into transactions (user_id, score, description) values (%1, %2, 'pay')").arg(userId).arg(-1 * score)))
        return qry.lastError().text();
    insertLog("transactions", "insert", qry.lastInsertId());

    return "";
}
