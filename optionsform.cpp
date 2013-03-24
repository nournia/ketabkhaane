#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication>
#include <QtPrintSupport/QPrinterInfo>

#include <helper.h>
#include <jalali.h>


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
    qry.exec("select title, description, image, started_at from library");
    if (qry.next())
    {
        ui->eLibraryTitle->setText(qry.value(0).toString());
        ui->eLibraryDescription->setText(qry.value(1).toString());
        ui->eStartDate->setText(toJalali(qry.value(3).toDate()));

        libraryLogo = QString("%1/files/%2").arg(dataFolder(), qry.value(2).toString());
        ui->bLibraryLogo->setIcon(QIcon(libraryLogo));
        newLibraryLogo = "";
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

    QVariantMap opt = options();

    opt["Match"] = ui->gMatch->isChecked();
    opt["CorrectorIdentifier"] = ui->cCorrectorIdentifier->itemData(ui->cCorrectorIdentifier->currentIndex()).toString();
    opt["MaxConcurrentMatches"] = ui->sMaxConcurrentMatches->value();
    opt["MaxMatchesInOneDay"] = ui->sMaxMatchesInOneDay->value();

    opt["BookBorrowDays"] = ui->sBookBorrowDays->value();

    if (!newLibraryLogo.isEmpty())
    {
        QFile::remove(libraryLogo);
        QFile::copy(newLibraryLogo, libraryLogo);
        insertLog("files", "update", "1");
    }

    // store options
    QSqlQuery qry;
    if (qry.exec(QString("update library set title = '%1', description = '%2', started_at = '%3', options = '%4'").arg(ui->eLibraryTitle->text(), ui->eLibraryDescription->toPlainText(), formatDate(toGregorian(ui->eStartDate->text())), QVariantMapToString(opt))))
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

void OptionsForm::on_bLibraryLogo_clicked()
{
    newLibraryLogo = QFileDialog::getOpenFileName(this, tr("Select Library Logo"), "", "Image (*.jpg)");
    if (!newLibraryLogo.isEmpty())
    {
        // check for logo format and dimension
        QImage logo(newLibraryLogo);

        if (logo.size() == QSize(300, 300))
            ui->bLibraryLogo->setIcon(QIcon(newLibraryLogo));
        else
            QMessageBox::critical(this, QApplication::tr("Reghaabat"), tr("Logo must be in 300px x 300px dimension."));
    }
}
