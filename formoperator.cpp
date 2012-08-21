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
#include <musers.h>
#include <mmatches.h>
#include <mobjects.h>


FormOperator::FormOperator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOperator)
{
    ui->setupUi(this);

    viewer = new ViewerForm((MainWindow*) parent);

    ui->eUser->setQuery(MUsers::getUsersQuery());
    connect(ui->eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(ui->eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    QString query;
    if (! options()["Match"].toBool())
        query = "select id as cid, label as clabel, substr(title, 0, 80) as ctitle from objects";
    else
        query = "select cid, clabel, substr(ctitle, 0, 80) as ctitle from (select id as cid, label as clabel, title as ctitle from objects union "
                "select 'x'||matches.id as cid, categories.title as clabel, matches.title as ctitle from matches inner join categories on matches.category_id = categories.id) as _t";

    ui->eObject->setQuery(query);

    connect(ui->eObject, SIGNAL(select()), this, SLOT(selectObject()));
    connect(ui->eObject, SIGNAL(cancel()), this, SLOT(cancelObject()));

    cancelUser();
}

FormOperator::~FormOperator()
{
    delete ui;
}

void FormOperator::cancelUser()
{
    ui->gDeliver->setEnabled(false);
    ui->gReceive->setEnabled(false);

    cancelObject();
    ui->eObject->setText("");

    ui->lFine->setText("0");
    ui->lDebt->setText("0");
    ui->sPayment->setValue(0);
    ui->sDiscount->setValue(0);

    // clean lObjects
    QLayoutItem *child;
    while ((child = ui->lObjects->layout()->takeAt(0)) != 0)
         delete child->widget();
}

void FormOperator::cancelObject()
{
    ui->bDeliver->setEnabled(false);
    ui->bPreview->setEnabled(false);

    int height = 60;
    ui->gMatchOpt->setVisible(false);
    ui->gDeliver->setMaximumHeight(height);
    ui->gDeliver->setMinimumHeight(height);

    ui->lMatchError->setText("");
}

void FormOperator::selectUser()
{
    cancelUser();
    if (! ui->eUser->value().isEmpty())
    {
        bool receive = false;
        QStringList matchObjects;
        QSqlQuery qry;
        MatchRow* row;
        int fine;

        // show delivered matches
        qry.exec(QString("select match_id, matches.title, matches.object_id, objects.label from answers inner join matches on answers.match_id = matches.id left join objects on matches.object_id = objects.id where user_id = %1 and received_at is null and answers.delivered_at > (select started_at from library)").arg(ui->eUser->value()));
        for (int i = 1; qry.next(); i++)
        {
            fine = qry.value(2).toString().isEmpty() ? 0 : MObjects::getFine(ui->eUser->value(), qry.value(2).toString());
            row = new MatchRow(qry.value(1).toString(), qry.value(3).toString(), QStringList() << "" << tr("Received"), qry.value(0).toString(), qry.value(2).toString(), fine, ui->gObjects);
            ui->lObjects->layout()->addWidget(row);
            matchObjects << qry.value(2).toString();
            connect(row, SIGNAL(changed()), this, SLOT(refreshFine()));
            receive = true;
        }

        // show delivered objects
        qry.exec(QString("select object_id, objects.title, objects.label from borrows inner join objects on borrows.object_id = objects.id where user_id = %1 and received_at is null").arg(ui->eUser->value()));
        for (int i = 1; qry.next(); i++)
        if (! matchObjects.contains(qry.value(0).toString()))
        {
            fine = MObjects::getFine(ui->eUser->value(), qry.value(0).toString());
            row = new MatchRow(qry.value(1).toString(), qry.value(2).toString(), QStringList() << "" << tr("Received") << tr("Renewed"), "", qry.value(0).toString(), fine, ui->gObjects);
            ui->lObjects->layout()->addWidget(row);
            connect(row, SIGNAL(changed()), this, SLOT(refreshFine()));
            receive = true;
        }

        // space filler
        ui->lObjects->layout()->addWidget(new QWidget);

        // numbers
        int debt = MObjects::getDebt(ui->eUser->value());
        ui->lDebt->setText(QString("%1").arg(debt));
        refreshFine();

        ui->gDeliver->setEnabled(true);
        if (receive)
            ui->gReceive->setEnabled(true);

        ui->sDiscount->setEnabled(Reghaabat::hasAccess("manager"));
        ui->eObject->setFocus();
    }
}

void FormOperator::refreshFine()
{
    int fine = 0;
    MatchRow* row;
    for (int i = 0; i < ui->lObjects->layout()->count() - 1; i++)
    {
        row = (MatchRow*)(ui->lObjects->layout()->itemAt(i)->widget());
        if (! row->getState().isEmpty())
            fine += row->fine;
    }

    ui->lFine->setText(QString("%1").arg(fine));
}

void FormOperator::selectObject()
{
    if (! ui->eObject->value().isEmpty()) {

        ui->bDeliver->setEnabled(true);

        if (ui->eObject->value().startsWith("x")) {
            // pure matches
            matchId = ui->eObject->value().mid(1);
        } else {
            // matches that linked with objects
            QSqlQuery qry;
            qry.exec(QString("select id from matches where object_id = %1").arg(ui->eObject->value()));
            if (qry.next())
                matchId = qry.value(0).toString();
        }

        if (! matchId.isEmpty()) {
            int height = 90;
            ui->gMatchOpt->setVisible(true);
            ui->gDeliver->setMaximumHeight(height);
            ui->gDeliver->setMinimumHeight(height);
            ui->lMatchError->setText(MMatches::isDeliverable(ui->eUser->value(), matchId));
            ui->lMatchError->setVisible(!ui->lMatchError->text().isEmpty());

            ui->bPreview->setEnabled(true);
            prepareViewer();
        }

        ui->bDeliver->setFocus();
    }
}

void FormOperator::on_bDeliver_clicked()
{
    if (! ui->eUser->value().isEmpty() && ! ui->eObject->value().isEmpty())
    {
        QString msg;
        bool match = (!matchId.isEmpty()) && (ui->cDeliverMatch->isChecked());

        if (match)
            msg = MMatches::isDeliverable(ui->eUser->value(), matchId);

        if (msg.isEmpty() && !ui->eObject->value().startsWith("x"))
            msg = MObjects::deliver(ui->eUser->value(), ui->eObject->value());

        if (match && msg.isEmpty()) {
            msg = MMatches::deliver(ui->eUser->value(), matchId);
            if (msg.isEmpty() && ui->cPrint->isChecked())
                viewer->on_bPrint_clicked();
        }

        if (! msg.isEmpty()) {
            QMessageBox::critical(0, QObject::tr("Reghaabat"), msg);
            return;
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
    if (prepared == matchId)
        return;

    StrMap match;
    QList<StrPair> questions, asks;
    MMatches::get(matchId, match, questions);
    match["user"] = ui->eUser->value();

    QSqlQuery qry;

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

    prepared = ui->eObject->value();
}

void FormOperator::focus()
{
    ui->eUser->setFocus();
}

void FormOperator::on_bReceive_clicked()
{
    QString msg;

    // payment
    msg = MObjects::charge(ui->eUser->value(), ui->lFine->text().toInt(), ui->sDiscount->value(), ui->sPayment->value());
    if (! msg.isEmpty())
    {
        QMessageBox::critical(0, QObject::tr("Reghaabat"), msg);
        return;
    }

    // last widget is space filler
    MatchRow* row;
    for (int i = 0; i < ui->lObjects->layout()->count() - 1; i++)
    {
        row = (MatchRow*)(ui->lObjects->layout()->itemAt(i)->widget());

        if (row->getState() == tr("Received"))
        {
            if (! row->objectId.isEmpty())
                MObjects::receive(ui->eUser->value(), row->objectId);
            if (! row->matchId.isEmpty())
                MMatches::receive(ui->eUser->value(), row->matchId);
        }
        else if (row->getState() == tr("Renewed"))
        {
            if (! row->objectId.isEmpty())
                MObjects::renew(ui->eUser->value(), row->objectId);
        }
    }

    cancelUser();
    ui->eUser->setText("");
    ui->eUser->setFocus();
}
