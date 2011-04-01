#include "questionmodule.h"
#include "ui_questionmodule.h"

QuestionModule::QuestionModule(QString q, QString a, QWidget *parent) :
    QWidget(parent), question(q), answer(a),
    ui(new Ui::QuestionModule)
{
    ui->setupUi(this);

    ui->eQuestion->setText(question);
    ui->eAnswer->setPlainText(answer);
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

    setMaximumHeight(h);
}

void QuestionModule::select()
{
    ui->eQuestion->setFocus();
}
