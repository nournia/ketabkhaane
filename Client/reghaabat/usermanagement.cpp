#include "usermanagement.h"
#include "ui_usermanagement.h"

UserManagement::UserManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagement)
{
    ui->setupUi(this);

    connect(ui->bNewUser, SIGNAL(clicked()), parent, SLOT(newUser()));
    connect(ui->bEditUser, SIGNAL(clicked()), parent, SLOT(editUser()));

    viewer = ((MainWindow*) parent)->viewer;
}

UserManagement::~UserManagement()
{
    delete ui;
}

void UserManagement::on_bMatchList_clicked()
{
    if (ui->cGenderGroup->isChecked())
        viewer->bUserGenderGroup();
    else
        viewer->bUserAll();
    viewer->exec();
}
