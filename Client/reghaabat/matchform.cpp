#include "matchform.h"
#include "ui_matchform.h"

MatchForm::MatchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MatchForm)
{
    ui->setupUi(this);

    eCorrector = new MyLineEdit("select id, firstname || ' ' || lastname as ctitle from users", this);
    ui->lCorrector->addWidget(eCorrector);

    eAuthor = new MyLineEdit("select id, title as ctitle from authors", this);
    ui->lAuthor->addWidget(eAuthor);

    ePublication = new MyLineEdit("select id, title as ctitle from publications", this);
    ui->lPublication->addWidget(ePublication);

    QWidget::setTabOrder(ui->eTitle, eCorrector);
    QWidget::setTabOrder(ui->cState, eAuthor);
    QWidget::setTabOrder(eAuthor, ePublication);

    ui->cType->setFocus();

    MMatches::fillAgeClassCombo(ui->cAgeClass);
    on_cType_currentIndexChanged(0);
}

MatchForm::~MatchForm()
{
    delete ui;
}

void MatchForm::on_cType_currentIndexChanged(int index)
{
    bool intructions = ui->cType->currentIndex() == 0;
    if (intructions)
    {
        ui->cGroup->clear();
        ui->cGroup->addItem(tr("book"));
        ui->cGroup->addItem(tr("multimedia"));

    } else
        MMatches::fillCategoryCombo(ui->cGroup);

    ui->gResource->setVisible(intructions);
}

void MatchForm::on_bNewQuestion_clicked()
{
    // check for last question for
    if (qModules.size() >= 1 && qModules.at(qModules.size()-1)->question == "")
        qModules.at(qModules.size()-1)->select();
    else // create new one
    {
        for (int i = 0; i < qModules.size(); i++)
            qModules.at(i)->refresh(true);

        QuestionModule* module = new QuestionModule("", "", this);
        qModules.append(module);
        ui->lQuestions->layout()->addWidget(module);
        module->select();
    }
}


