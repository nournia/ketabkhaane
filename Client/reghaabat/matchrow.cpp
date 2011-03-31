#include "matchrow.h"
#include "ui_matchrow.h"

#include <mmatches.h>

MatchRow::MatchRow(QString uid, QString mid, QString title, QWidget *parent) :
    QWidget(parent), userId(uid), matchId(mid),
    ui(new Ui::MatchRow)
{
    ui->setupUi(this);

    ui->label->setText(title);
}

MatchRow::~MatchRow()
{
    delete ui;
}

void MatchRow::on_button_clicked()
{
    MMatches::receive(userId, matchId);
    delete this;
}
