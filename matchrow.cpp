#include "matchrow.h"
#include "ui_matchrow.h"

#include <mmatches.h>
#include <jalali.h>

MatchRow::MatchRow(QString title, QString label, QStringList states, QString mid, QString oid, int fi, QWidget *parent) :
    QWidget(parent), ui(new Ui::MatchRow), matchId(mid), objectId(oid), fine(fi)
{
    ui->setupUi(this);

    ui->lTitle->setText(title);
    ui->lLabel->setText(label);
    ui->combo->addItems(states);

    if (fine > 0)
        ui->lFine->setText(tr("fine: %1").arg(fine));
    else
        ui->lFine->setVisible(false);

    ui->lObject->setVisible(! objectId.isEmpty());
    ui->lMatch->setVisible(! matchId.isEmpty());

    connect(ui->combo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
}

MatchRow::~MatchRow()
{
    delete ui;
}

QString MatchRow::getState()
{
    return ui->combo->currentText();
}
