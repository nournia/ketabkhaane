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

    ui->gSelectObjects->setVisible(false);
}

ObjectManagement::~ObjectManagement()
{
    delete ui;
}

void ObjectManagement::on_bPrint_clicked()
{
    ui->gSelectObjects->setVisible(!ui->gSelectObjects->isVisible());

    QSqlQuery qry;
    qry.exec("select min(label), max(label) from objects where label > '000-000'");
    if (qry.next())
    {
        ui->eFromLabel->setText(qry.value(0).toString());
        ui->eToLabel->setText(qry.value(1).toString());
    }
}

void ObjectManagement::on_bPreview_clicked()
{
    ViewerForm* viewer = new ViewerForm((MainWindow*) parent());

    if (ui->rList->isChecked())
        viewer->showObjectList(ui->eFromLabel->text(), ui->eToLabel->text());
    else if (ui->rLabels->isChecked())
        viewer->showObjectLabels(ui->eFromLabel->text(), ui->eToLabel->text());

    viewer->exec();
}
