#include "matchrow.h"
#include "ui_matchrow.h"

MatchRow::MatchRow(QString id, QString title, QWidget *parent) :
    QWidget(parent), matchId(id),
    ui(new Ui::MatchRow)
{
    ui->setupUi(this);

    ui->label->setText(title);
}

MatchRow::~MatchRow()
{
    delete ui;
}
