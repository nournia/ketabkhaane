#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
//    ui->buttonBox->addButton(QString::fromUtf8("تایید"), QDialogButtonBox::AcceptRole);
//    ui->buttonBox->addButton(QString::fromUtf8("لغو"), QDialogButtonBox::RejectRole);


    eUsername = new MyLineEdit(this);
    eUsername->setObjectName("eUsername");
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, eUsername);

    MyCompleter * completer = new MyCompleter("select firstname || ' ' || lastname from users inner join permissions on users.id = permissions.user_id", this);
    eUsername->setCompleter(completer);

//    qDebug() << Reghaabat::instance()->userId;
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
