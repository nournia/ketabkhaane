#ifndef MATCHTABLE_H
#define MATCHTABLE_H

#include <QMessageBox>
#include <QSqlQueryModel>

#include <mmatches.h>
#include <matchform.h>

class MatchListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    QMap<QModelIndex, QVariant> edited;

    MatchListModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        sort(1);

        setHeaderData(1, Qt::Horizontal, tr("Title"));
        setHeaderData(2, Qt::Horizontal, tr("Kind"));
        setHeaderData(3, Qt::Horizontal, tr("Age Class"));
        setHeaderData(4, Qt::Horizontal, tr("Score"));
        setHeaderData(5, Qt::Horizontal, tr("Answers"));
        setHeaderData(6, Qt::Horizontal, tr("State"));
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        if (/*index.column() == 1 || */index.column() == 3 || index.column() == 4 || index.column() == 6)
            flags |= Qt::ItemIsEditable;
        return flags;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::TextAlignmentRole && (index.column() != 1))
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
        if (!(index.column() == 1 || index.column() == 3 || index.column() == 4 || index.column() == 6))
            return false;

        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        StrMap match;
        QList<StrPair> questions;
        MMatches::get(id, match, questions);

        switch (index.column())
        {
        case 1:
            match["title"] = value.toString();
        break;
        case 3:
            match["ageclass"] = value.toString();
        break;
        case 4:
            match["score"] = value.toString();
        break;
        case 6:
            match["current_state"] = value.toString();
        break;
        }

        QString msg = MMatches::set(id, match, questions);

        if (! msg.isEmpty())
            QMessageBox::warning(0, QObject::tr("Reghaabat"), msg);
        else
            edited[index] = value;

        return msg.isEmpty();
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        edited.clear(); if (column == 0) return;
        QStringList fields = QStringList() << "title" << "kind" << "ageclass" << "score" << "answer_ratio" << "state";

        QString sql = QString() +
            "select matches.id, matches.title as title, "+
            "ifnull(categories.title, case resources.kind when 'book' then '"+ MatchForm::tr("book") +"' when 'multimedia' then '"+ MatchForm::tr("multimedia") +"' end) as kind, "+
            "ageclasses.title as ageclass, "+
            "supports.score, "+
            "ifnull(answer_ratio, '-') as answer_ratio, "+
            "case supports.current_state when 'active' then '"+ QObject::tr("active") +"' when 'imported' then '"+ QObject::tr("imported") +"' when 'disabled' then '"+ QObject::tr("disabled") +"' end as state "+
            "from matches inner join supports on matches.id = supports.match_id inner join ageclasses on matches.ageclass = ageclasses.id left join categories on categories.id = matches.category_id left join resources on matches.resource_id = resources.id "+
            "left join (select match_id, count(question)||' / '||count(answer) as answer_ratio from questions group by match_id) as t_questions on matches.id = t_questions.match_id "+
            "order by " + fields[column-1];

        if (order == Qt::DescendingOrder)
            sql += " desc";

        setQuery(sql);
    }
};

namespace Ui {
    class MatchTable;
}

class MatchTable : public QDialog
{
    Q_OBJECT

public:
    explicit MatchTable(QWidget *parent = 0);
    ~MatchTable();

    MatchListModel* model;

private:
    Ui::MatchTable *ui;
};

#endif // MATCHTABLE_H
