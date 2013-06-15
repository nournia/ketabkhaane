#include "objectmanagement.h"
#include "ui_objectmanagement.h"

#include <mainwindow.h>
#include <uihelper.h>
#include <jalali.h>

ObjectManagement::ObjectManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectManagement)
{
    ui->setupUi(this);

    connect(ui->bNewObject, SIGNAL(clicked()), parent, SLOT(newObject()));
    connect(ui->bEditObject, SIGNAL(clicked()), parent, SLOT(editObject()));

    model = new ObjectsModel(this);
    ui->tObjects->setModel(model);

    customizeTable(ui->tObjects, 6, 80, true, 2);
    ui->tObjects->setColumnWidth(3, 120);
    ui->tObjects->setColumnWidth(4, 120);
    ui->tObjects->setColumnWidth(6, 160);

    // config labels table
    ui->tLabels->setColumnCount(2);
    ui->tLabels->setColumnHidden(2, true);
    ui->tLabels->verticalHeader()->setDefaultSectionSize(22);
    ui->tLabels->horizontalHeader()->setDefaultSectionSize(60);
    ui->tLabels->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tLabels->verticalHeader()->hide();
    ui->tLabels->setHorizontalHeaderLabels(QStringList() << tr("Label") << tr("Title"));

    // set default values
    QSqlQuery qry;
    qry.exec("select min(label) from belongs where label > '000-000'");
    if (qry.next()) {
        ui->eFromLabels->setText(qry.value(0).toString());
        ui->eToLabels->setText(qry.value(0).toString());
        ui->eOneLabel->setText(qry.value(0).toString());
    }
    ui->eFromDate->setText(toJalali(QDate::currentDate()));
    ui->eToDate->setText(toJalali(QDate::currentDate()));

    ui->bPrintList->setVisible(false);
    ui->gList->setVisible(false);
    ui->gLabels->setVisible(false);
//    if (! options()["Match"].toBool())
        ui->cAgeClassMark->setVisible(false);
}

ObjectManagement::~ObjectManagement()
{
    delete ui;
}

void ObjectManagement::refresh()
{
    model->sort(model->column, model->order);
}

void ObjectManagement::on_bPreviewList_clicked()
{
    ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
    viewer->showObjectList(ui->eFromList->text(), ui->eToList->text());
    viewer->exec();
}

void ObjectManagement::on_bPreviewLabels_clicked()
{
    ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
    viewer->showObjectLabels(labels, stars, ui->cAgeClassMark->isChecked());
    viewer->exec();
}

void ObjectManagement::on_bPrintLabels_clicked()
{
    ui->gLabels->setVisible(!ui->gLabels->isVisible());
    ui->tObjects->setVisible(!ui->gLabels->isVisible());
    ui->gList->setVisible(false);
}

void ObjectManagement::on_bPrintList_clicked()
{
    ui->gList->setVisible(!ui->gList->isVisible());
    ui->gLabels->setVisible(false);
    ui->tObjects->setVisible(true);

    QSqlQuery qry;
    qry.exec("select min(label), max(label) from belongs where label > '000-000'");
    if (qry.next())
    {
        ui->eFromList->setText(qry.value(0).toString());
        ui->eToList->setText(qry.value(1).toString());
    }
}

void ObjectManagement::on_bEditObject_clicked()
{
    QModelIndex c = ui->tObjects->currentIndex();
    if (c.isValid())
        App::instance()->tmpId = model->data(c.sibling(c.row(), 0)).toString();
}

void ObjectManagement::addLabels(QString condition)
{
    QSqlQuery qry;
    QString labelSql = "select label, objects.title, case matches.ageclass when 0 then '*' when 1 then '**' when 2 then '***' when 3 then '****' when 4 then '*****' end as level from belongs inner join objects on belongs.object_id = objects.id left join matches on belongs.object_id = matches.object_id";
    qry.exec(labelSql + QString(" where %1 order by label").arg(condition));
    for (int row = ui->tLabels->rowCount(); qry.next(); row++) {
        if (labels.contains(qry.value(0).toString())) {
            row--;
            continue;
        }

        labels.append(qry.value(0).toString());
        stars.append(qry.value(2).toString());
        ui->tLabels->setRowCount(row+1);
        for (int column = 0; column < 2; column++) {
          QTableWidgetItem* item = new QTableWidgetItem();
          item->setText(qry.value(column).toString());
          if (column == 0) item->setTextAlignment(Qt::AlignCenter);
          ui->tLabels->setItem(row, column, item);
        }
    }
}


void ObjectManagement::on_bAddLabelRange_clicked()
{
    addLabels(QString("label >= '%1' and label <= '%2'").arg(ui->eFromLabels->text(), ui->eToLabels->text()));
}

void ObjectManagement::on_bAddOneLabel_clicked()
{
    addLabels(QString("label == '%1'").arg(ui->eOneLabel->text()));
}

void ObjectManagement::on_bAddCreationDateRange_clicked()
{
    addLabels(QString("objects.id in (select row_id from logs where table_name='objects' and row_op = 'insert' and created_at >= '%1' and created_at <= '%2')").arg(formatDate(toGregorian(ui->eFromDate->text())), formatDate(toGregorian(ui->eToDate->text()))));
}

void ObjectManagement::on_bRemoveAllLabels_clicked()
{
    labels.clear(); stars.clear();
    while (ui->tLabels->rowCount())
        ui->tLabels->removeRow(0);
}

void ObjectManagement::on_bRemoveLabel_clicked()
{
    QModelIndex c = ui->tLabels->currentIndex();
    if (c.isValid()) {
        labels.removeAt(c.row()); stars.removeAt(c.row());
        ui->tLabels->removeRow(c.row());
    }
}
