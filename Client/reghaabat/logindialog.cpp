#include "logindialog.h"
#include "ui_logindialog.h"

#include <MUsers.h>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->eUsername->setQuery("select users.id as cid, users.id as clabel, firstname || ' ' || lastname as ctitle from users inner join permissions on users.id = permissions.user_id where permission != 'user' and upassword is not null");
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

    this->close();

    StrMap user;
    if (MUsers::login(ui->eUsername->value(), ui->ePassword->text(), user))
    {
        Reghaabat::instance()->userId = user["id"].toString();
        Reghaabat::instance()->userName = user["name"].toString();
        Reghaabat::instance()->userGender = user["gender"].toString();
        Reghaabat::instance()->userPermission = user["permission"].toString();
    }
    else QMessageBox::warning(this, QApplication::tr("Reghaabat"), tr("Invalid Username or Password."));
}

void LoginDialog::on_buttonBox_rejected()
{
    this->close();
}
