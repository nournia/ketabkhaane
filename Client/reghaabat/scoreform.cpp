#include "scoreform.h"
#include "ui_scoreform.h"

#include <spinboxdelegate.h>

ScoreForm::ScoreForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScoreForm)
{
    ui->setupUi(this);

    // add corrector edit
    eCorrector = new MyLineEdit("select id, firstname ||' '|| lastname as ctitle from users where id in (select corrector_id from supports)", this);
    ui->lCorrector->addWidget(eCorrector);
    connect(eCorrector, SIGNAL(select()), this, SLOT(selectCorrector()));
    connect(eCorrector, SIGNAL(cancel()), this, SLOT(cancelCorrector()));

    // table configurations
    model = new SetScoreModel(this);
    ui->tScores->setModel(model);

    QWidget::setTabOrder(eCorrector, ui->tScores);

    ui->tScores->setColumnHidden(0, true);
    ui->tScores->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    ui->tScores->setColumnWidth(1, 150);
    ui->tScores->verticalHeader()->setDefaultSectionSize(22);
    ui->tScores->verticalHeader()->setVisible(false);

    ui->tScores->setItemDelegateForColumn(4, new SpinBoxDelegate(-100, 10000, 50, ui->tScores));

    cancelCorrector();
}

ScoreForm::~ScoreForm()
{
    delete ui;
}

void ScoreForm::selectCorrector()
{
    ((SetScoreModel*) ui->tScores->model())->setCorrector(eCorrector->value());
    ui->tScores->setFocus();
}

void ScoreForm::cancelCorrector()
{
    ((SetScoreModel*) ui->tScores->model())->setCorrector("");
    eCorrector->setFocus();
}
