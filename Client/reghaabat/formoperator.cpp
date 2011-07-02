#include "formoperator.h"
#include "ui_formoperator.h"

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include <helper.h>
#include <matchrow.h>
#include <mainwindow.h>
#include <jalali.h>

FormOperator::FormOperator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOperator)
{
    ui->setupUi(this);

    viewer = new ViewerForm((MainWindow*) parent);

    // add username edit
    QString condition;
    if (!Reghaabat::hasAccess("manager"))
        condition = QString(" where gender = '%1'").arg(Reghaabat::instance()->userGender);
    eUser = new MyLineEdit("select id as cid, id as clabel, firstname||' '||lastname as ctitle from users" + condition, this);
    ui->lUser->addWidget(eUser);
    connect(eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    // add matchname edit
    eMatch = new MyLineEdit("", this);
    ui->lMatch->addWidget(eMatch);
    QWidget::setTabOrder(eMatch, ui->bDeliver);
    QWidget::setTabOrder(ui->bDeliver, ui->cPrint);
    QWidget::setTabOrder(ui->cPrint, ui->bPreview);

    connect(eMatch, SIGNAL(select()), this, SLOT(selectMatch()));
    connect(eMatch, SIGNAL(cancel()), this, SLOT(cancelMatch()));

    cancelUser();
}

FormOperator::~FormOperator()
{
    delete ui;
}

void FormOperator::cancelUser()
{
    ui->gDelivered->setEnabled(false);
    ui->gMatch->setEnabled(false);

    cancelMatch();
    eMatch->setText("");

    // clean gDelivered
    QLayoutItem *child;
    while ((child = ui->lDelivered->layout()->takeAt(0)) != 0)
         delete child->widget();
}

void FormOperator::cancelMatch()
{
    ui->bDeliver->setEnabled(false);
    ui->bPreview->setEnabled(false);
}

void FormOperator::selectUser()
{
    cancelUser();
    if (! eUser->value().isEmpty())
    {
        QSqlQuery qry;
        qry.exec(QString("select match_id, matches.title from answers inner join matches on answers.match_id = matches.id where user_id = %1 and received_at is null and answers.delivered_at > (select started_at from library)").arg(eUser->value()));

        for (int i = 1; qry.next(); i++)
        {
            MatchRow* row = new MatchRow(eUser->value(), qry.value(0).toString(), qry.value(1).toString(), ui->gDelivered);
            ui->lDelivered->layout()->addWidget(row);
        }

        // space filler
        ui->lDelivered->layout()->addWidget(new QWidget);

        ui->gDelivered->setEnabled(true);
        ui->gMatch->setEnabled(true);

        QString query;
        if (ui->cQuickSearch->isChecked())
            query = QString("select matches.id as cid, objects.label as clabel, matches.title as ctitle from matches left join objects on matches.resource_id = objects.resource_id inner join supports on matches.id = supports.match_id where supports.current_state = 'active' and abs(ageclass - %1) <= 1 and matches.id not in (select match_id from answers where user_id = %2)").arg(MUsers::getAgeClass(eUser->value())).arg(eUser->value());
        else
            query = "select matches.id as cid, objects.label as clabel, matches.title as ctitle from matches left join objects on matches.resource_id = objects.resource_id";
        eMatch->setQuery(query);
        eMatch->setFocus();
    }
}

void FormOperator::selectMatch()
{
    ui->bDeliver->setEnabled(! eMatch->value().isEmpty());
    ui->bPreview->setEnabled(! eMatch->value().isEmpty());

    if (! eMatch->value().isEmpty())
    {
        ui->bDeliver->setFocus();
        prepareViewer();
    }
}

void FormOperator::on_bDeliver_clicked()
{
    if (! eUser->value().isEmpty() && ! eMatch->value().isEmpty())
    {
        QString msg;

        QSqlQuery qry;
        qry.exec(QString("select count(match_id) from answers where user_id = %1 and received_at is null and answers.delivered_at > (select started_at from library)").arg(eUser->value()));
        if (qry.next() && qry.value(0).toInt() >= 3)
            msg = QObject::tr("You received enough matches at the moment.");

        if (msg.isEmpty())
            msg = MMatches::deliver(eUser->value(), eMatch->value());

        if (! msg.isEmpty())
        {
            QMessageBox::warning(0, QObject::tr("Reghaabat"), msg);
            return;
        }

        if (ui->cPrint->isChecked())
            viewer->on_bPrint_clicked();

        // deliver book in library
        bool ok; QSqlDatabase library = Connector::connectLibrary(ok);
        if (ok)
        {
            QSqlQuery qryObj;
            qryObj.exec("select label from matches inner join objects on matches.resource_id = objects.resource_id where matches.id = "+ eMatch->value());

            if (qryObj.next())
            {
                QSqlQuery qry(library);
                qry.prepare("insert into cash values (?, ?, ?, null, ?, null)");
                qry.addBindValue(eUser->value());
                qry.addBindValue(qryObj.value(0).toString());
                qry.addBindValue(toJalali(QDate::currentDate()));
                qry.addBindValue(Reghaabat::instance()->userId);
                qry.exec();
            }
        }

        selectUser();
    }
}

void FormOperator::on_bPreview_clicked()
{
    viewer->exec();
}

void FormOperator::prepareViewer()
{
    // don't perpare match again
    static QString prepared;
    if (prepared == eMatch->value())
        return;

    StrMap match;
    QList<StrPair> questions, asks;
    MMatches::get(eMatch->value(), match, questions);
    match["user"] = eUser->value();

    QSqlQuery qry;

    // corrector
    qry.exec(QString("select id from users where firstname ||' '|| lastname = '%1'").arg(match["corrector"].toString()));
    if (qry.next())
        match["corrector"] = qry.value(0).toString();

    // select random questions
    int qs = 4;
    qry.exec("select questions from ageclasses where id = "+ match["ageclass"].toString());
    if (qry.next())
        qs = qry.value(0).toInt();

    if (questions.count() < qs)
        qs = questions.count();

    int* selected = new int[qs];
    for (int i = 0; i < qs;)
    {
        int n = rand() % questions.count();

        bool exists = false;
        for (int j = 0; j < i; j++)
            if (selected[j] == n) { exists = true; break; }

        if (! exists)
        {
            asks.append(qMakePair(questions[n].first, QString("")));
            selected[i] = n; i++;
        }
    }

    viewer->showMatch(match, asks);

    prepared = eMatch->value();
}

void FormOperator::on_cQuickSearch_clicked()
{
    selectUser();
}
