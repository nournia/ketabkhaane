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
    if (ui->cType->currentIndex() == 0)
    {
        ui->cGroup->clear();
        ui->cGroup->addItem(tr("book"));
        ui->cGroup->addItem(tr("multimedia"));

    } else
        MMatches::fillCategoryCombo(ui->cGroup);
}
