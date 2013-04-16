#include "scoreform.h"
#include "ui_scoreform.h"

#include <spinboxdelegate.h>
#include <uihelper.h>
#include <musers.h>

ScoreForm::ScoreForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScoreForm)
{
    ui->setupUi(this);

    // add corrector edit
    ui->eCorrector->setQuery(MUsers::getUsersQuery() + " and users.id in (select corrector_id from supports)");
    connect(ui->eCorrector, SIGNAL(select()), this, SLOT(selectCorrector()));
    connect(ui->eCorrector, SIGNAL(cancel()), this, SLOT(cancelCorrector()));

    // table configurations
    model = new SetScoreModel(this);
    ui->tScores->setModel(model);

    customizeTable(ui->tScores, 4, 80, true, 2);
    ui->tScores->setColumnWidth(1, 150);

    ui->tScores->setItemDelegateForColumn(4, new SpinBoxDelegate(-100, 10000, 50, ui->tScores));
}

ScoreForm::~ScoreForm()
{
    delete ui;
}

void ScoreForm::selectCorrector()
{
    ((SetScoreModel*) ui->tScores->model())->setOptions(ui->eCorrector->value(), ui->sDays->value());
    ui->tScores->setFocus();
}

void ScoreForm::cancelCorrector()
{
    ((SetScoreModel*) ui->tScores->model())->setOptions(ui->eCorrector->value(), ui->sDays->value());
    ui->eCorrector->setFocus();
}

void ScoreForm::on_sDays_editingFinished()
{
    ((SetScoreModel*) ui->tScores->model())->setOptions(ui->eCorrector->value(), ui->sDays->value());
}

void ScoreForm::focus()
{
    ui->eCorrector->setFocus();
}
