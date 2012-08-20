#include "usermanagement.h"
#include "ui_usermanagement.h"

#include <uihelper.h>

UserManagement::UserManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagement)
{
    ui->setupUi(this);

    connect(ui->bNewUser, SIGNAL(clicked()), parent, SLOT(newUser()));
    connect(ui->bEditUser, SIGNAL(clicked()), parent, SLOT(editUser()));

    model = new UsersModel(this);
    ui->tUsers->setModel(model);

    customizeTable(ui->tUsers, 4, 80, true, 2);
    ui->tUsers->setColumnWidth(1, 160);

    viewer = ((MainWindow*) parent)->viewer;

    ui->gScoreList->setVisible(false);
    ui->gLogs->setVisible(false);
}

UserManagement::~UserManagement()
{
    delete ui;
}

void UserManagement::on_bLogs_clicked()
{
    ui->gScoreList->setVisible(false);
    ui->gLogs->setVisible(!ui->gLogs->isVisible());
}

void UserManagement::on_bScoreList_clicked()
{
    ui->gScoreList->setVisible(!ui->gScoreList->isVisible());
    ui->gLogs->setVisible(false);
}

void UserManagement::on_bPreviewScoreList_clicked()
{
    if (ui->cGenderGroup->isChecked())
        viewer->bUserGenderGroup();
    else
        viewer->bUserAll();
    viewer->exec();
}

void UserManagement::on_bPreviewLogs_clicked()
{
    viewer->showLogs(ui->eLogTitle->text());
    viewer->exec();
}
