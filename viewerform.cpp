#include "viewerform.h"
#include "ui_viewerform.h"

#include <QTextStream>
#include <QFile>
#include <QSqlQuery>
#include <QFileDialog>
#include <QSettings>
#include <QProcess>
#include <QDesktopServices>
#include <QtWebKit/QWebElement>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>

#include <jalali.h>
#include <matchform.h>
#include <musers.h>

ViewerForm::ViewerForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewerForm)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("Ketabkhaane"));

    landscape = false;
    margin = true;
}

ViewerForm::~ViewerForm()
{
    delete ui;
}

void ViewerForm::loadHtml(QString name, bool _margin, bool _landscape)
{
    QFile file(QString(":/resources/%1.html").arg(name));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->webView->setHtml(QTextStream(&file).readAll());

    landscape = _landscape;
    margin = _margin;

    setMinimumHeight(600);
    if (!landscape)
        setMinimumWidth(915);
    else
        setMinimumWidth(1000);
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

void ViewerForm::showObjectLabels(QString from, QString to, bool onlyMatchObjects)
{
    loadHtml("labels", false);

    QString content, library, image;

    QSqlQuery qry;
    qry.exec("select title, image from library");
    if (qry.next())
    {
        library = qry.value(0).toString();
        image = QString("%1/%2").arg(filesUrl(), qry.value(1).toString());
    }

    content += QString("<style>span.logo { background: url(%1) no-repeat; }</style>").arg(image);

    QString sql;
    if (onlyMatchObjects) {
        sql = "select label, case matches.ageclass when 0 then '*' when 1 then '**' when 2 then '***' when 3 then '****' when 4 then '*****' end from belongs inner join matches on belongs.object_id = matches.object_id";
    } else {
        sql = "select label, '' from belongs";
        content += "<style>span.age { display: none; } span.id { margin-right: 0; }</style>";
    }

    qry.exec(QString(sql + " where label >= '%1' and label <= '%2' order by label").arg(from, to));
    int i = 0;
    for (; qry.next(); i++) {

        if (!(i % 60))
        {
            if (i) content += "</tr></tbody></table>";
            content += "<table cellspacing='0'><tbody>";
        }
        if (!(i % 4))
        {
            if (i) content += "</tr>";
            content += "<tr>";
        }

        content += QString("<td><div class='label'><span class='logo'></span><span class='library'>%1</span><span class='id'>%2</span><span class='age'>%3</span></div></td>").arg(library, qry.value(0).toString(), qry.value(1).toString());
    }

    // empty grid bug
    for (int j = 4 - i; j > 0; j--)
        content += "<td></td>";

    content += "</tr></tbody></table>";

    ui->webView->page()->mainFrame()->findFirstElement("body").setInnerXml(content);
}

void ViewerForm::showObjectList(QString from, QString to)
{
    loadHtml("objects", false, true);

    QString content;

    QSqlQuery qry;
    qry.exec(QString(
        "select belongs.label, objects.title, authors.title as author, publications.title as publication, _branches.title as branch, types.title as type "
        "from objects inner join belongs on objects.id = belongs.object_id inner join types on objects.type_id = types.id left join authors on objects.author_id = authors.id left join publications on objects.publication_id = publications.id "
        "left join (select branches.id, case when branches.title != '' then roots.title ||' - '|| branches.title else roots.title end as title from branches inner join roots on branches.root_id = roots.id) as _branches on belongs.branch_id = _branches.id "
        "where belongs.label >= '%1' and belongs.label <= '%2' order by label").arg(from, to));

    int p = 0;
    QString thead = QString("<thead><tr><th>%1</th><th>%2</th><th>%3</th><th>%4</th><th>%5</th></tr></thead>").arg(tr("Lablel"), tr("Title"), tr("Author"), tr("Publication"), tr("Branch"));
    for (int i = 0; qry.next(); i++) {
        if (!(i % 35))
        {
            if (i != 0) content += QString("</tbody></table><span class='page'>%1 %2</span>").arg(tr("Page")).arg(++p);
            content += "<table cellspacing='0'>"+ thead +"<tbody>";
        }

        content += "<tr>";
        for (int j = 0; j < 5; j++)
            content += QString("<td>%1</td>").arg(qry.value(j).toString());
        content += "</tr>";
    }
    content += QString("</tbody></table><span class='page'>%1 %2</span>").arg(tr("Page")).arg(++p);

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

    if (! match["category_id"].toString().isEmpty()) {
        qry.exec("select title from categories where id = "+ match["category_id"].toString());
        if (qry.next())
            kind = qry.value(0).toString();
        advice = tr("Your score depends on your mood, attention and genuis.");
    }
    else if (! match["object_id"].toString().isEmpty()) {
        qry.exec("select objects.type_id, types.title from objects left join types on objects.type_id = types.id where objects.id = "+ match["object_id"].toString());
        if (qry.next()) {
            kind = qry.value(1).toString();
            if (qry.value(0).toString() == "0")
                advice = tr("You can achive up to twice score for writing an abstract of book.");
            else
                advice = tr("You can achive up to twice score for writing an abstract of match content.");
        }
    }

    frame->findFirstElement("#kind").setPlainText(kind);
    frame->findFirstElement("#help").setPlainText(advice);

    // user
    if (! match["user"].toString().isEmpty()) {
        qry.exec("select firstname ||' '|| lastname from users where id = "+ match["user"].toString());
        if (qry.next())
            frame->findFirstElement("#user").setPlainText(qry.value(0).toString());
    }


    // corrector
    qry.exec("select firstname, lastname, label from users where id = "+ match["corrector_id"].toString());
    if (qry.next()) {
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

QMap<QString, QStringList> logTable(QString join, QString condition)
{
    QSqlQuery qry;
    QMap<QString, QStringList> table;

    QString sql =
        "select user_id, score, count, cast(rate*100 as int)||'%' from ("
            "select transactions.user_id, sum(transactions.score) as score, count(transactions.id) as count, round(avg(answers.rate), 2) as rate from transactions "
            "inner join matches on matches.id = substr(description,5) inner join answers on answers.user_id = transactions.user_id and answers.match_id = matches.id %1 "
            "where created_at > (select started_at from library) and transactions.score > 0 %2 group by transactions.user_id"
        ") as _t order by score desc";

    qry.exec(QString(sql).arg(join, condition));
    for (int i = 1; qry.next(); i++)
        table[qry.value(0).toString()] = QStringList() << QString("%1").arg(i) << qry.value(1).toString() << qry.value(2).toString() << qry.value(3).toString();

    return table;
}
void ViewerForm::prepareLogs()
{
    if (!log_titles.isEmpty())
        return;

    QSqlQuery qry;

    // types
    qry.exec("select * from types");
    while (qry.next()) {
        log_titles << qry.value(1).toString();
        log_tables << logTable("inner join objects on objects.id = matches.object_id", "and objects.type_id = "+ qry.value(0).toString());
    }

    // categories
    qry.exec("select * from categories");
    while (qry.next()) {
        log_titles << qry.value(1).toString();
        log_tables << logTable("", "and matches.category_id = "+ qry.value(0).toString());
    }

    // all
    log_titles << tr("All");
    log_tables << logTable("", "");
}

QString getLibraryLogo()
{
    QSqlQuery qry;
    qry.exec("select title, image from library");
    if (qry.next())
        return QString("<div class='logo'><img src='%1/%2' /><span>%3</span></div>").arg(filesUrl(), qry.value(1).toString(), qry.value(0).toString());

    return "";
}

void ViewerForm::showLogs(QString title)
{
    prepareLogs();
    loadHtml("logs");
    QString content;
    QString header = getLibraryLogo() + QString("<div class='title'>%1</div>").arg(title);
    QString tableHeader = QString("<tr><th>%1</th><th>%2</th><th>%3</th><th>%4</th><th>%5</th></tr>").arg(tr("Group"), tr("Rank"), tr("Score"), tr("Count"), tr("Quality"));

    QSqlQuery qry;

    qry.exec(QString(
        "select transactions.user_id as id, firstname||' '||lastname as name, ageclasses.title as ageclass, %1 as ageclass_id, sum(transactions.score) as score from transactions "
        "inner join users on transactions.user_id = users.id inner join ageclasses on ageclass_id = ageclasses.id "
        "inner join matches on matches.id = substr(transactions.description,5) inner join answers on answers.user_id = transactions.user_id and answers.match_id = matches.id "
        "where created_at > (select started_at from library) and transactions.score > 0 group by transactions.user_id order by score desc"
    ).arg(MUsers::getAgeClassCase("current_date")));

    // iterate users in all table
    while (qry.next()) {
        content += "<article>";
        content += header;
        content += QString("<div class='user'><p class='name'><span>%2</span></p><p class='ageclass'>%3 <span>%4</span></p></div>").arg(qry.value(1).toString(), tr("Class"), qry.value(2).toString());

        QString rows, user_id = qry.value(0).toString();
        for (int i = 0; i < log_tables.length(); i++)
            if (log_tables[i].contains(user_id)) {
                QStringList row = log_tables[i][user_id];
                rows += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>").arg(log_titles[i], row[0], row[1], row[2], row[3]);
            }

        content += QString("<table cellspacing='0'><thead>%2</thead><tbody>%3</tbody></table>").arg(tableHeader, rows);
        content += "</article>";
    }

    ui->webView->page()->mainFrame()->findFirstElement("body").setInnerXml(content);
}

void ViewerForm::savePdf(QString filename)
{
    // wkhtmltopdf
    QString orientationStr = landscape ? "-O Landscape" : "-O Portrait";
    QString marginStr = margin ? "-L 10 -T 10 -R 10 -B 10" : "-L 4 -T 8 -R 4 -B 8";

    QProcess* p = new QProcess(this);
    p->start(QString("wkhtmltopdf.exe -q %3 %4 %1 %2").arg("p.html", filename, orientationStr, marginStr));
    p->waitForFinished();

/*
    QPrinter printer(QPrinterInfo::defaultPrinter());
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);

    if (!landscape)
        printer.setOrientation(QPrinter::Portrait);
    else
        printer.setOrientation(QPrinter::Landscape);

    if (margin)
        printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    else
        printer.setPageMargins(8, 4, 8, 4, QPrinter::Millimeter);

    ui->webView->print(&printer);
*/
}

void ViewerForm::on_bPdf_clicked()
{
    QFile file("p.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec( "UTF-8" );
        out << ui->webView->page()->mainFrame()->toHtml();
        file.close();
    }

    QString filename = QFileDialog::getSaveFileName(this, "");
    if (!filename.isEmpty())
    {
        if (!filename.endsWith(".pdf"))
            filename += ".pdf";
        savePdf(filename);

        QDesktopServices::openUrl(QUrl(filename));
    }
}

void ViewerForm::on_bPrint_clicked()
{
    QString tmpFile = "p";

    savePdf(tmpFile);

    QString printer = App::instance()->settings->value("Printer", "").toString();

    if (printer.isEmpty())
        printer = "-print-to-default";
    else
        printer = QString("-print-to \"%1\"").arg(printer);

    QProcess* p = new QProcess(this);
    p->start(QString("sumatra.exe -reuse-instance %1 %2").arg(printer).arg(tmpFile));
}
