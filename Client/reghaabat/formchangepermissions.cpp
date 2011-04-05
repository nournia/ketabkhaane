#include "formchangepermissions.h"
#include "ui_formchangepermissions.h"

FormChangePermissions::FormChangePermissions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChangePermissions)
{
    ui->setupUi(this);

    // table configurations
    ui->tPermissions->setFont(QFont("Tahoma"));
    ui->tPermissions->setColumnHidden(0, true);
    ui->tPermissions->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

//    model = new PermissionModel(this);
//    ui->tPermissions->setModel(model);

    QList<StrPair> permissions;
    permissions.append(qMakePair(tr("admin"), QString("admin")));
    permissions.append(qMakePair(tr("manager"), QString("manager")));
    permissionDelegate = new DelegateComboBox(permissions, ui->tPermissions);
    ui->tPermissions->setItemDelegateForColumn(2, permissionDelegate);
}

FormChangePermissions::~FormChangePermissions()
{
    delete ui;
}
