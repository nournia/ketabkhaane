#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    eUsername = new MyLineEdit(this);
    eUsername->setObjectName("eUsername");
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, eUsername);

    MyCompleter * completer = new MyCompleter("select firstname || ' ' || lastname from users", this);
    eUsername->setCompleter(completer);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
