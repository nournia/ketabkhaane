#include "matchform.h"
#include "ui_matchform.h"

MatchForm::MatchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MatchForm)
{
    ui->setupUi(this);
}

MatchForm::~MatchForm()
{
    delete ui;
}
