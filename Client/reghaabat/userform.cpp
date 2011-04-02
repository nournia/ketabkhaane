#include "userform.h"
#include "ui_userform.h"

#include <QMessageBox>

#include <jalali.h>

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
    user["email"] = ui->eEmail->text();
    user["birth_date"] = toGregorian(ui->eBirthDate->text());
    user["gender"] = ui->rMale->isChecked() ? "male" : "female";

    QString msg = MUsers::set(eUser->value(), user);

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), msg);
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
        ui->eEmail->setText(user["email"].toString());

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
    eUser->setCompleter(new MyCompleter("select id, firstname || ' ' || lastname as ctitle from users", eUser));

    ui->eFirstname->setText("");
    ui->eLastname->setText("");
    ui->eNationalId->setText("");
    ui->eAddress->setText("");
    ui->ePhone->setText("");
    ui->eDescription->setText("");
    ui->eEmail->setText("");
    ui->eBirthDate->setText("");
    ui->rMale->setChecked(true);

    ui->gData->setEnabled(false);
    ui->buttonBox->setEnabled(false);

    eUser->setFocus();
}
