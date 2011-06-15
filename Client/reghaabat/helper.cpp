#include <helper.h>

#include <QCoreApplication>

QString getAbsoluteAddress(QString address)
{
    return QString(QCoreApplication::applicationDirPath() + "/" + address).replace("/", "\\");
}

QString c1 = QString::fromUtf8("ي"), r1 = QString::fromUtf8("ی");
QString c2 = QString::fromUtf8("ك"), r2 = QString::fromUtf8("ک");

QString refineText(QString text)
{
    // replace specific charcters
    return text.replace(c1, r1).replace(c2, r2);
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
