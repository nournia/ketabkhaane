#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    titleEdit = new TitleEdit(ui->eUsername, this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
