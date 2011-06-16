#include "matchtable.h"
#include "ui_matchtable.h"

#include <QHeaderView>

#include <delegatecombobox.h>

MatchTable::MatchTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MatchTable)
{
    ui->setupUi(this);

    model = new MatchListModel(this);
    ui->tMatches->setModel(model);

    ui->tMatches->setColumnHidden(0, true);
    ui->tMatches->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tMatches->setColumnWidth(2, 60);
    ui->tMatches->setColumnWidth(3, 60);
    ui->tMatches->setColumnWidth(4, 60);
    ui->tMatches->setColumnWidth(5, 60);
    ui->tMatches->verticalHeader()->setDefaultSectionSize(22);
    ui->tMatches->verticalHeader()->setVisible(false);

    ui->tMatches->setItemDelegateForColumn(3, new DelegateComboBox(MMatches::ageclasses(true), ui->tMatches));
    ui->tMatches->setItemDelegateForColumn(6, new DelegateComboBox(MMatches::states(), ui->tMatches));
}

MatchTable::~MatchTable()
{
    delete ui;
}
