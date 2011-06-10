#include "formchangepermissions.h"
#include "ui_formchangepermissions.h"

FormChangePermissions::FormChangePermissions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChangePermissions)
{
    ui->setupUi(this);

    // add username edit
    eUser = new MyLineEdit("select id, firstname || ' ' || lastname as ctitle from users where id not in (select user_id from permissions)", this);
    ui->lUser->addWidget(eUser);
    connect(eUser, SIGNAL(select()), this, SLOT(selectUser()));
    connect(eUser, SIGNAL(cancel()), this, SLOT(cancelUser()));

    // table configurations
    model = new PermissionModel(this);
    ui->tPermissions->setModel(model);

    ui->tPermissions->setColumnHidden(0, true);
    ui->tPermissions->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tPermissions->verticalHeader()->setDefaultSectionSize(22);
    ui->tPermissions->verticalHeader()->setVisible(false);

    ui->tPermissions->setItemDelegateForColumn(2, new DelegateComboBox(PermissionModel::getPermissions(), ui->tPermissions));
    ui->tPermissions->setItemDelegateForColumn(3, new DelegatePassword(ui->tPermissions));

    cancelUser();
    eUser->setFocus();
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
    if (! eUser->value().isEmpty())
    {
        QSqlQuery qry;
        qry.exec(QString("insert into permissions (user_id, permission) values (%1, 'user')").arg(eUser->value()));

        model->refresh();

        eUser->setText("");
        eUser->setCompleter(new MyCompleter("select id, firstname || ' ' || lastname as ctitle from users  where id not in (select user_id from permissions)", eUser));
    }
}
