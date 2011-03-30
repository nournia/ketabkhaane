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

    eUsername = new MyLineEdit(this);
    eUsername->setObjectName("eUsername");
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, eUsername);
    QWidget::setTabOrder(eUsername, ui->ePassword);
    eUsername->setFocus();

    MyCompleter * completer = new MyCompleter("select users.id, firstname || ' ' || lastname as ctitle from users inner join permissions on users.id = permissions.user_id", this);
    eUsername->setCompleter(completer);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_accepted()
{
    if (! MUsers::login(eUsername->value(), ui->ePassword->text()))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Invalid Username or Password."));
        msgBox.exec();
    }
    this->close();
}

void LoginDialog::on_buttonBox_rejected()
{
    this->close();
}
