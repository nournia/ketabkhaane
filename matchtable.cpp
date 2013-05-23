#include "matchtable.h"
#include "ui_matchtable.h"

#include <QHeaderView>

#include <comboboxdelegate.h>
#include <spinboxdelegate.h>
#include <lineeditdelegate.h>
#include <mainwindow.h>
#include <uihelper.h>

MatchTable::MatchTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MatchTable)
{
    ui->setupUi(this);

    connect(ui->bNewMatch, SIGNAL(clicked()), parent, SLOT(newMatch()));
    connect(ui->bEditMatch, SIGNAL(clicked()), parent, SLOT(editMatch()));

    model = new MatchListModel(this);
    ui->tMatches->setModel(model);

    customizeTable(ui->tMatches, 5, 60);

    ui->tMatches->setItemDelegateForColumn(5, new SpinBoxDelegate(0, 10000, 50, ui->tMatches));
    ui->tMatches->setItemDelegateForColumn(6, new ComboBoxDelegate(MMatches::states(), ui->tMatches));

    viewer = ((MainWindow*) parent)->viewer;
}

MatchTable::~MatchTable()
{
    delete ui;
}

void MatchTable::refresh()
{
    model->sort(model->column, model->order);
}

void MatchTable::on_bEditMatch_clicked()
{
    QModelIndex c = ui->tMatches->currentIndex();
    if (c.isValid())
        App::instance()->tmpId = model->data(c.sibling(c.row(), 0)).toString();
}

void MatchTable::on_bMatchList_clicked()
{
    if (ui->cAgeGroup->isChecked())
        viewer->bMatchAgeGroup();
    else
        viewer->bMatchAll();
    viewer->exec();
}
