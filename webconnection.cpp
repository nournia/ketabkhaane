#include "webconnection.h"
#include "ui_webconnection.h"

#include <helper.h>
#include <uihelper.h>
#include <viewerform.h>
#include <mainwindow.h>
#include <mmatches.h>

#include <QMessageBox>
#include <QCheckBox>
#include <QAbstractItemView>

WebConnection::WebConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebConnection)
{
    ui->setupUi(this);

    receiver = new Receiver(this);
    connect(receiver, SIGNAL(received(QVariantMap)), this, SLOT(received(QVariantMap)));

    receiver->get("m=matches&o=list");
}

WebConnection::~WebConnection()
{
    delete ui;
}

void WebConnection::received(QVariantMap data)
{
    // read data
    QStringList fields;
    if (!data.keys().length()) {
        qDebug() << "Server Error";
    }
    else if (data["operation"] == "list") {
        items = new QStandardItemModel(0, 6, this);

        items->setHeaderData(0, Qt::Horizontal, "");
        items->setHeaderData(1, Qt::Horizontal, "");
        items->setHeaderData(2, Qt::Horizontal, tr("Title"));
        items->setHeaderData(3, Qt::Horizontal, tr("AgeClass"));
        items->setHeaderData(4, Qt::Horizontal, tr("Category"));
        items->setHeaderData(5, Qt::Horizontal, tr("Answers"));

        QStringList imported;
        QSqlQuery qry;
        qry.exec("select id from matches where id / 100000 != (select id from library)");
        while(qry.next())
            imported.append(qry.value(0).toString());

        QStandardItem* item;
        foreach(QVariant row, data["matches"].toList()) {
            fields = row.toStringList();
            if (imported.contains(fields[0]))
                continue;

            items->insertRow(0);
            for (int i = 0; i < 5; i++)
                items->setData(items->index(0, i+1), fields[i]);

            item = new QStandardItem();
            item->setCheckable(true);
            item->setEditable(false);
            items->setItem(0, 0, item);
        }

        ui->tImports->setModel(items);
        customizeTable(ui->tImports, 6, 80, true, 2, true);
        ui->tImports->setColumnWidth(0, 20);
        ui->tImports->setColumnHidden(1, true);
        ui->tImports->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else if (data["operation"] == "items") {
        if (!preview) {
            receiver->storeRows("authors", data["authors"]);
            receiver->storeRows("publications", data["publications"]);
            receiver->storeRows("objects", data["objects"]);
            receiver->storeRows("matches", data["matches"]);
            receiver->storeRows("questions", data["questions"]);
            receiver->storeRows("files", data["files"]);

            // download files
            foreach(QVariant row, data["files"].toList()) {
                fields = row.toStringList();
                receiver->queueUrl(QString("%2.%3").arg(fields[0], fields[1]), true);
            }

            // complete
            QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Data Imported."));
            for(int i = 0; i < items->rowCount(); i++)
                if (items->item(i, 0)->checkState())
                    items->removeRow(i);
            ui->bImport->setEnabled(true);
        } else {
            fields = data["matches"].toList()[0].toStringList();

            StrMap match;
            QList<StrPair> questions;
            match["corrector_id"] = fields[1];
            match["title"] = fields[2];
            match["ageclass"] = fields[3];
            match["object_id"] = fields[4];

            if (fields[5].isEmpty())
                questions = MMatches::extractQuestions(fields[6]);
            else {
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

    receiver->popUrl();
}

void WebConnection::on_bImport_clicked()
{
    QStringList matchIds;
    for(int i = 0; i < items->rowCount(); i++)
        if (items->item(i, 0)->checkState())
            matchIds.append(items->data(items->index(i, 1)).toString());

    preview = false;
    if (matchIds.length()) {
        receiver->get("m=matches&o=items&q="+ matchIds.join(","));
        ui->bImport->setEnabled(false);
    } else
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Please select matches."));
}

void WebConnection::on_bPreview_clicked()
{
    QModelIndex c = ui->tImports->currentIndex();
    if (c.isValid()) {
        QString matchId = items->data(c.sibling(c.row(), 1)).toString();
        preview = true;
        receiver->get("m=matches&o=items&q="+ matchId);
    }
}
