
#include <mmatches.h>
#include <musers.h>

#include <QJsonDocument>
#include <QJsonArray>

bool MMatches::get(QString matchId, StrMap& match, QList<StrPair>& questions)
{
    QSqlQuery qry;
    qry.exec("select matches.title, matches.ageclass, matches.object_id, objects.title as object, matches.category_id, matches.content, supports.current_state, supports.score, supports.corrector_id, users.firstname || ' ' || users.lastname as corrector "
             "from matches left join objects on matches.object_id = objects.id left join supports on matches.id = supports.match_id left join users on supports.corrector_id = users.id where matches.id = " + matchId);
    if (! qry.next()) return false;

    match = getRecord(qry);
    if (!match["category_id"].toString().isEmpty())
        match["content"] = match["content"].toString().replace("src=\"", QString("src=\"%1/").arg(filesUrl()));
    else
        questions = extractQuestions(match["content"].toString());

    return true;
}

QList<StrPair> MMatches::extractQuestions(QString content)
{
    QList<StrPair> questions;
    QVariantList items = QJsonDocument::fromJson(content.mid(5).toUtf8()).array().toVariantList();
    foreach (QVariant item, items)
        questions.append(qMakePair(item.toStringList()[0], item.toStringList()[1]));
    return questions;
}

QStringList MMatches::extractFilenames(QString content)
{
    QStringList filenames;
    for (int i = 0; (i = content.indexOf("src=\"", i+1)) > 0;) {
        int cur = i + QString("src=\"").length();
        filenames << content.mid(cur, content.indexOf("\"", cur) - cur);
    }
    return filenames;
}

QString MMatches::set(QString matchId, StrMap data, QList<StrPair> questions)
{
    QSqlQuery qry;

    // basic validation
    if (data["title"].toString().isEmpty())
        return QObject::tr("Title is required.");
    if (data["corrector_id"].toString().isEmpty())
        return QObject::tr("Corrector is required.");
    if (data["score"].toInt() <= 0)
        return QObject::tr("Score is invalid.");

    // used title
    data["title"] = refineText(data["title"].toString().trimmed());
    qry.exec(QString("select id from matches where title = '%1'").arg(data["title"].toString()));
    if (qry.next())
        if (qry.value(0).toString() != matchId)
            return QObject::tr("There is another match with this title.");

    // store
    QSqlDatabase db = Connector::connectDb();
    db.transaction();

    StrMap match;
    QStringList oldfiles, newfiles;

    match["category_id"] = "";
    match["content"] = "";
    match["object_id"] = "";

    if (! data["category_id"].toString().isEmpty())
    {
        match["category_id"] = data["category_id"];

        QString content = data["content"].toString();

        // move extenal files
        QStringList filenames = extractFilenames(content);
        foreach (QString filename, filenames)
        {
            QString newfile = getInAppFilename(filename);
            content.replace(filename, newfile);
        }

        // extract old filenames
        qry.exec("select content from matches where id = "+ matchId);
        if (qry.next())
            oldfiles = extractFilenames(qry.value(0).toString());

        match["content"] = content;
    } else {
        match["object_id"] = data["object_id"];

        // questions
        QStringList content; int q = 0, a = 0;
        foreach (StrPair item, questions) {
            if (!item.first.isEmpty()) q++;
            if (!item.second.isEmpty()) a++;
            content.append(QString("[%1,%2]").arg(getJsonValue(item.first.replace("'", "\"")), getJsonValue(item.second.replace("'", "\""))));
        }
        QString aq = QString("%1/%2").arg(a).arg(q);
        match["content"] = QString("%1[%2]").arg(aq.leftJustified(5, ' '), content.join(","));
    }

    match["designer_id"] = data["corrector_id"];
    match["title"] = data["title"];
    match["ageclass"] = data["ageclass"];

    // matches table
    bool create = matchId.isEmpty();

    if (! qry.exec(getReplaceQuery("matches", match, matchId))) {
        db.rollback();
        return qry.lastError().text();
    }
    if (create)
        insertLog("matches", "insert", matchId);
    else
        insertLog("matches", "update", matchId);

    QString supportId;
    StrMap support;
    support["match_id"] = matchId;
    support["corrector_id"] = data["corrector_id"];
    support["current_state"] = data["current_state"];
    support["score"] = data["score"];


    // supports table
    qry.exec("select id from supports where match_id = "+ matchId);
    if (qry.next())
        supportId = qry.value(0).toString();
    if (! qry.exec(getReplaceQuery("supports", support, supportId))) {
        db.rollback();
        return qry.lastError().text();
    }
    if (supportId.isEmpty()) {
        supportId = qry.lastInsertId().toString();
        insertLog("supports", "insert", supportId);
    } else
        insertLog("supports", "update", supportId);


    // files table
    newfiles = extractFilenames(match["content"].toString());
    foreach (QString filename, oldfiles)
        if (! newfiles.contains(filename))
            removeInAppFile(filename);


    db.commit();

    return "";
}

QString MMatches::isDeliverable(QString userId, QString matchId)
{
    QSqlQuery qry;

    // max concurrent matches
    qry.exec(QString("select count(match_id) from answers where user_id = %1 and delivered_at > (select started_at from library) and received_at is null").arg(userId));
    if (qry.next() && qry.value(0).toInt() >= options()["MaxConcurrentMatches"].toInt())
        return QObject::tr("You received enough matches at the moment.");

    // max matches in one day
    qry.exec(QString("select count(match_id) from answers where user_id = %1 and delivered_at > (select started_at from library) and delivered_at > datetime('now', '-12 hours')").arg(userId));
    if (qry.next() && qry.value(0).toInt() >= options()["MaxMatchesInOneDay"].toInt())
        return QObject::tr("You received enough matches today.");

    // delivered match
    qry.exec(QString("select id from answers where user_id = %1 and match_id = %2").arg(userId).arg(matchId));
    if (qry.next())
        return QObject::tr("You received this match before");

    // match state
    qry.exec(QString("select current_state from supports where match_id = %1").arg(matchId));
    if (! qry.next())
        return QObject::tr("This match is not supported.");

    if (qry.value(0).toString() != "active")
        return QObject::tr("This match is not in active state.");

    // user ageclass
    qry.exec(QString("select abs(ageclass - %1) from matches where id = %2").arg(MUsers::getAgeClass(userId)).arg(matchId));
    if (! qry.next())
        return QObject::tr("Error in user or match");

    if (qry.value(0).toInt() > 1)
        return QObject::tr("This matches' ageclass deffers more than one level with yours.");

    return "";
}

QString MMatches::deliver(QString userId, QString matchId)
{
    // validations
    QString msg = isDeliverable(userId, matchId);
    if (! msg.isEmpty())
        return msg;

    // deliver
    QSqlQuery qry;
    qry.prepare("insert into answers (user_id, match_id) values (?, ?)");
    qry.addBindValue(userId);
    qry.addBindValue(matchId);
    if (! qry.exec())
        return qry.lastError().text();

    insertLog("answers", "insert", qry.lastInsertId());

    return "";
}

QString MMatches::receive(QString userId, QString matchId)
{
    QSqlQuery qry;
    qry.prepare("update answers set received_at = ? where user_id = ? and match_id = ?");
    qry.addBindValue(formatDateTime(QDateTime::currentDateTime()));
    qry.addBindValue(userId);
    qry.addBindValue(matchId);
    if (! qry.exec())
        return qry.lastError().text();

    qry.exec(QString("select id from answers where user_id = %1 and match_id = %2").arg(userId).arg(matchId));
    if (qry.next())
        insertLog("answers", "update", qry.value(0));

    return "";
}

QString MMatches::correct(QString answerId, QString Score)
{
    QSqlQuery qry;

    qry.prepare("select supports.score, answers.user_id, answers.match_id from supports inner join answers on supports.match_id = answers.match_id where answers.id = ?");
    qry.addBindValue(answerId);
    qry.exec();
    if (! qry.next())
        return QObject::tr("Invalid record selected.");

    float rate = Score.toFloat() / qry.value(0).toInt();
    QString userId = qry.value(1).toString(), matchId = qry.value(2).toString();

    if (rate < -1 || rate > 2)
        return QObject::tr("Score must be less than 2 * max score.");

    qry.prepare("update answers set rate = ?, corrected_at = current_timestamp where id = ?");
    qry.addBindValue(rate);
    qry.addBindValue(answerId);
    if (! qry.exec())
        return qry.lastError().text();

    insertLog("answers", "update", answerId);


    // register score

    qry.exec(QString("select id from transactions where user_id = %1 and description = 'mid:%2'").arg(userId).arg(matchId));
    if (qry.next())
    {
        QString tid = qry.value(0).toString();

        qry.exec(getScoreSql("select") + " and answers.id = "+ answerId);
        qry.next();
        QString score = qry.value(1).toString();

        if (qry.exec(QString("update transactions set score = %1 where id = %2").arg(score).arg(tid)))
            insertLog("transactions", "update", tid);
        else
            return qry.lastError().text();
    }
    else
    {
        if (qry.exec(getScoreSql("insert") + " and answers.id = "+ answerId))
            insertLog("transactions", "insert", qry.lastInsertId());
        else
            return qry.lastError().text();
    }

    return "";
}

QString MMatches::getScoreSql(QString opt = "insert")
{
    QString scoreSt = "answers.rate * supports.score * (case matches.ageclass - ("+ MUsers::getAgeClassCase("answers.corrected_at") +") when 0 then 1 when 1 then 1.25 when -1 then 0.75 else 0 end)";
    QString selectSt = "select answers.user_id, round("+ scoreSt +") as score, answers.corrected_at, 'mid:'||answers.match_id from answers inner join supports on answers.match_id = supports.match_id inner join matches on answers.match_id = matches.id inner join users on answers.user_id = users.id where answers.rate is not null";

    if (opt == "insert")
        return "insert into transactions (user_id, score, created_at, description) " + selectSt;
    else
        return selectSt;
}
