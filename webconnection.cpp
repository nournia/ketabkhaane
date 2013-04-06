#include "webconnection.h"
#include "ui_webconnection.h"

#include <helper.h>
#include <uihelper.h>
#include <viewerform.h>
#include <mainwindow.h>

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
        items = new QStandardItemModel(0, 5, this);

        items->setHeaderData(0, Qt::Horizontal, "");
        items->setHeaderData(1, Qt::Horizontal, "");
        items->setHeaderData(2, Qt::Horizontal, tr("Title"));
        items->setHeaderData(3, Qt::Horizontal, tr("AgeClass"));
        items->setHeaderData(4, Qt::Horizontal, tr("Category"));

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
    else if (data["operation"] == "items") {
        if (!preview) {
            storeRows("authors", data["authors"]);
            storeRows("publications", data["publications"]);
            storeRows("objects", data["objects"]);
            storeRows("matches", data["matches"]);
            storeRows("questions", data["questions"]);
            storeRows("files", data["files"]);

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

            if (fields[6].isEmpty()) {
                foreach(QVariant row, data["questions"].toList()) {
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

    receiver->popUrl();
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
