#include "matchrow.h"
#include "ui_matchrow.h"

#include <mmatches.h>
#include <jalali.h>

MatchRow::MatchRow(QString title, QStringList states, QString mid, QString oid, QString icon, int fine, QWidget *parent) :
    QWidget(parent), ui(new Ui::MatchRow), matchId(mid), objectId(oid)
{
    ui->setupUi(this);

    ui->label->setText(title);
    ui->combo->addItems(states);
    ui->lIcon->setPixmap(QPixmap(icon));

    if (fine > 0)
        ui->lFine->setText(tr("fine: %1").arg(fine));
}

MatchRow::~MatchRow()
{
    delete ui;
}

QString MatchRow::getState()
{
    return ui->combo->currentText();
}
