#include "questionmodule.h"
#include "ui_questionmodule.h"

#include <helper.h>

QuestionModule::QuestionModule(QString question, QString answer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionModule)
{
    ui->setupUi(this);

    ui->eQuestion->setText(question);
    ui->eAnswer->setPlainText(answer);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QuestionModule::~QuestionModule()
{
    delete ui;
}

void QuestionModule::on_bAnswer_clicked()
{
    ui->wAnswer->setVisible(! ui->wAnswer->isVisible());
    refresh();
}

void QuestionModule::refresh(bool collapse)
{
    if (collapse)
        ui->wAnswer->setVisible(false);

    int h = ui->wQuestion->minimumHeight();
    if (ui->wAnswer->isVisible())
    {
        ui->bAnswer->setIcon(QIcon(":/images/minus.png"));
        h += ui->wAnswer->minimumHeight();
    } else
        ui->bAnswer->setIcon(QIcon(":/images/plus.png"));

    resize(width(), h);
}

void QuestionModule::select()
{
    ui->eQuestion->setFocus();
}

QString QuestionModule::question()
{
    return ui->eQuestion->text();
}

QString QuestionModule::answer()
{
    return ui->eAnswer->toPlainText();
}
