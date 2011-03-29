#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    eUsername = new MyLineEdit(this);
    eUsername->setObjectName("eUsername");
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, eUsername);

    MyCompleter * completer = new MyCompleter("select firstname || ' ' || lastname from users inner join permissions on users.id = permissions.user_id", this);
    eUsername->setCompleter(completer);

    qDebug() << Reghaabat::instance()->userId;

//    MainWindow::reghaabat.user_id = "login";
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
