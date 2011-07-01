#include <helper.h>

#include <QCoreApplication>
#include <QSettings>
#include <QDebug>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

QString getAbsoluteAddress(QString address)
{
    return QCoreApplication::applicationDirPath() + "/" + address;
}

QString dataFolder()
{
    QSettings settings("Rooyesh", "Reghaabat");
    return settings.value("DataFolder", getAbsoluteAddress("data")).toString();
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

void insertLog(QString table, QString operation, QVariant id, QString userId, QDateTime time)
{
    QString data;

    QSqlQuery qry;
    qry.exec(QString("select * from %1 where id = %2").arg(table).arg(id.toString()));
    if (qry.next())
        data = getRecordJSON(qry);

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
    qry.addBindValue(time);
    if (! qry.exec())
        qDebug() << "log " << qry.lastError();
}
