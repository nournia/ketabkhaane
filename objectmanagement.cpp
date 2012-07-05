#include "objectmanagement.h"
#include "ui_objectmanagement.h"

#include <mainwindow.h>

ObjectManagement::ObjectManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectManagement)
{
    ui->setupUi(this);

    connect(ui->bNewObject, SIGNAL(clicked()), parent, SLOT(newObject()));
    connect(ui->bEditObject, SIGNAL(clicked()), parent, SLOT(editObject()));

    ui->gLabels->setVisible(false);
}

ObjectManagement::~ObjectManagement()
{
    delete ui;
}

void ObjectManagement::on_bLabels_clicked()
{
    ui->gLabels->setVisible(!ui->gLabels->isVisible());

    QSqlQuery qry;
    qry.exec("select min(label), max(label) from objects where label > '000-000'");
    if (qry.next())
    {
        ui->eFromLabel->setText(qry.value(0).toString());
        ui->eToLabel->setText(qry.value(1).toString());
    }
}

void ObjectManagement::on_bPreviewLabels_clicked()
{
    ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
    viewer->showLabels(ui->eFromLabel->text(), ui->eToLabel->text());
    viewer->exec();
}
