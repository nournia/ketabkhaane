#include "objectform.h"
#include "ui_objectform.h"

#include <QMessageBox>

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
    ui->cType->setEnabled(! edit);
    ui->cBranch->setEnabled(! edit);

    if (edit)
        ui->eObject->setFocus();
    checkReadOnly();
}

void ObjectForm::selectObject()
{
    if (! ui->eObject->value().isEmpty())
    {
        cancelObject();

        StrMap object;
        MObjects::get(ui->eObject->value(), object);

        ui->eTitle->setText(object["title"].toString());
        ui->sCount->setValue(object["cnt"].toInt());
        ui->cType->setCurrentIndex(ui->cType->findData(object["type_id"]));
        ui->cBranch->setCurrentIndex(ui->cBranch->findData(object["branch_id"]));
        ui->eAuthor->setText(object["author"].toString());
        ui->eAuthor->setValue(object["author_id"].toString());
        ui->ePublication->setText(object["publication"].toString());
        ui->ePublication->setValue(object["publication_id"].toString());
        ui->eLabel->setText(object["label"].toString());

        ui->gData->setEnabled(true);
        ui->eTitle->setFocus();
    }
    checkReadOnly();
}

void ObjectForm::cancelObject()
{
    ui->eObject->setQuery(MObjects::getObjectsQuery());

    ui->eTitle->setText("");
    ui->sCount->setValue(1);
    ui->cType->setCurrentIndex(0);
    ui->cBranch->setCurrentIndex(0);
    ui->eAuthor->setText("");
    ui->ePublication->setText("");
    ui->eLabel->setText(MObjects::getNewLabel(ui->cBranch->itemData(ui->cBranch->currentIndex()).toString()));

    ui->gData->setEnabled(false);

    ui->eObject->setFocus();
}

void ObjectForm::checkReadOnly()
{
    bool original = true;
    if (! ui->eObject->value().isEmpty())
        original = ui->eObject->value().startsWith(Reghaabat::instance()->libraryId);

    ui->cType->setEnabled(original);
    ui->eTitle->setEnabled(original);
    ui->eAuthor->setEnabled(original);
    ui->ePublication->setEnabled(original);
}

void ObjectForm::on_cType_currentIndexChanged(int index)
{
    fillComboBox(ui->cBranch, MObjects::branches(ui->cType->itemData(index).toString()));
}

void ObjectForm::on_cBranch_currentIndexChanged(int index)
{
    ui->eLabel->setText(MObjects::getNewLabel(ui->cBranch->itemData(index).toString()));
}

void ObjectForm::on_buttonBox_accepted()
{
    StrMap object;
    object["title"] = ui->eTitle->text();
    object["author_id"] = ! ui->eAuthor->value().isEmpty() ? ui->eAuthor->value() : ui->eAuthor->text();
    object["publication_id"] = ! ui->ePublication->value().isEmpty() ? ui->ePublication->value() : ui->ePublication->text();
    object["cnt"] = ui->sCount->value();
    object["type_id"] = ui->cType->itemData(ui->cType->currentIndex());
    object["branch_id"] = ui->cBranch->itemData(ui->cBranch->currentIndex());
    object["label"] = ui->eLabel->text();

    QString msg = MObjects::set(ui->eObject->value(), object);

    // there isn't any error
    if (msg == "")
    {
        emit closeForm();

        if (ui->eObject->value().isEmpty())
        {
            MObjects::get(object["id"].toString(), object);
            msg = tr("%1 registered with %2 label.").arg(object["title"].toString()).arg(object["label"].toString());
            QMessageBox::information(this, QApplication::tr("Reghaabat"), msg);
        }
    }
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}
