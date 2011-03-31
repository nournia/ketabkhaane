#include "matchrow.h"
#include "ui_matchrow.h"

#include <QSqlQuery>
#include <QDateTime>

MatchRow::MatchRow(QString uid, QString mid, QString title, QWidget *parent) :
    QWidget(parent), userId(uid), matchId(mid),
    ui(new Ui::MatchRow)
{
    ui->setupUi(this);

    ui->label->setText(title);
}

MatchRow::~MatchRow()
{
    delete ui;
}

void MatchRow::on_button_clicked()
{
    QSqlQuery qry;
    qry.prepare("update answers set received_at = ? where user_id = ? and match_id = ?");
    qry.addBindValue(QDateTime::currentDateTime());
    qry.addBindValue(userId);
    qry.addBindValue(matchId);
    qry.exec();

    delete this;
}
