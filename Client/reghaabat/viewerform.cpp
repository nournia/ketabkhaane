#include "viewerform.h"
#include "ui_viewerform.h"

#include <QTextStream>
#include <QFile>
#include <QWebFrame>
#include <QWebElement>
#include <QSqlQuery>
#include <QPrinter>
#include <QPrinterInfo>

ViewerForm::ViewerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewerForm)
{
    ui->setupUi(this);
}

ViewerForm::~ViewerForm()
{
    delete ui;
}

void ViewerForm::loadHtml(QString name)
{
    QFile file(QString("../%1.html").arg(name));
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
//        qDebug() << "sql file not found";
//        return false;
    }
    ui->webView->setHtml(QTextStream(&file).readAll());
}

void ViewerForm::showList(QString title, QStringList fields, QString query)
{
    QSqlQuery qry;
    qry.exec(query);

    loadHtml("list");
    QWebFrame *frame = ui->webView->page()->mainFrame();

    QString table = "<table>";

    // table head
    for (int i = 0; i < fields.count(); i++)
        table += QString("<th>%1</th>").arg(fields[i]);

    // table body
    for (int c = 0; qry.next(); c++)
    {
        table += "<tr>";
        for (int i = 0; i < fields.count(); i++)
            table += QString("<td>%1</td>").arg(qry.value(i).toString());
        table += "</tr>";
    }

    table += "</table>";

    frame->findFirstElement("#title").setPlainText(title);
    frame->findFirstElement("#table").setInnerXml(table);
}

void ViewerForm::on_pushButton_clicked()
{
    QPrinter printer(QPrinterInfo::defaultPrinter());
    printer.setCreator("Foo Bar");
    printer.setDocName("Foo Bar");
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    ui->webView->print(&printer);
}
