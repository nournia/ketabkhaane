#include "matchform.h"
#include "ui_matchform.h"

#include <QMessageBox>
#include <mainwindow.h>

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

    // fill cState
    ui->cState->addItem(tr("active"), "active");
    ui->cState->addItem(tr("imported"), "imported");
    ui->cState->addItem(tr("disabled"), "disabled");

    // fill cAgeClass
    MMatches::fillAgeClassCombo(ui->cAgeClass);

    on_cType_currentIndexChanged(0);
}

void MatchForm::select(QString id)
{
    matchId = id;

    QList<StrPair> questions;
    StrMap match;
    MMatches::get(id, match, questions);

    ui->eTitle->setText(match["title"].toString());
    eCorrector->setText(match["corrector"].toString());
    ui->sScore->setValue(match["score"].toInt());
    ui->cAgeClass->setCurrentIndex(ui->cAgeClass->findData(match["ageclass"]));
    ui->cState->setCurrentIndex(ui->cState->findData(match["current_state"]));

    if (match["category_id"] == "")
    {
        ui->cType->setCurrentIndex(0); // Questions
        ui->cGroup->setCurrentIndex(ui->cGroup->findData(match["kind"]));
        eAuthor->setText(match["author"].toString());
        ePublication->setText(match["publication"].toString());

        // questions
        for (int i = 0; i < questions.size(); i++)
        {
            QuestionModule* module = new QuestionModule(questions.at(i).first, questions.at(i).second, this);
            qModules.append(module);
            module->refresh(true);
            ui->lQuestions->layout()->addWidget(module);
        }
    }
    else
    {
        ui->cType->setCurrentIndex(1); // Instructions
        ui->cGroup->setCurrentIndex(ui->cGroup->findData(match["category_id"].toString()));
        ui->eContent->setPlainText(match["content"].toString());
    }
}

void MatchForm::clear()
{
    matchId = "";
    ((MainWindow*)parent())->clear();
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
        ui->cGroup->addItem(tr("book"), "book");
        ui->cGroup->addItem(tr("multimedia"), "multimedia");
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

void MatchForm::on_buttonBox_accepted()
{
    StrMap match;
    QList<StrPair> questions;

    match["title"] = ui->eTitle->text();
    match["corrector"] = eCorrector->value();
    match["score"] = ui->sScore->value();
    match["ageclass"] = ui->cAgeClass->itemData(ui->cAgeClass->currentIndex());
    match["current_state"] = ui->cState->itemData(ui->cState->currentIndex());

    if (ui->cType->currentIndex() == 0)
    {
        match["kind"] = ui->cGroup->itemData(ui->cGroup->currentIndex());
        match["author"] = eAuthor->value();
        match["publication"] = ePublication->value();

        // questions
        for (int i = 0; i < qModules.size(); i++)
            if (qModules.at(i)->question() != "")
                questions.append(qMakePair(qModules.at(i)->question(), qModules.at(i)->answer()));
    }
    else
    {
        match["category_id"] = ui->cGroup->itemData(ui->cGroup->currentIndex());
        match["content"]  = ui->eContent->toPlainText();
    }

    QString msg = MMatches::set(matchId, match, questions);

    // there isn't any error
    if (msg == "")
        clear();
    else
        QMessageBox::warning(this, QApplication::tr("Reghaabat"), msg);
}

void MatchForm::on_buttonBox_rejected()
{
    clear();
}
