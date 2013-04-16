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

    // table configurations
    model = new PermissionModel(this);
    ui->tPermissions->setModel(model);

    customizeTable(ui->tPermissions, 3);

    ui->tPermissions->setItemDelegateForColumn(2, new ComboBoxDelegate(PermissionModel::getPermissions(), ui->tPermissions));
    ui->tPermissions->setItemDelegateForColumn(3, new LineEditDelegate(true, ui->tPermissions));
}

FormChangePermissions::~FormChangePermissions()
{
    delete ui;
}
