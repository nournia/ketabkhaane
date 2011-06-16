#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>
#include <QMessageBox>
#include <QPrinterInfo>
#include <QFileDialog>

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
    ui->eDataFolder->setText(settings.value("DataFolder", "").toString());
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
    settings.setValue("DataFolder", ui->eDataFolder->text());
    settings.setValue("Printer", ui->cPrinters->currentText());

    QString msg = "";

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}

void OptionsForm::on_bSelectLibraryDb_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select Library Database")).replace("\\", "/");
    if (!filename.isEmpty())
        ui->eLibraryAddress->setText(filename);
}

void OptionsForm::on_bSelectDataFolder_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this, tr("Select Reghaabat Data Folder")).replace("\\", "/");
    if (!filename.isEmpty())
        ui->eDataFolder->setText(filename);
}
