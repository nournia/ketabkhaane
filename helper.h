#ifndef HELPER_H
#define HELPER_H

#include <QVariant>
#include <QMap>
#include <QStringList>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDateTime>
#include <QDebug>

// types

typedef QMap<QString, QVariant> StrMap;
typedef QPair<QString, QString> StrPair;

// refine

QString QVariantMapToString(QVariantMap& data);
QString getAbsoluteAddress(QString address);

// constants

QString dataFolder();
QString filesUrl();


// db

StrMap getRecord(QSqlQuery& query);
QString getReplaceQuery(QString table, StrMap data, QString id);
QVariant insertTitleEntry(QString table, QString title);
void insertLog(QString table, QString operation, QVariant id, QString userId = "", QDateTime time = QDateTime::currentDateTime());

// files

QString getInAppFilename(QString filename);
void removeInAppFile(QString filename);

// options

QVariantMap options();
void writeOption(QString key, QVariant value);


// inlines

inline QString refineText(QString text)
{
    return text.replace(QString::fromUtf8("ي"), QString::fromUtf8("ی"))
               .replace(QString::fromUtf8("ك"), QString::fromUtf8("ک"));
}

inline QString formatDate(QDate date)
{
    return date.toString("yyyy-MM-dd");
}

inline QString formatDateTime(QDateTime time)
{
    return time.toString("yyyy-MM-dd hh:mm:ss");
}

// from qjson library
inline QString sanitizeString( QString str )
{
    str.replace('&', ','); // must change !!!
    str.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
    str.replace( QLatin1String( "\"" ), QLatin1String( "\\\"" ) );
    str.replace( QLatin1String( "\b" ), QLatin1String( "\\b" ) );
    str.replace( QLatin1String( "\f" ), QLatin1String( "\\f" ) );
    str.replace( QLatin1String( "\n" ), QLatin1String( "\\n" ) );
    str.replace( QLatin1String( "\r" ), QLatin1String( "\\r" ) );
    str.replace( QLatin1String( "\t" ), QLatin1String( "\\t" ) );
    return QString( QLatin1String( "\"%1\"" ) ).arg( str );
}
inline QString getJsonValue(QVariant v)
{
    if (v.isNull())
        return "null";
    else {
        bool ok; int i = v.toInt(&ok);
        if (ok)
            return QString::number(i);
        else
            return sanitizeString(v.toString());
    }
}
inline QString getRecordJSON(QSqlQuery& qry)
{
    int cols = qry.record().count();
    QString json;
    bool firstCol = true;
    for (int i = 1; i < cols; i++)
    {
        if (! firstCol) json += ','; else firstCol = false;
        json += getJsonValue(qry.value(i).toString());
    }
    return json;
}

#include <syncer.h>

// rghaabat global variables
// from: http://stackoverflow.com/questions/3747085/global-variables-in-qt
class Reghaabat
{
private:
    static Reghaabat* m_Instance;

public:
    QString userId, userName, userGender, userPermission, tmpId;
    Syncer* syncer;

    static Reghaabat* instance()
    {
        if (! m_Instance)
            m_Instance = new Reghaabat;
        return m_Instance;
    }

    static bool hasAccess(QString limit)
    {
        QString permission = Reghaabat::instance()->userPermission;
        QStringList permissions = QStringList() << "user" << "operator" << "designer" << "manager" << "master";

        if (permission == "designer" && limit == "operator")
            return false;

        return permissions.indexOf(permission) >= permissions.indexOf(limit);
    }
};

#endif // HELPER_H
