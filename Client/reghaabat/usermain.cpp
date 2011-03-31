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
    QSqlQuery qry;
    qry.exec(QString("select match_id, matches.title from answers inner join matches on answers.match_id = matches.id where user_id = %1 and received_at is null;").arg(userId));

    QFormLayout* lay = new QFormLayout();
    lay->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    lay->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    for (int i = 1; qry.next(); i++)
    {
        QLabel* label = new QLabel(qry.value(1).toString(), this);
        lay->setWidget(i, QFormLayout::LabelRole, label);

        QPushButton* button = new QPushButton(tr("Deliver"), this);
        button->setMaximumSize(50, 40);
        lay->setWidget(i, QFormLayout::FieldRole, button);
    }

    delete ui->gMatches->layout();
    ui->gMatches->setLayout(lay);
}
