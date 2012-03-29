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

    // add username edit
    QString condition;
    if (!Reghaabat::hasAccess("manager"))
        condition = QString(" where gender = '%1'").arg(Reghaabat::instance()->userGender);
    ui->eUser->setQuery("select id as cid, label as clabel, firstname||' '||lastname as ctitle from users" + condition);
    connect(ui->eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(ui->eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    connect(ui->eObject, SIGNAL(select()), this, SLOT(selectObject()));
    connect(ui->eObject, SIGNAL(cancel()), this, SLOT(cancelObject()));

    ui->cLimitedSearch->setVisible(false);
    on_cDeliver_currentIndexChanged(0);

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

    // clean gDelivered
    QLayoutItem *child;
    while ((child = ui->lObjects->layout()->takeAt(0)) != 0)
         delete child->widget();
}

void FormOperator::cancelObject()
{
    ui->bDeliver->setEnabled(false);
    ui->bPreview->setEnabled(false);
}

void FormOperator::selectUser()
{
    cancelUser();
    if (! ui->eUser->value().isEmpty())
    {
        bool receive = false;

        // show delivered matches
        QSqlQuery qry;
        qry.exec(QString("select match_id, matches.title from answers inner join matches on answers.match_id = matches.id where user_id = %1 and received_at is null and answers.delivered_at > (select started_at from library)").arg(ui->eUser->value()));
        for (int i = 1; qry.next(); i++)
        {
            MatchRow* row = new MatchRow(qry.value(1).toString(), QStringList() << "" << tr("Received"), qry.value(0).toString(), "", ":/images/match.png", ui->gObjects);
            ui->lObjects->layout()->addWidget(row);
            receive = true;
        }

        // show delivered objects
        qry.exec(QString("select object_id, objects.title from borrows inner join objects on borrows.object_id = objects.id where user_id = %1 and received_at is null").arg(ui->eUser->value()));
        for (int i = 1; qry.next(); i++)
        {
            MatchRow* row = new MatchRow(qry.value(1).toString(), QStringList() << "" << tr("Received") << tr("Renewed"), "", qry.value(0).toString(), ":/images/object.png", ui->gObjects);
            ui->lObjects->layout()->addWidget(row);
            receive = true;
        }

        // space filler
        ui->lObjects->layout()->addWidget(new QWidget);

        ui->gDeliver->setEnabled(true);
        if (receive)
            ui->gReceive->setEnabled(true);

        on_cDeliver_currentIndexChanged(ui->cDeliver->currentIndex());
    }
}

void FormOperator::selectObject()
{
    ui->bDeliver->setEnabled(! ui->eObject->value().isEmpty());
    ui->bPreview->setEnabled(! ui->eObject->value().isEmpty());

    if (! ui->eObject->value().isEmpty())
    {
        ui->bDeliver->setFocus();
        prepareViewer();
    }
}

void FormOperator::on_bDeliver_clicked()
{
    if (! ui->eUser->value().isEmpty() && ! ui->eObject->value().isEmpty())
    {
        QString msg;
        QSqlQuery qry;

        bool object = ui->cDeliver->currentIndex() == 0;

        if (object)
        {
            msg = MObjects::deliver(ui->eUser->value(), ui->eObject->value());
        }
        else {
            qry.exec(QString("select count(match_id) from answers where user_id = %1 and delivered_at > (select started_at from library) and received_at is null").arg(ui->eUser->value()));
            if (qry.next() && qry.value(0).toInt() >= options()["MaxConcurrentMatches"].toInt())
                msg = tr("You received enough matches at the moment.");

            qry.exec(QString("select count(match_id) from answers where user_id = %1 and delivered_at > (select started_at from library) and delivered_at > datetime('now', '-12 hours')").arg(ui->eUser->value()));
            if (qry.next() && qry.value(0).toInt() >= options()["MaxMatchesInOneDay"].toInt())
                msg = tr("You received enough matches today.");

            if (msg.isEmpty())
            {
                msg = MMatches::deliver(ui->eUser->value(), ui->eObject->value());

                if (msg.isEmpty())
                {
                    if (ui->cPrint->isChecked())
                        viewer->on_bPrint_clicked();
                    if (ui->cDeliverObject->isChecked())
                    {
                        qry.exec(QString("select object_id from matches where id = %1").arg(ui->eObject->value()));
                        if (qry.next() && ! qry.value(0).toString().isEmpty())
                            MObjects::deliver(ui->eUser->value(), qry.value(0).toString());
                    }
                }
            }
        }

        if (! msg.isEmpty())
        {
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
    if (prepared == ui->eObject->value())
        return;

    StrMap match;
    QList<StrPair> questions, asks;
    MMatches::get(ui->eObject->value(), match, questions);
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

void FormOperator::on_cLimitedSearch_clicked()
{
    selectUser();
}

void FormOperator::focus()
{
    ui->eUser->setFocus();
}

void FormOperator::on_cDeliver_currentIndexChanged(int index)
{
    bool object = index == 0;
    ui->gMatchOpt->setVisible(! object);

    int height = object ? 60 : 90;
    ui->gDeliver->setMaximumHeight(height);
    ui->gDeliver->setMinimumHeight(height);

    QString query;
    if (ui->cDeliver->currentIndex() == 0)
        query = "select id as cid, label as clabel, substr(title, 0, 80) as ctitle from objects";
    else
        query = "select matches.id as cid, objects.label as clabel, substr(matches.title, 0, 80) as ctitle from matches left join objects on matches.object_id = objects.id";

    // todo limited search of matches

    ui->eObject->setText("");
    ui->eObject->setQuery(query);
    ui->eObject->setFocus();
}
