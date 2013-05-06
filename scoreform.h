#ifndef SCOREFORM_H
#define SCOREFORM_H

#include <QMessageBox>
#include <QSqlQueryModel>
#include <mmatches.h>

class SetScoreModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    int _column;
    Qt::SortOrder _order;
    QMap<QModelIndex, QVariant> edited;

    SetScoreModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        _column = 2;
        setOptions("", 1);

        setHeaderData(1, Qt::Horizontal, tr("Name"));
        setHeaderData(2, Qt::Horizontal, tr("Title"));
        setHeaderData(3, Qt::Horizontal, tr("Max Score"));
        setHeaderData(4, Qt::Horizontal, tr("Score"));
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        if (index.column() == 4)
            flags |= Qt::ItemIsEditable;
        return flags;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::TextAlignmentRole && (index.column() == 3 || index.column() == 4))
            return Qt::AlignCenter;
        else
        {
            if (edited.contains(index) && (role == Qt::DisplayRole || role == Qt::EditRole))
                return edited[index];

            return QSqlQueryModel::data(index, role);
        }
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!(index.column() == 4))
            return false;

        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        QString msg = "";

        if (index.column() == 4)
            msg = MMatches::correct(id, value.toString());

        if (! msg.isEmpty())
            QMessageBox::warning(0, QObject::tr("Ketabkhaane"), msg);
        else
            edited[index] = value;

        return msg.isEmpty();
    }

    QString corrector, days;
    void setOptions(QString c, int d)
    {
        corrector = c;
        days = QString("-%1 days").arg(d);

        sort(_column, _order);
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        edited.clear(); if (column == 0) return; _column = column; _order = order;
        QStringList fields = QStringList() << "name" << "title" << "maxscore" << "score";

        QString tmp = "-1";
        if (! corrector.isEmpty())
            tmp = corrector;

        QString sql = QString("select answers.id, firstname||' '||lastname as name, matches.title, supports.score as maxscore, round(answers.rate * supports.score) as score from answers inner join supports on answers.match_id = supports.match_id inner join users on answers.user_id = users.id inner join matches on answers.match_id = matches.id where received_at is not null and (corrected_at is null or corrected_at >= date('now', '%3')) and answers.delivered_at > (select started_at from library) and  corrector_id = %1 order by %2").arg(tmp).arg(fields[column-1]).arg(days);

        if (order == Qt::DescendingOrder)
            sql += " desc";

        setQuery(sql);
    }
};



#include <QWidget>

namespace Ui {
    class ScoreForm;
}

class ScoreForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreForm(QWidget *parent = 0);
    ~ScoreForm();

    void focus();

    SetScoreModel* model;

private:
    Ui::ScoreForm *ui;

private slots:
    void on_sDays_editingFinished();
    void selectCorrector();
    void cancelCorrector();
};

#endif // SCOREFORM_H
