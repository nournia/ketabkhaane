#include "questionmodule.h"
#include "ui_questionmodule.h"

QuestionModule::QuestionModule(QString question, QString answer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuestionModule)
{
    ui->setupUi(this);

    ui->eQuestion->setText(question);
    ui->eAnswer->setPlainText(answer);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    refresh(false);
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
        h += ui->wAnswer->minimumHeight();

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
