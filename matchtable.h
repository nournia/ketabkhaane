#ifndef MATCHTABLE_H
#define MATCHTABLE_H

#include <QMessageBox>
#include <QSqlQueryModel>

#include <mmatches.h>
#include <matchform.h>
#include <viewerform.h>

class MatchListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    QMap<QModelIndex, QVariant> edited;
    QList<StrPair> ageclasses;
    int column;
    Qt::SortOrder order;

    MatchListModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        ageclasses = MMatches::ageclasses(true);
        sort(1);

        setHeaderData(1, Qt::Horizontal, tr("Title"));
        setHeaderData(2, Qt::Horizontal, tr("Kind"));
        setHeaderData(3, Qt::Horizontal, tr("Age Class"));
        setHeaderData(4, Qt::Horizontal, tr("Answers"));
        setHeaderData(5, Qt::Horizontal, tr("Score"));
        setHeaderData(6, Qt::Horizontal, tr("State"));
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        if (index.column() == 5 || index.column() == 6)
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
        if (!(index.column() == 5 || index.column() == 6))
            return false;

        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        StrMap match;
        QList<StrPair> questions;
        MMatches::get(id, match, questions);

        QString val = value.toString();
        switch (index.column()) {
            case 5:
                match["score"] = val;
            break;
            case 6:
                match["current_state"] = val;
            break;
        }

        QString msg = MMatches::set(id, match, questions);

        if (! msg.isEmpty())
            QMessageBox::warning(0, QObject::tr("Ketabkhaane"), msg);
        else {
            if (index.column() == 6)
                edited[index] = QObject::tr(val.toLatin1());
            else
                edited[index] = val;
        }

        return msg.isEmpty();
    }

    void sort(int col, Qt::SortOrder ord = Qt::AscendingOrder)
    {
        edited.clear(); if (col == 0) return; column = col; order = ord;
        QStringList fields = QStringList() << "title" << "kind" << "ageclass" << "answer_ratio" << "score" << "state";

        QString sql = QString() +
            "select matches.id, matches.title as title, "+
            "ifnull(categories.title, types.title) as kind, "+
            "ageclasses.title as ageclass, "+
            "case when category_id is null then trim(substr(content, 1, 5)) else '-' end as answer_ratio, "+
            "supports.score, "+
            "case supports.current_state when 'active' then '"+ QObject::tr("active") +"' when 'imported' then '"+ QObject::tr("imported") +"' when 'disabled' then '"+ QObject::tr("disabled") +"' end as state "+
            "from matches inner join supports on matches.id = supports.match_id inner join ageclasses on matches.ageclass = ageclasses.id left join categories on categories.id = matches.category_id left join objects on matches.object_id = objects.id left join types on objects.type_id = types.id "+
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
    void refresh();

    MatchListModel* model;
    ViewerForm* viewer;

private:
    Ui::MatchTable *ui;


private slots:
    void on_bMatchList_clicked();
    void on_bEditMatch_clicked();
};

#endif // MATCHTABLE_H
