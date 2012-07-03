#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>
#include <QMessageBox>
#include <QPrinterInfo>
#include <QFileDialog>
#include <QCoreApplication>

#include <helper.h>


OptionsForm::OptionsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsForm)
{
    ui->setupUi(this);

    ui->cPrinters->clear();
    foreach(QPrinterInfo printer, QPrinterInfo::availablePrinters())
        ui->cPrinters->addItem(printer.printerName());

    ui->cCorrectorIdentifier->addItem(tr("NameFamily"), "NameFamily");
    ui->cCorrectorIdentifier->addItem(tr("Family"), "Family");
    ui->cCorrectorIdentifier->addItem(tr("Label"), "Label");

    QSettings settings("Sobhe", "Reghaabat");
    ui->eDataFolder->setText(settings.value("DataFolder", "").toString());
    ui->cPrinters->setCurrentIndex(ui->cPrinters->findText(settings.value("Printer", "").toString()));
    ui->lVersion->setText(QCoreApplication::applicationVersion());

    ui->gMatch->setChecked(options()["Match"].toBool());
    ui->cCorrectorIdentifier->setCurrentIndex(ui->cCorrectorIdentifier->findData(options()["CorrectorIdentifier"].toString()));
    ui->sMaxConcurrentMatches->setValue(options()["MaxConcurrentMatches"].toInt());
    ui->sMaxMatchesInOneDay->setValue(options()["MaxMatchesInOneDay"].toInt());

    ui->sBookBorrowDays->setValue(options()["BookBorrowDays"].toInt());

    QSqlQuery qry;
    qry.exec("select title, description, image from library");
    if (qry.next())
    {
        ui->eLibraryTitle->setText(qry.value(0).toString());
        ui->eLibraryDescription->setText(qry.value(1).toString());

        libraryLogo = qry.value(2).toString();
        ui->bLibraryLogo->setIcon(QIcon(QString("%1/files/%2").arg(dataFolder(), libraryLogo)));
    }
}

OptionsForm::~OptionsForm()
{
    delete ui;
}

void OptionsForm::on_buttonBox_rejected()
{
    emit closeForm();
}

void OptionsForm::on_buttonBox_accepted()
{
    QSettings settings("Sobhe", "Reghaabat");
    settings.setValue("DataFolder", ui->eDataFolder->text());
    settings.setValue("Printer", ui->cPrinters->currentText());

    writeOption("Match", ui->gMatch->isChecked());
    writeOption("CorrectorIdentifier", ui->cCorrectorIdentifier->itemData(ui->cCorrectorIdentifier->currentIndex()).toString());
    writeOption("MaxConcurrentMatches", ui->sMaxConcurrentMatches->value());
    writeOption("MaxMatchesInOneDay", ui->sMaxMatchesInOneDay->value());

    writeOption("BookBorrowDays", ui->sBookBorrowDays->value());

    QSqlQuery qry;
    if (qry.exec(QString("update library set title = '%1', description = '%2', image = '%3' where id = 1").arg(ui->eLibraryTitle->text(), ui->eLibraryDescription->toPlainText(), libraryLogo)))
        insertLog("library", "update", "1");

    QString msg = "";

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}

void OptionsForm::on_bSelectDataFolder_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this, tr("Select Reghaabat Data Folder")).replace("\\", "/");
    if (!filename.isEmpty())
        ui->eDataFolder->setText(filename);
}
