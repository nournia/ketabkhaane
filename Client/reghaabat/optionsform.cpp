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

    ui->bConvertDb->setVisible(ui->bConvertDb->isEnabled());

    ui->cPrinters->clear();
    foreach(QPrinterInfo printer, QPrinterInfo::availablePrinters())
        ui->cPrinters->addItem(printer.printerName());

    ui->cCorrectorIdentifier->addItem(tr("NameFamily"), "NameFamily");
    ui->cCorrectorIdentifier->addItem(tr("Family"), "Family");
    ui->cCorrectorIdentifier->addItem(tr("Id"), "Id");

    QSettings settings("Rooyesh", "Reghaabat");
    ui->eLibraryAddress->setText(settings.value("LibraryAddress", "").toString());
    ui->eDataFolder->setText(settings.value("DataFolder", "").toString());
    ui->cPrinters->setCurrentIndex(ui->cPrinters->findText(settings.value("Printer", "").toString()));
    ui->cCorrectorIdentifier->setCurrentIndex(ui->cCorrectorIdentifier->findData(options()["CorrectorIdentifier"].toString()));
    ui->sMaxConcurrentMatches->setValue(options()["MaxConcurrentMatches"].toInt());
    ui->sMaxMatchesInOneDay->setValue(options()["MaxMatchesInOneDay"].toInt());
}

OptionsForm::~OptionsForm()
{
    delete ui;
}

void OptionsForm::on_bConvertDb_clicked()
{
    convertAccessDbToSqliteDb("C:\\Users\\Noorian\\Desktop\\Reghaabat.mdb");
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
    writeOption("CorrectorIdentifier", ui->cCorrectorIdentifier->itemData(ui->cCorrectorIdentifier->currentIndex()).toString());
    writeOption("MaxConcurrentMatches", ui->sMaxConcurrentMatches->value());
    writeOption("MaxMatchesInOneDay", ui->sMaxMatchesInOneDay->value());

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
