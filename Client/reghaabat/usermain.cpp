#include "usermain.h"
#include "ui_usermain.h"

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

#include <helper.h>


UserMain::UserMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserMain)
{
    ui->setupUi(this);
}

UserMain::~UserMain()
{
    delete ui;
}

void UserMain::select(QString userId)
{
    // clean gMatches
    QLayoutItem *child;
    if (ui->gMatches->layout())
    while ((child = ui->gMatches->layout()->takeAt(0)) != 0)
         delete child->widget();
    delete ui->gMatches->layout();


    QSqlQuery qry;
    qry.exec(QString("select match_id, matches.title from answers inner join matches on answers.match_id = matches.id where user_id = %1 and received_at is null;").arg(userId));

    QFormLayout* lay = new QFormLayout(ui->gMatches);
    for (int i = 1; qry.next(); i++)
    {
        QLabel* label = new QLabel(qry.value(1).toString(), ui->gMatches);
        label->setLayoutDirection(Qt::LeftToRight);
        lay->setWidget(i, QFormLayout::FieldRole, label);

        QPushButton* button = new QPushButton(tr("Receive"), ui->gMatches);
        button->setMaximumWidth(40);
        lay->setWidget(i, QFormLayout::LabelRole, button);
    }

    ui->gMatches->setLayout(lay);
}
