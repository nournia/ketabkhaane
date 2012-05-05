#include "viewerform.h"
#include "ui_viewerform.h"

#include <QTextStream>
#include <QFile>
#include <QWebFrame>
#include <QWebElement>
#include <QSqlQuery>
#include <QPrinter>
#include <QPrinterInfo>
#include <QFileDialog>
#include <QSettings>
#include <QProcess>

#include <jalali.h>
#include <matchform.h>

ViewerForm::ViewerForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewerForm)
{
    ui->setupUi(this);
    setMinimumWidth(915);
    setMinimumHeight(600);
}

ViewerForm::~ViewerForm()
{
    delete ui;
}

void ViewerForm::loadHtml(QString name)
{
    QFile file(QString(":/resources/%1.html").arg(name));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->webView->setHtml(QTextStream(&file).readAll());
}

QString ViewerForm::addTable(QString title, QStringList fields, QString query)
{
    QString content = QString("<header>%1</header>").arg(title);

    QSqlQuery qry;
    qry.exec(query);

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
        {
            table += QString("<td>%1</td>").arg(c);
            for (int i = 0; i < fields.count()-1; i++)
                table += QString("<td>%1</td>").arg(qry.value(i).toString());
        } else
            for (int i = 0; i < fields.count(); i++)
                table += QString("<td>%1</td>").arg(qry.value(i).toString());

        table += "</tr>";
    }
    table += "</tbody>";

    table += "</table>";


    table += "<style>";

    int fill = fields[0] == ViewerForm::tr("Rank") ? 2 : 1;
    table += QString("th:nth-child(%1), td:nth-child(%1) {text-align: right; width: auto;}").arg(fill);

    if (fields[0] == ViewerForm::tr("Rank"))
        table += "th:first-child, td:first-child {padding-left: 10px; width: 20px;}";
    table += "</style>";

    content += QString("<article>%1</article>").arg(table);

    return content;
}

QString getScoreListQuery(QString condition)
{
    return QString("select firstname ||' '|| lastname, corrected, sscore from users inner join (select user_id, sum(score) as sscore from transactions where substr(description, 1, 3) = 'mid' and created_at > (select started_at from library) group by user_id) as _scores on users.id = _scores.user_id inner join (select user_id, count(id) as corrected from answers where corrected_at > (select started_at from library) group by user_id) as _corrected on users.id =_corrected.user_id where sscore > 0 %1 order by sscore desc").arg(condition);
}

QString getMatchListQuery(QString ageclass)
{
    QString tField, tTable, tCondition;
    if (ageclass.isEmpty())
    {
        tField = "ageclasses.title,";
        tTable = "inner join ageclasses on matches.ageclass = ageclasses.id";
    } else
        tCondition = "and matches.ageclass = " + ageclass;

    return QString("select matches.title, %1 ifnull(categories.title, types.title) as kind, supports.score from matches inner join supports on matches.id = supports.match_id %2 left join categories on categories.id = matches.category_id left join objects on matches.object_id = objects.id left join types on objects.type_id = types.id where supports.current_state = 'active' %3 order by kind, supports.score desc, matches.title").arg(tField).arg(tTable).arg(tCondition);
}

void ViewerForm::bUserAll()
{
    loadHtml("list");

    QString content = addTable(tr("Score List"), QStringList() << tr("Rank") << tr("Name") << tr("Matches") << tr("Score"), getScoreListQuery(""));

    QWebFrame *frame = ui->webView->page()->mainFrame();
    frame->findFirstElement("body").setInnerXml(content);
}

void ViewerForm::bUserGenderGroup()
{
    loadHtml("list");

    QString content;
    QStringList fields = QStringList() << tr("Rank") << tr("Name") << tr("Matches") << tr("Score");
    content += addTable(tr("Men Score List"), fields, getScoreListQuery("and users.gender = 'male'"));
    content += addTable(tr("Women Score List"), fields, getScoreListQuery("and users.gender = 'female'"));

    QWebFrame *frame = ui->webView->page()->mainFrame();
    frame->findFirstElement("body").setInnerXml(content);
}

void ViewerForm::bMatchAll()
{
    loadHtml("list");

    QString content = addTable(tr("Match List"), QStringList() << tr("Title") << tr("AgeClass") << tr("Kind") << tr("Score"), getMatchListQuery(""));

    QWebFrame *frame = ui->webView->page()->mainFrame();
    frame->findFirstElement("body").setInnerXml(content);
}


void ViewerForm::bMatchAgeGroup()
{
    loadHtml("list");
    QString content;

    QSqlQuery qry; qry.exec("select id, description from ageclasses");
    while (qry.next())
        content += addTable(tr("Match List") + " - " + tr("Group") + " " + qry.value(1).toString(), QStringList() << tr("Title") << tr("Kind") << tr("Score"), getMatchListQuery(qry.value(0).toString()));

    ui->webView->page()->mainFrame()->findFirstElement("body").setInnerXml(content);
}

void ViewerForm::showMatch(StrMap match, QList<StrPair> questions)
{
    setWindowTitle(tr("Preview"));

    loadHtml("match");
    QWebFrame* frame = ui->webView->page()->mainFrame();
    frame->findFirstElement("#title").setPlainText(match["title"].toString());
    frame->findFirstElement("#date").setPlainText(toJalali(QDate::currentDate()));

    QString content, evaluations, choices;
    //choices = QString("<span class='choice'>%1</span><span class='choice'>%2</span><span class='choice'>%3</span><span class='choice'>%4</span>").arg(tr("Grade1")).arg(tr("Grade2")).arg(tr("Grade3")).arg(tr("Grade4"));

    if (questions.count() > 0)
        for (int i = 0; i < questions.count() + 1; i++)
        {
            if (i < questions.count())
            {
                if (! questions[i].second.isEmpty()) questions[i].second += "<hr />";
                content += QString("<p><span>%1.<span> %2<br />%3</p>").arg(i+1).arg(questions[i].first).arg(questions[i].second);
                //evaluations += QString("<div class='question'><span class='index'>%1 %2</span>%3</div>").arg(tr("Question")).arg(i+1).arg(choices);
            } else
            {
                //evaluations += QString("<div class='quality'><span class='index'>%1</span>%2</div>").arg(tr("Abstract")).arg(choices);
            }
        }
    else
    {
        content = match["content"].toString();
        //evaluations += QString("<div class='quality'><span class='index'>%1</span>%2</div>").arg(tr("Quality")).arg(choices);
    }

    frame->findFirstElement("#questions").setInnerXml(content);
    //frame->findFirstElement("#evaluations").appendInside(evaluations);


    QSqlQuery qry;


    // kind
    QString kind, advice;

    if (! match["category_id"].toString().isEmpty())
    {
        qry.exec("select title from categories where id = "+ match["category_id"].toString());
        if (qry.next())
            kind = qry.value(0).toString();
        advice = tr("Your score depends on your mood, attention and genuis.");
    }
    else if (! match["object_id"].toString().isEmpty())
    {
        qry.exec("select objects.type_id, types.title from objects left join types on objects.type_id = types.id where objects.id = "+ match["object_id"].toString());
        if (qry.next())
            kind = qry.value(1).toString();

        if (qry.value(0).toString() == "0")
            advice = tr("You can achive up to twice score for writing an abstract of book.");
        else
            advice = tr("You can achive up to twice score for writing an abstract of match content.");
    }

    frame->findFirstElement("#kind").setPlainText(kind);
    frame->findFirstElement("#help").setPlainText(advice);


    // user
    if (! match["user"].toString().isEmpty())
    {
        qry.exec("select firstname ||' '|| lastname from users where id = "+ match["user"].toString());
        if (qry.next())
            frame->findFirstElement("#user").setPlainText(qry.value(0).toString());
    }


    // corrector
    qry.exec("select firstname, lastname, label from users where id = "+ match["corrector_id"].toString());
    if (qry.next())
    {
        QString corrector;
        QString correctorPrint = options()["CorrectorIdentifier"].toString();

        if (correctorPrint == "NameFamily")
            corrector = qry.value(0).toString() + " " + qry.value(1).toString();
        else if (correctorPrint == "Family")
            corrector = qry.value(1).toString();
        else if (correctorPrint == "Label")
            corrector = corrector = qry.value(2).toString();

        frame->findFirstElement("#corrector").setPlainText(corrector);
    }

    // score
    frame->findFirstElement("#score").setPlainText(match["score"].toString());


    // library
    qry.exec("select title, image from library");
    qry.next();
    if (! qry.value(1).toString().isEmpty())
    {
        QString library;
        library += QString("<img src='%1/%2' />").arg(filesUrl()).arg(qry.value(1).toString());
        library += QString("<span>%1</span>").arg(qry.value(0).toString());
        frame->findFirstElement("#library").setInnerXml(library);
    }

}

void ViewerForm::savePdf(QString filename)
{
    QPrinter printer(QPrinterInfo::defaultPrinter());
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    printer.setOutputFileName(filename);
    ui->webView->print(&printer);
}

void ViewerForm::on_bPdf_clicked()
{
    QFile file("print.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec( "UTF-8" );
        out << ui->webView->page()->mainFrame()->toHtml();
        file.close();
    }

    QString filename = QFileDialog::getSaveFileName(this, "");
    if (!filename.endsWith(".pdf"))
        filename += ".pdf";
    savePdf(filename);
}

void ViewerForm::on_bPrint_clicked()
{
    QSettings settings("Sobhe", "Reghaabat");
    QString tmpFile = "p";

    savePdf(tmpFile);

    QString printer = settings.value("Printer", "").toString();

    if (printer.isEmpty())
        printer = "-print-to-default";
    else
        printer = QString("-print-to \"%1\"").arg(printer);

    QProcess* p = new QProcess(this);
    p->start(QString("sumatra.exe -reuse-instance %1 %2").arg(printer).arg(tmpFile));
}