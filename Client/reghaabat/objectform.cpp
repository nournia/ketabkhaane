#include "objectform.h"
#include "ui_objectform.h"

#include <uihelper.h>
#include <mobjects.h>

ObjectForm::ObjectForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectForm)
{
    ui->setupUi(this);

    ui->eAuthor->setQuery("select id as cid, '' as clabel, title as ctitle from authors");
    ui->ePublication->setQuery("select id as cid, '' as clabel, title as ctitle from publications");

    connect(ui->eObject, SIGNAL(select()), this, SLOT(selectObject()));
    connect(ui->eObject, SIGNAL(cancel()), this, SLOT(cancelObject()));

    fillComboBox(ui->cType, MObjects::types());
    fillComboBox(ui->cRoot, MObjects::roots());
    fillComboBox(ui->cBranch, MObjects::branches("1"));
}

ObjectForm::~ObjectForm()
{
    delete ui;
}

void ObjectForm::editMode(bool edit)
{
    if (ui->eObject->text().isEmpty())
        cancelObject();
    else
        ui->eObject->setText("");

    ui->gObject->setVisible(edit);
    ui->gData->setEnabled(! edit);

    if (edit)
        ui->eObject->setFocus();
}

void ObjectForm::selectObject()
{
    if (! ui->eObject->value().isEmpty())
    {
        cancelObject();

        StrMap object;
        MObjects::get(ui->eObject->value(), object);

        ui->eLabel->setText(object["label"].toString());
        ui->eTitle->setText(object["title"].toString());
        ui->sCount->setValue(object["cnt"].toInt());
        ui->cType->setCurrentIndex(ui->cType->findData(object["type_id"]));
        ui->cRoot->setCurrentIndex(ui->cRoot->findData(object["root_id"]));
        ui->cBranch->setCurrentIndex(ui->cBranch->findData(object["branch_id"]));
        ui->eAuthor->setText(object["author"].toString());
        ui->eAuthor->setValue(object["author_id"].toString());
        ui->ePublication->setText(object["publication"].toString());
        ui->ePublication->setValue(object["publication_id"].toString());

        ui->gData->setEnabled(true);
        ui->eTitle->setFocus();
    }
}

void ObjectForm::cancelObject()
{
    ui->eObject->setQuery("select id as cid, label as clabel, title as ctitle from objects");

    ui->eTitle->setText("");
    ui->sCount->setValue(1);
    ui->cType->setCurrentIndex(0);
    ui->cRoot->setCurrentIndex(0);
    ui->cBranch->setCurrentIndex(0);
    ui->eAuthor->setText("");
    ui->ePublication->setText("");
    ui->eLabel->setText(MObjects::getNewLabel(ui->cBranch->itemData(ui->cBranch->currentIndex()).toString()));

    ui->gData->setEnabled(false);

    ui->eObject->setFocus();
}

void ObjectForm::on_cRoot_currentIndexChanged(int index)
{
    fillComboBox(ui->cBranch, MObjects::branches(ui->cRoot->itemData(index).toString()));
}

void ObjectForm::on_cBranch_currentIndexChanged(int index)
{
    ui->eLabel->setText(MObjects::getNewLabel(ui->cBranch->itemData(index).toString()));
}
