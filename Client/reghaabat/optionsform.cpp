#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>
#include <QMessageBox>

#include <accesstosqlite.h>

OptionsForm::OptionsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsForm)
{
    ui->setupUi(this);

    QSettings settings("Rooyesh", "Reghaabat");
    ui->eLibraryAddress->setText(settings.value("LibraryAddress", "").toString());
}

OptionsForm::~OptionsForm()
{
    delete ui;
}

void OptionsForm::on_bConvertDb_clicked()
{
    convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb");
}

void OptionsForm::on_buttonBox_rejected()
{
    emit closeForm();
}

void OptionsForm::on_buttonBox_accepted()
{
    QSettings settings("Rooyesh", "Reghaabat");
    settings.setValue("LibraryAddress", ui->eLibraryAddress->text());

    QString msg = "";

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);

}
