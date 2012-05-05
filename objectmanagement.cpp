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
}

ObjectManagement::~ObjectManagement()
{
    delete ui;
}
