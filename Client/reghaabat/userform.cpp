#include "userform.h"
#include "ui_userform.h"

#include <jalali.h>

UserForm::UserForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserForm)
{
    ui->setupUi(this);
}

UserForm::~UserForm()
{
    delete ui;
}

void UserForm::edit(QString userId)
{
    StrMap user = MUsers::get(userId);

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
}
