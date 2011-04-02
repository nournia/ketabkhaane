#include "optionsform.h"
#include "ui_optionsform.h"

#include <accesstosqlite.h>

OptionsForm::OptionsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsForm)
{
    ui->setupUi(this);
}

OptionsForm::~OptionsForm()
{
    delete ui;
}

void OptionsForm::on_bConvertDb_clicked()
{
    convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb");
}
