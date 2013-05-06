#include "payment.h"
#include "ui_payment.h"

#include <helper.h>
#include <uihelper.h>
#include <musers.h>
#include <jalali.h>
#include <viewerform.h>
#include <QMessageBox>

Payment::Payment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Payment)
{
    ui->setupUi(this);

    ui->eUser->setQuery(MUsers::getUsersQuery());
    connect(ui->eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(ui->eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    // table configurations
    customizeTable(ui->tPayments, 2, 80, false, 0, true);
    customizeTable(ui->tScores, 2, 80, false, 0, true);

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
    int payment = MUsers::getPayment(ui->eUser->value()), score = MUsers::getScore(ui->eUser->value());

    // fill tables
    qry.exec(QString("select created_at, -1 * score from transactions where user_id = %1 and description = 'pay' and created_at > (select started_at from library) order by created_at desc").arg(ui->eUser->value()));
    for (int row = 0; qry.next(); row++)
    {
        ui->tPayments->insertRow(row);
        ui->tPayments->setItem(row, 0, getItem(toJalali(qry.value(0).toDate())));
        ui->tPayments->setItem(row, 1, getItem(qry.value(1).toString()));
    }

    QString rate = QString("case when rate between -10 and 0.25 then '%1' when rate between 0.25 and 0.5 then '%2' when rate between 0.5 and 0.75 then '%3' when rate between 0.75 and 10 then '%4' end").arg(ViewerForm::tr("Grade1")).arg(ViewerForm::tr("Grade2")).arg(ViewerForm::tr("Grade3")).arg(ViewerForm::tr("Grade4"));
    qry.exec(QString("select matches.title, %1 as grate from answers inner join matches on answers.match_id = matches.id where rate is not null and user_id = %2 and delivered_at > (select started_at from library) order by corrected_at").arg(rate).arg(ui->eUser->value()));
    for (int row = 0; qry.next(); row++)
    {
        ui->tScores->insertRow(row);
        ui->tScores->setItem(row, 0, getItem(qry.value(0).toString()));
        ui->tScores->setItem(row, 1, getItem(qry.value(1).toString()));
    }

    ui->lSum->setText(QString("%1").arg(score));
    ui->lResidue->setText(QString("%1").arg(score - payment));

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
    QString msg = MUsers::pay(ui->eUser->value(), ui->sPay->value());

    // there isn't any error
    if (msg == "")
    {
        cancelUser();
        selectUser();
    }
    else
        QMessageBox::critical(this, QObject::tr("Ketabkhaane"), msg);
}
