#include "dialogchangepassword.h"
#include "ui_dialogchangepassword.h"

#include <QMessageBox>

#include <musers.h>

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

    QString msg = MUsers::changePassword(Reghaabat::instance()->userId, ui->eOldPassword->text(), ui->eNewPassword->text(), ui->eRetypedPassword->text());
    QMessageBox::warning(this, QApplication::tr("Reghaabat"), msg);
}

void DialogChangePassword::on_buttonBox_rejected()
{
    this->close();
}
