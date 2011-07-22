#include <helper.h>

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <json.h>


// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;


QString QVariantMapToString(QVariantMap& data)
{
    QString json;
    bool first = true;

    json = "{";
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext()) {
        i.next();
        json += (first ? "" : ", ") + QString("\"%1\": %2").arg(i.key()).arg(getJsonValue(i.value().toString()));
        if (first) first = false;
    }

    json += "}";

    return json;
}

QString getAbsoluteAddress(QString address)
{
    return QCoreApplication::applicationDirPath() + "/" + address;
}

QString dataFolder()
{
    QSettings settings("Rooyesh", "Reghaabat");
    return settings.value("DataFolder", getAbsoluteAddress("data")).toString();
}

QString filesUrl()
{
    QString folder = dataFolder();
    if (!folder.startsWith("//"))
        folder = "///" + folder;
    return QString("file:%1/files").arg(folder);
}

StrMap getRecord(QSqlQuery& query)
{
    StrMap map;
    for (int i = 0; i < query.record().count(); i++)
        map[query.record().fieldName(i)] = query.value(i);
    return map;
}

QString getReplaceQuery(QString table, StrMap data, QString id)
{
    QString fields, values;

    bool first = true;
    QString sep, val;
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext()) {
        i.next();

        sep = first ? "" : ",";
        val = "'" + i.value().toString() + "'";
        if (val == "''") val = "null";

        if (id == "")
        {
            fields.append(sep + i.key());
            values.append(sep + val);
        } else
            values.append(sep + i.key() + "=" + val);
        first = false;
    }

    values = refineText(values);
    if (id == "")
        return QString("insert into %1 (%2) values (%3)").arg(table).arg(fields).arg(values);
    else
        return QString("update %1 set %2 where id = %3").arg(table).arg(values).arg(id);
}

void fillComboBox(QComboBox* combobox, QList<StrPair> data)
{
    combobox->clear();
    for (int i = 0; i < data.size(); i++)
        combobox->addItem(data.at(i).first, data.at(i).second);
}

QVariant insertTitleEntry(QString table, QString title)
{
    title = refineText(title);
    QVariant null; null.clear();
    if (title == "") return null;

    QSqlQuery tmp;
    tmp.exec("select id from " + table + " where title = '"+ title +"'");

    if (tmp.next())
        return tmp.value(0);

    if (tmp.exec("insert into " + table + " (title) values ('" + title + "')"))
    {
        QString id = tmp.lastInsertId().toString();
        insertLog(table, "insert", id);
        return id;
    }
}

QString getInAppFilename(QString filename)
{
    if (! filename.startsWith(filesUrl()))
    {
        QString ext = filename.mid(filename.indexOf('.') + 1);

        QSqlQuery qry;
        if (qry.exec(QString("insert into files (extension) values ('%1')").arg(ext)))
        {
            QString id = qry.lastInsertId().toString();
            insertLog("files", "insert", id);

            QString newfile = QString("%1/files/%2.%3").arg(dataFolder()).arg(id).arg(ext);

            if (QFile::exists(newfile))
                QFile::remove(newfile);

            if (QFile::copy(filename, newfile))
                filename = newfile;
            else
            {
                qDebug() << "file copy error: " << newfile;
                return "";
            }
        } else {
            qDebug() << qry.lastError().text();
            return "";
        }
    }

    return QFileInfo(filename).fileName();
}

void removeInAppFile(QString filename)
{
    QString id = QFileInfo(filename).baseName();
    QSqlQuery qry;
    if (! qry.exec("delete from files where id = " + id))
        qDebug() << qry.lastError();

    insertLog("files", "delete", id);

    filename = QString("%1/files/%2").arg(dataFolder()).arg(filename);
    if (! QFile::remove(filename))
        qDebug() << "remove file error: " << filename;
}

void insertLog(QString table, QString operation, QVariant id, QString userId, QDateTime time)
{
    if (operation != "insert" && operation != "update" && operation != "delete")
        qDebug() << "log operation error: " << operation;

    QString data;

    QSqlQuery qry;
    qry.exec(QString("select * from %1 where id = %2").arg(table).arg(id.toString()));
    if (qry.next())
        data = getRecordJSON(qry);

    if (operation == "update")
    {
        qry.exec(QString("select row_data from logs where table_name = '%1' and row_id = %2 order by created_at desc").arg(table).arg(id.toString()));
        if (qry.next() && data == qry.value(0).toString())
            return; // new data exact matches old one
    }

    if (! time.isValid())
        time = QDateTime::currentDateTime();

    if (userId.isEmpty())
        userId = Reghaabat::instance()->userId;
    else if (userId.toInt() == 0)
        userId.clear();

    qry.prepare("insert into logs values (?, ?, ?, ?, ?, ?)");
    qry.addBindValue(table);
    qry.addBindValue(operation);
    qry.addBindValue(id);
    qry.addBindValue(data);
    qry.addBindValue(userId);
    qry.addBindValue(formatDateTime(time));
    if (! qry.exec())
        qDebug() << "log " << qry.lastError();
}

QVariantMap options()
{
    QVariantMap options;
    options["CorrectorIdentifier"] = "NameFamily";
    options["MaxMatchesInOneDay"] = 3;
    options["MaxConcurrentMatches"] = 3;

    QSqlQuery qry;
    qry.exec("select options from library limit 1");
    if (qry.next())
    {
        bool ok;
        QVariantMap data = Json::parse(qry.value(0).toString(), ok).toMap();
        if (ok)
        {
            QMapIterator<QString, QVariant> i(data);
            while (i.hasNext()) {
                i.next();
                if (options.contains(i.key()))
                    options[i.key()] = i.value();
            }
        }
    }

    return options;
}

void writeOption(QString key, QVariant value)
{
    QVariantMap opt = options();
    if (opt.contains(key))
        opt[key] = value;

    QSqlQuery qry;
    if (qry.exec(QString("update library set options = '%1'").arg(QVariantMapToString(opt))))
        insertLog("library", "update", "1");
}
