#include "userform.h"
#include "ui_userform.h"

#include <QMessageBox>

#include <jalali.h>
#include <connector.h>

UserForm::UserForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserForm)
{
    ui->setupUi(this);

    // add username edit
    eUser = new MyLineEdit("", this);
    ui->lUser->addWidget(eUser);
    connect(eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    cancelUser();
    editMode(false);
    ui->eFirstname->setFocus();
}

UserForm::~UserForm()
{
    delete ui;
}

void UserForm::editMode(bool edit)
{
    ui->gUser->setVisible(edit);
    ui->gData->setEnabled(! edit);
    ui->buttonBox->setEnabled(! edit);
}

void UserForm::on_buttonBox_rejected()
{
    emit closeForm();
}

void UserForm::on_buttonBox_accepted()
{
    StrMap user;
    user["firstname"] = ui->eFirstname->text();
    user["lastname"] = ui->eLastname->text();
    user["national_id"] = ui->eNationalId->text();
    user["address"] = ui->eAddress->text();
    user["phone"] = ui->ePhone->text();
    user["description"] = ui->eDescription->text();
    user["birth_date"] = toGregorian(ui->eBirthDate->text());
    user["gender"] = ui->rMale->isChecked() ? "male" : "female";

    QString msg = MUsers::set(eUser->value(), user);

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}

void UserForm::selectUser()
{
    if (! eUser->value().isEmpty())
    {
        StrMap user;
        MUsers::get(eUser->value(), user);
        ui->eFirstname->setText(user["firstname"].toString());
        ui->eLastname->setText(user["lastname"].toString());
        ui->eNationalId->setText(user["national_id"].toString());
        ui->eAddress->setText(user["address"].toString());
        ui->ePhone->setText(user["phone"].toString());
        ui->eDescription->setText(user["description"].toString());

        ui->eBirthDate->setText(toJalali(user["birth_date"].toDate()));

        if (user["gender"] == "male")
            ui->rMale->setChecked(true);
        else
            ui->rFemale->setChecked(true);

        ui->gData->setEnabled(true);
        ui->buttonBox->setEnabled(true);
        ui->eFirstname->setFocus();
    }
}

void UserForm::cancelUser()
{
    QString condition;
    if (!Reghaabat::hasAccess("manager"))
        condition = QString(" where gender = '%1'").arg(Reghaabat::instance()->userGender);
    eUser->setQuery("select id as cid, id as clabel, firstname||' '||lastname as ctitle from users" + condition);

    ui->eFirstname->setText("");
    ui->eLastname->setText("");
    ui->eNationalId->setText("");
    ui->eAddress->setText("");
    ui->ePhone->setText("");
    ui->eDescription->setText("");
    ui->eBirthDate->setText("");
    ui->rMale->setChecked(true);

    ui->gData->setEnabled(false);
    ui->buttonBox->setEnabled(false);

    eUser->setFocus();
}

void UserForm::on_bImport_clicked()
{
    bool ok;
    QSqlDatabase library = Connector::connectLibrary(ok);

    if (!ok)
    {
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), tr("Library connection error."));
        return;
    }

    QSqlQuery qry(library);
    qry.exec(QString("select Name, Family, Adress, Phon, T_T, [Is Men] from users where id = '%1'").arg(ui->eLibraryId->text()));

    if (!qry.next())
    {
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), tr("Invalid library id."));
        return;
    }

    // permission validation
    if (!Reghaabat::hasAccess("manager"))
        if (!Reghaabat::hasAccess("operator") ||
            !((qry.value(5).toBool() && Reghaabat::instance()->userGender == "male") || (!qry.value(5).toBool() && Reghaabat::instance()->userGender == "female")))
        {
            QMessageBox::critical(this, QApplication::tr("Reghaabat"), tr("Permission denied."));
            return;
        }

    ui->eFirstname->setText(qry.value(0).toString());
    ui->eLastname->setText(qry.value(1).toString());
    ui->eAddress->setText(qry.value(2).toString());
    ui->ePhone->setText(qry.value(3).toString());

    ui->eBirthDate->setText(qry.value(4).toString());

    if (qry.value(5).toBool())
        ui->rMale->setChecked(true);
    else
        ui->rFemale->setChecked(true);

    ui->gData->setEnabled(true);
    ui->buttonBox->setEnabled(true);
    ui->eFirstname->setFocus();
}

void UserForm::on_gImport_clicked()
{
    ui->gImport->setMaximumHeight(ui->gImport->isChecked() ? 54 : 14);

    if (ui->gImport->isChecked())
        ui->eLibraryId->setFocus();
}
