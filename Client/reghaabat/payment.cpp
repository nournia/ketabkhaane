#include "payment.h"
#include "ui_payment.h"

#include <helper.h>
#include <musers.h>

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

    cancelUser();
}

Payment::~Payment()
{
    delete ui;
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

    ui->gPay->setEnabled(true);
    ui->sPay->setFocus();
}


void Payment::cancelUser()
{
    ui->lSum->setText("");
    ui->lResidue->setText("");
    ui->sPay->setValue(0);
    ui->gPay->setEnabled(false);
}

void Payment::on_pushButton_clicked()
{
    QString msg = MUsers::pay(eUser->value(), ui->sPay->value());

    // there isn't any error
    if (msg == "")
        selectUser();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}
