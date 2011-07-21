#include "payment.h"
#include "ui_payment.h"

#include <helper.h>
#include <musers.h>
#include <jalali.h>
#include <viewerform.h>

#include <QMessageBox>

Payment::Payment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Payment)
{
    ui->setupUi(this);

    QString condition;
    if (!Reghaabat::hasAccess("manager"))
        condition = QString(" where gender = '%1'").arg(Reghaabat::instance()->userGender);
    eUser = new MyLineEdit("select id as cid, id as clabel, firstname||' '||lastname as ctitle from users" + condition, this);
    ui->lUser->addWidget(eUser);
    connect(eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    QWidget::setTabOrder(eUser, ui->sPay);

    // table configurations
    ui->tPayments->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tScores->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);

    cancelUser();
}

Payment::~Payment()
{
    delete ui;
}

inline QTableWidgetItem* getItem(QString str)
{
    QTableWidgetItem* item = new QTableWidgetItem(str);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

void Payment::selectUser()
{
    QSqlQuery qry;

    qry.exec("select score from scores where user_id = "+ eUser->value());
    if (! qry.next())
        return;
    ui->lSum->setText(qry.value(0).toString());

    qry.exec(QString("select score - ifnull((select sum(payment) as payed_score from payments where user_id = %1 and payed_at > (select started_at from library)), 0) as residue from scores where user_id = %1").arg(eUser->value()));
    if (! qry.next())
        return;
    ui->lResidue->setText(qry.value(0).toString());


    // fill tables
    QTableWidgetItem* item;

    qry.exec(QString("select payed_at, payment from payments where user_id = %1 and payed_at > (select started_at from library) order by payed_at desc").arg(eUser->value()));
    for (int row = 0; qry.next(); row++)
    {
        ui->tPayments->insertRow(row);
        ui->tPayments->setItem(row, 0, getItem(toJalali(qry.value(0).toDate())));
        ui->tPayments->setItem(row, 1, getItem(qry.value(1).toString()));
    }

    QString rate = QString("case when rate between -10 and 0.25 then '%1' when rate between 0.25 and 0.5 then '%2' when rate between 0.5 and 0.75 then '%3' when rate between 0.75 and 10 then '%4' end").arg(ViewerForm::tr("Grade1")).arg(ViewerForm::tr("Grade2")).arg(ViewerForm::tr("Grade3")).arg(ViewerForm::tr("Grade4"));
    qry.exec(QString("select matches.title, %1 as grate from answers inner join matches on answers.match_id = matches.id where rate is not null and user_id = %2 and delivered_at > (select started_at from library) order by corrected_at").arg(rate).arg(eUser->value()));
    for (int row = 0; qry.next(); row++)
    {
        ui->tScores->insertRow(row);
        ui->tScores->setItem(row, 0, getItem(qry.value(0).toString()));
        ui->tScores->setItem(row, 1, getItem(qry.value(1).toString()));
    }


    ui->gPay->setEnabled(true);
    ui->sPay->setFocus();
}


void Payment::cancelUser()
{
    ui->lSum->setText("");
    ui->lResidue->setText("");
    ui->sPay->setValue(0);
    ui->gPay->setEnabled(false);
    while (ui->tPayments->rowCount() >= 1) ui->tPayments->removeRow(0);
    while (ui->tScores->rowCount() >= 1) ui->tScores->removeRow(0);
}

void Payment::on_pushButton_clicked()
{
    QString msg = MUsers::pay(eUser->value(), ui->sPay->value());

    // there isn't any error
    if (msg == "")
    {
        cancelUser();
        selectUser();
    }
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}
