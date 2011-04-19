#include "formoperator.h"
#include "ui_formoperator.h"

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

#include <helper.h>
#include <matchrow.h>
#include <mainwindow.h>
#include <viewerform.h>

FormOperator::FormOperator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOperator)
{
    ui->setupUi(this);

    // add username edit
    eUser = new MyLineEdit("select id, firstname || ' ' || lastname as ctitle from users", this);
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
    if (ui->gDelivered->layout())
    while ((child = ui->gDelivered->layout()->takeAt(0)) != 0)
         delete child->widget();
}

void FormOperator::cancelMatch()
{
    ui->newMatchButtons->setEnabled(false);
    eMatch->setCompleter(new MyCompleter(QString("select id, title as ctitle from matches where abs(ageclass - %1) <= 1 and id not in (select match_id from answers where user_id = %2)").arg(MUsers::getAgeClass(eUser->value())).arg(eUser->value()), this));
}

void FormOperator::selectUser()
{
    cancelUser();
    if (! eUser->value().isEmpty())
    {
        ui->gDelivered->setEnabled(true);
        ui->gMatch->setEnabled(true);
        eMatch->setFocus();

        QSqlQuery qry;
        qry.exec(QString("select match_id, matches.title from answers inner join matches on answers.match_id = matches.id where user_id = %1 and received_at is null;").arg(eUser->value()));

        for (int i = 1; qry.next(); i++)
        {
            MatchRow* row = new MatchRow(eUser->value(), qry.value(0).toString(), qry.value(1).toString(), ui->gDelivered);
            ui->gDelivered->layout()->addWidget(row);
        }

        // space filler
        ui->gDelivered->layout()->addWidget(new QWidget);
    }
}

void FormOperator::selectMatch()
{
    ui->newMatchButtons->setEnabled(! eMatch->value().isEmpty());

    if (! eMatch->value().isEmpty())
        ui->bDeliver->setFocus();
}

void FormOperator::on_bDeliver_clicked()
{
    if (! eUser->value().isEmpty() && ! eMatch->value().isEmpty())
    {
        MMatches::deliver(eUser->value(), eMatch->value());
        if (ui->cPrint->isChecked())
            previewMatch(true);
        selectUser();
    }
}

void FormOperator::on_bPreview_clicked()
{
    previewMatch(false);
}

void FormOperator::previewMatch(bool print)
{
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

    ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
    viewer->showMatch(match, asks);

    if (print)
        viewer->on_bPrint_clicked();
    else
        viewer->exec();
}
