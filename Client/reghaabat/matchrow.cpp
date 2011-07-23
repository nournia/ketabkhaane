#include "matchrow.h"
#include "ui_matchrow.h"

#include <mmatches.h>
#include <jalali.h>

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
    MMatches::receive(userId, matchId);

    // receive book in library
    bool ok; QSqlDatabase library = Connector::connectLibrary(ok);
    if (ok)
    {
        QSqlQuery qryObj;
        qryObj.exec("select label from matches inner join objects on matches.resource_id = objects.resource_id where matches.id = "+ matchId);

        if (qryObj.next())
        {
            QSqlQuery qry(library);
            qry.prepare("update cash set Date_Bargasht = ?, Login_of_Bargasht = ? where UserID = ? AND ObjectID = ? AND Date_Bargasht is null");
            qry.addBindValue(toJalali(QDate::currentDate()));
            qry.addBindValue(Reghaabat::instance()->userId);
            qry.addBindValue(userId);
            qry.addBindValue(qryObj.value(0).toString());
            qry.exec();
        }
    }

    delete this;
}
