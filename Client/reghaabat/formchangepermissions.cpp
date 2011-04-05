#include "formchangepermissions.h"
#include "ui_formchangepermissions.h"

FormChangePermissions::FormChangePermissions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChangePermissions)
{
    ui->setupUi(this);

    // table configurations
    model = new PermissionModel(this);
    ui->tPermissions->setModel(model);

    ui->tPermissions->setFont(QFont("Tahoma"));
    ui->tPermissions->setColumnHidden(0, true);
    ui->tPermissions->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

    ui->tPermissions->setItemDelegateForColumn(2, new DelegateComboBox(PermissionModel::getPermissions(), ui->tPermissions));
}

FormChangePermissions::~FormChangePermissions()
{
    delete ui;
}
