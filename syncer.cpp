#include "syncer.h"

#include <helper.h>
#include <sender.h>

Syncer::Syncer(QObject *parent)
    :QObject(parent)
{}

void Syncer::exec()
{
    QSqlQuery qry;
    qry.exec("select id, license from library");
    qry.next();
    QString id = qry.value(0).toString(), license = qry.value(1).toString();

    // Register
    if (license.isEmpty())
        registerDb();
}

void Syncer::registerDb()
{
    QMap<QString, QString> data;
    data["command"] = "register";

    Sender* sender = new Sender(this);
    sender->post(QUrl("http://reghaabat.ap01.aws.af.cm/backend.php"), data);
    connect(sender, SIGNAL(received(QString)), this, SLOT(registerFinished(QString)));
    qDebug() << "register request sent.";
}

void Syncer::registerFinished(QString response)
{
    QSqlQuery qry;
    QStringList values = response.split(",");
    if (values.length() == 2) {
        qry.exec(QString("update library set id = %1, license = \"%2\", synced_at = null").arg(values[0], values[1]));
        insertLog("library", "update", values[0]);
        qDebug() << "library registered.";
    }
}
