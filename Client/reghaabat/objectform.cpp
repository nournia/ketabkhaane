#include "objectform.h"
#include "ui_objectform.h"

ObjectForm::ObjectForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectForm)
{
    ui->setupUi(this);
}

ObjectForm::~ObjectForm()
{
    delete ui;
}

void ObjectForm::editMode(bool edit)
{

}

void ObjectForm::selectObject()
{

}

void ObjectForm::cancelObject()
{

}
