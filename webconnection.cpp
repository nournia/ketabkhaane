#include "webconnection.h"
#include "ui_webconnection.h"

#include <helper.h>
#include <uihelper.h>
#include <viewerform.h>
#include <mainwindow.h>

#include <QMessageBox>
#include <QCheckBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAbstractItemView>
#include <QFile>

WebConnection::WebConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebConnection)
{
    ui->setupUi(this);

    queueUrl("m=matches&o=list"); popUrl();
}

WebConnection::~WebConnection()
{
    delete ui;
}

void WebConnection::queueUrl(QString args, bool file)
{
    QString url = Reghaabat::instance()->serverUrl;
    if (file)
        url += "files.php?q=";
    else
        url += "data.php?i="+ Reghaabat::instance()->libraryId +"&";
    queue.append(url + args);
}

void WebConnection::popUrl()
{
    if (queue.length() > 0) {
        QNetworkRequest request(QUrl(queue.takeFirst()));
        reply = qnam.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(receive()));
    }
}

void WebConnection::receive()
{
    QString url = reply->url().toString();
    QString fileId = "files.php?q=";

    // save file
    if (url.indexOf(fileId) > 0) {
        QString filename = url.mid(url.indexOf(fileId)+fileId.length());
        QFile file(QString("%1/files/%2").arg(dataFolder()).arg(filename));
        if (file.open(QIODevice::WriteOnly))
            file.write(reply->readAll());

        popUrl();
        return;
    }

    // read data
    QString data = reply->readAll();
    QStringList fields;
    QVariantMap response = QJsonDocument::fromJson(data.toUtf8()).object().toVariantMap();
    if (!response.keys().length()) {
        qDebug() << "Server Error";
    }
    else if (response["operation"] == "list") {
        items = new QStandardItemModel(0, 5, this);

        items->setHeaderData(0, Qt::Horizontal, "");
        items->setHeaderData(1, Qt::Horizontal, "");
        items->setHeaderData(2, Qt::Horizontal, tr("Title"));
        items->setHeaderData(3, Qt::Horizontal, tr("AgeClass"));
        items->setHeaderData(4, Qt::Horizontal, tr("Category"));

        QStandardItem* item;
        foreach(QVariant row, response["matches"].toList()) {
            items->insertRow(0);
            fields = row.toStringList();
            for (int i = 0; i < 4; i++)
                items->setData(items->index(0, i+1), fields[i]);

            item = new QStandardItem();
            item->setCheckable(true);
            item->setEditable(false);
            items->setItem(0, 0, item);
        }

        ui->tImports->setModel(items);
        customizeTable(ui->tImports, 5, 80, true, 2, true);
        ui->tImports->setColumnWidth(0, 20);
        ui->tImports->setColumnHidden(1, true);
        ui->tImports->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else if (response["operation"] == "items") {
        if (!preview) {
            storeRows("authors", response["authors"]);
            storeRows("publications", response["publications"]);
            storeRows("objects", response["objects"]);
            storeRows("matches", response["matches"]);
            storeRows("questions", response["questions"]);
            storeRows("files", response["files"]);

            // download files
            foreach(QVariant row, response["files"].toList()) {
                fields = row.toStringList();
                queueUrl(QString("%2.%3").arg(fields[0], fields[1]), true);
            }

        } else {
            fields = response["matches"].toList()[0].toStringList();

            StrMap match;
            QList<StrPair> questions;
            match["corrector_id"] = fields[1];
            match["title"] = fields[2];
            match["ageclass"] = fields[3];
            match["object_id"] = fields[4];

            if (fields[6].isEmpty()) {
                foreach(QVariant row, response["questions"].toList()) {
                    fields = row.toStringList();
                    questions.append(qMakePair(fields[2], fields[3]));
                }
            } else {
                match["category_id"] = fields[5];
                QString html = fields[6];
                html.replace("src=\"", "src=\""+ Reghaabat::instance()->serverUrl +"files.php?q=");
                match["content"] = html;
            }

            ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
            viewer->showMatch(match, questions);
            viewer->exec();
        }
    }

    popUrl();
}

void WebConnection::storeRows(QString table, QVariant rows)
{
    QSqlQuery qry;
    QStringList marks;
    QVariantList fields;

    foreach(QVariant row, rows.toList()) {
        fields = row.toList();
        marks.clear();
        for(int i = 0; i < fields.length(); i++)
            marks.append("?");

        qry.prepare(QString("insert into %1 values (%2)").arg(table, marks.join(",")));

        foreach(QVariant field, fields)
            qry.addBindValue(field);

        if (qry.exec()) {
            // supports and belongs table
            if (table == "matches") {
                if (qry.exec(QString("insert into supports (match_id, corrector_id, current_state, score) values (%1, %2, 'imported', 0)").arg(fields[0].toString(), Reghaabat::instance()->userId)))
                    insertLog("supports", "insert", qry.lastInsertId());
            } else if (table == "objects") {
                if (qry.exec(QString("insert into belongs (object_id) values (%1)").arg(fields[0].toString())))
                    insertLog("belongs", "insert", qry.lastInsertId());
            }
        }
    }
}

void WebConnection::on_bImport_clicked()
{
    QStringList matchIds;
    for(int i = 0; i < items->rowCount(); i++)
        if (items->item(i, 0)->checkState())
            matchIds.append(items->data(items->index(i, 1)).toString());

    preview = false;
    if (matchIds.length())
        queueUrl("m=matches&o=items&q="+ matchIds.join(","));
    popUrl();
}

void WebConnection::on_bPreview_clicked()
{
    QModelIndex c = ui->tImports->currentIndex();
    if (c.isValid()) {
        QString matchId = items->data(c.sibling(c.row(), 1)).toString();
        preview = true;
        queueUrl("m=matches&o=items&q="+ matchId);
    }
    popUrl();
}
