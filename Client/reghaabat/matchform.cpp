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

    select("311013");
}

#include <QDebug>
void MatchForm::select(QString id)
{
    matchId = id;

    StrMap match = MMatches::get(id);

    ui->eTitle->setText(match["title"].toString());
    ui->cAgeClass->setCurrentIndex(ui->cAgeClass->findData(match["ageclass"]));
    eCorrector->setText(match["corrector"].toString());
    ui->sScore->setValue(match["score"].toInt());

    QString state = match["current_state"].toString();
    if (state == "active")
        ui->cState->setCurrentIndex(0);
    else if (state == "imported")
        ui->cState->setCurrentIndex(1);
    else if (state == "disabled")
        ui->cState->setCurrentIndex(2);

    if (match["category_id"] == "")
    {
        ui->cType->setCurrentIndex(0); // Questions

        QString kind = match["kind"].toString();
        if (kind == "book")
            ui->cGroup->setCurrentIndex(0);
        else if (kind == "multimedia")
            ui->cGroup->setCurrentIndex(1);

        eAuthor->setText(match["author"].toString());
        ePublication->setText(match["publication"].toString());

        // questions
        QSqlQuery questions = MMatches::getQuestions(id);
        while (questions.next())
        {
            QuestionModule* module = new QuestionModule(questions.value(0).toString(), questions.value(1).toString(), this);
            qModules.append(module);
            module->refresh(true);
            ui->lQuestions->layout()->addWidget(module);
        }
    }
    else
    {
        ui->cType->setCurrentIndex(1); // Instructions
        ui->cGroup->setCurrentIndex(ui->cGroup->findData(match["category_id"].toString()));
        ui->eContent->setHtml(match["content"].toString());
    }
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
    ui->sQuestions->setVisible(intructions);
    ui->eContent->setVisible(! intructions);
}

void MatchForm::on_bNewQuestion_clicked()
{
    // check for last question for
    if (qModules.size() >= 1 && qModules.at(qModules.size()-1)->question() == "")
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



