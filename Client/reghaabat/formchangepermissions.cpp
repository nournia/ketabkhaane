#include "formchangepermissions.h"
#include "ui_formchangepermissions.h"

FormChangePermissions::FormChangePermissions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChangePermissions)
{
    ui->setupUi(this);
}

FormChangePermissions::~FormChangePermissions()
{
    delete ui;
}
