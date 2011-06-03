#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>
#include <QMessageBox>
#include <QPrinterInfo>

#include <accesstosqlite.h>

OptionsForm::OptionsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsForm)
{
    ui->setupUi(this);

    ui->cPrinters->clear();
    foreach(QPrinterInfo printer, QPrinterInfo::availablePrinters())
        ui->cPrinters->addItem(printer.printerName());

    QSettings settings("Rooyesh", "Reghaabat");
    ui->eLibraryAddress->setText(settings.value("LibraryAddress", "").toString());
    ui->cPrinters->setCurrentIndex(ui->cPrinters->findText(settings.value("Printer", "").toString()));
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
    settings.setValue("Printer", ui->cPrinters->currentText());

    QString msg = "";

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}
