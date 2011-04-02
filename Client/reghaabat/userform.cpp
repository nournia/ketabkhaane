#include "userform.h"
#include "ui_userform.h"

#include <QMessageBox>

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

void UserForm::clear()
{
    userId = "";
    ((MainWindow*)parent())->clear();
}

void UserForm::select(QString id)
{
    userId = id;

    StrMap user;
    MUsers::get(id, user);
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

void UserForm::on_buttonBox_rejected()
{
    clear();
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

    QString msg = MUsers::set(userId, user);

    // there isn't any error
    if (msg == "")
        clear();
    else
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), msg);
}
