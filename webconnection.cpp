#include "webconnection.h"
#include "ui_webconnection.h"

#include <helper.h>
#include <sender.h>

#include <QCryptographicHash>

WebConnection::WebConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebConnection)
{
    ui->setupUi(this);
    syncer = new Syncer(this);
}

WebConnection::~WebConnection()
{
    delete ui;
}

void WebConnection::on_bSync_clicked()
{
    static Sender sender(this);

    QString url = "http://127.0.0.1/reghaabat-server/backend.php";

    QSqlQuery qry;
    qry.exec("select id, license, synced_at from library");
    qry.next();

    QDateTime syncTime;
    QStringList logs, files;
    QMap<QString, QString> posts;

    if (qry.value(1).toString().isEmpty())
        posts["command"] = "register";
    else {
        posts["id"] = qry.value(0).toString();
        posts["key"] = QCryptographicHash::hash(QString(qry.value(0).toString() + "|x|" + qry.value(1).toString()).toUtf8(), QCryptographicHash::Sha1).toHex();
        posts["command"] = "store";
        posts["finished"] = syncer->getRecordsAndFiles(syncTime, logs, files);
        posts["synced_at"] = formatDateTime(syncTime);
        posts["count"] = QString("%1").arg(logs.length());
        posts["logs"] = logs.join("|-|");
    }

    if (posts["command"] == "register" || posts["count"].toInt() > 0)
        sender.send(url, posts, files);
}
