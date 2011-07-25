#include "formchangepermissions.h"
#include "ui_formchangepermissions.h"

#include <comboboxdelegate.h>
#include <lineeditdelegate.h>

#include <uihelper.h>

FormChangePermissions::FormChangePermissions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChangePermissions)
{
    ui->setupUi(this);

    // add username edit
    ui->eUser->setQuery("select id as cid, id as clabel, firstname || ' ' || lastname as ctitle from users where id not in (select user_id from permissions)");
    connect(ui->eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(ui->eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    // table configurations
    model = new PermissionModel(this);
    ui->tPermissions->setModel(model);

    customizeTable(ui->tPermissions, 3);

    ui->tPermissions->setItemDelegateForColumn(2, new ComboBoxDelegate(PermissionModel::getPermissions(), ui->tPermissions));
    ui->tPermissions->setItemDelegateForColumn(3, new LineEditDelegate(true, ui->tPermissions));

    cancelUser();
}

FormChangePermissions::~FormChangePermissions()
{
    delete ui;
}

void FormChangePermissions::selectUser()
{
    ui->bAdd->setEnabled(true);
    ui->bAdd->setFocus();
}

void FormChangePermissions::cancelUser()
{
    ui->bAdd->setEnabled(false);
}

void FormChangePermissions::on_bAdd_clicked()
{
    if (! ui->eUser->value().isEmpty())
    {
        MUsers::setPermission(ui->eUser->value(), "user");

        model->sort(1);

        ui->eUser->setText("");
        ui->eUser->setQuery("select id as cid, id as clabel, firstname || ' ' || lastname as ctitle from users where id not in (select user_id from permissions)");
    }
}
