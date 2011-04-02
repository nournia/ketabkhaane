#include "dialogchangepassword.h"
#include "ui_dialogchangepassword.h"

#include <QMessageBox>

#include <musers.h>
#include <mainwindow.h>

DialogChangePassword::DialogChangePassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangePassword)
{
    ui->setupUi(this);
}

DialogChangePassword::~DialogChangePassword()
{
    delete ui;
}

void DialogChangePassword::on_buttonBox_accepted()
{
    this->close();

    if (MUsers::changePassword(Reghaabat::instance()->userId, ui->eOldPassword->text(), ui->eNewPassword->text()))
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Password Changed."));
    else
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Invalid Password."));
}

void DialogChangePassword::on_buttonBox_rejected()
{
    this->close();
}
