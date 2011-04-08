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

    QString table = "<table cellspacing='0'>";

    // table head
    table += "<thead><tr>";
    for (int i = 0; i < fields.count(); i++)
        table += QString("<th>%1</th>").arg(fields[i]);
    table += "</tr></thead>";

    // table body
    table += "<tbody>";
    for (int c = 1; qry.next(); c++)
    {
        table += "<tr>";

        if (fields[0] == ViewerForm::tr("Rank"))
            table += QString("<td>%1</td>").arg(c);

        for (int i = 0; i < fields.count()-1; i++)
            table += QString("<td>%1</td>").arg(qry.value(i).toString());

        table += "</tr>";
    }
    table += "</tbody>";

    table += "</table>";

    frame->findFirstElement("header").setPlainText(title);
    frame->findFirstElement("article").setInnerXml(table);
}

void ViewerForm::on_pushButton_clicked()
{
    QFile file("res.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec( "UTF-8" );
        out << ui->webView->page()->mainFrame()->toHtml();
        file.close();
    }

    QPrinter printer(QPrinterInfo::defaultPrinter());
    printer.setDocName("List");
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    ui->webView->print(&printer);
}
