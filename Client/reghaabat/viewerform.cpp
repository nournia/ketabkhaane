#include "viewerform.h"
#include "ui_viewerform.h"

#include <QTextStream>
#include <QFile>
#include <QWebFrame>
#include <QWebElement>
#include <QSqlQuery>

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

void ViewerForm::showList(QString query, QStringList fields)
{
    QSqlQuery qry;
    qry.exec(query);

    loadHtml("list");
    QWebFrame *frame = ui->webView->page()->mainFrame();
    QWebElement tableDiv = frame->findFirstElement("#table");

    QString table = "<table>";

    // table head
    for (int i = 0; i < fields.count(); i++)
        table += QString("<th>%1</th>").arg(fields[i]);

    // table body
    while (qry.next())
    {
        table += "<tr>";
        for (int i = 0; i < fields.count(); i++)
            table += QString("<td>%1</td>").arg(qry.value(i).toString());
        table += "</tr>";
    }

    table += "</table>";

    tableDiv.setInnerXml(table);
}
