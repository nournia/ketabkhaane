#include "scoreform.h"
#include "ui_scoreform.h"

ScoreForm::ScoreForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScoreForm)
{
    ui->setupUi(this);

    // add corrector edit
    eCorrector = new MyLineEdit("select id, firstname || ' ' || lastname as ctitle from users where id in (select corrector_id from supports)", this);
    ui->lCorrector->addWidget(eCorrector);
    connect(eCorrector, SIGNAL(select()), this, SLOT(selectCorrector()));
    connect(eCorrector, SIGNAL(cancel()), this, SLOT(cancelCorrector()));
}

ScoreForm::~ScoreForm()
{
    delete ui;
}

void ScoreForm::selectCorrector()
{

}

void ScoreForm::cancelCorrector()
{

}
