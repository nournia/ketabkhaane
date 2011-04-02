#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <MUsers.h>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    eUsername = new MyLineEdit("select users.id, firstname || ' ' || lastname as ctitle from users inner join permissions on users.id = permissions.user_id", this);
    eUsername->setObjectName("eUsername");
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, eUsername);
    QWidget::setTabOrder(eUsername, ui->ePassword);
    eUsername->setFocus();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_accepted()
{
    if (ui->ePassword->text().isEmpty()) return;

    if (! MUsers::login(eUsername->value(), ui->ePassword->text()))
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Invalid Username or Password."));
    this->close();
}

void LoginDialog::on_buttonBox_rejected()
{
    this->close();
}
