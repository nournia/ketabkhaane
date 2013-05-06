#include "logindialog.h"
#include "ui_logindialog.h"

#include <musers.h>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->eUsername->setQuery("select users.id as cid, permissions.label as clabel, firstname||' '||lastname as ctitle from users inner join permissions on users.id = permissions.user_id where permission != 'user' and upassword is not null");
    connect(ui->eUsername, SIGNAL(select()), this, SLOT(selectUser()));
    ui->eUsername->setFocus();
}

void LoginDialog::selectUser()
{
    ui->ePassword->setFocus();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_accepted()
{
    if (ui->ePassword->text().isEmpty())
        return;

    StrMap user;
    if (MUsers::login(ui->eUsername->value(), ui->ePassword->text(), user))
    {
        App::instance()->userId = user["id"].toString();
        App::instance()->userName = user["name"].toString();
        App::instance()->userGender = user["gender"].toString();
        App::instance()->userPermission = user["permission"].toString();
        this->close();
    }
    else {
        QMessageBox::warning(this, QObject::tr("Ketabkhaane"), tr("Invalid Username or Password."));
        ui->ePassword->setText("");
        ui->ePassword->setFocus();
    }
}

void LoginDialog::on_buttonBox_rejected()
{
    this->close();
}
