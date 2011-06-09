#ifndef MATCHTABLE_H
#define MATCHTABLE_H

#include <QDialog>

#include <QSqlQueryModel>
#include <mmatches.h>
#include <matchform.h>

class MatchListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
//    QString sql;
    MatchListModel(QObject *parent = 0) : QSqlQueryModel(parent) { refresh(); }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
//        if (index.column() == 4)
//            flags |= Qt::ItemIsEditable;
        return flags;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if ((index.column() != 1) && role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        else
            return QSqlQueryModel::data(index, role);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
//        if (!(index.column() == 4))
//            return false;

//        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        QString msg = "";

//        if (index.column() == 4)
//            msg = MMatches::setScore(id, value.toString());

//        if (! msg.isEmpty())
//            QMessageBox::warning(0, QObject::tr("Reghaabat"), msg);

//        clear();

//        refresh();
        return msg.isEmpty();
    }

    void refresh()
    {
        QString sql = QString() +
            "select matches.id, matches.title as title, ageclasses.title as ageclass, supports.score,"+
            "ifnull(categories.title, case resources.kind when 'book' then '"+ MatchForm::tr("book") +"' when 'multimedia' then '"+ MatchForm::tr("multimedia") +"' end) as kind, "+
            "case supports.current_state when 'active' then '"+ QObject::tr("active") +"' when 'imported' then '"+ MatchForm::tr("imported") +"' when 'disabled' then '"+ MatchForm::tr("disabled") +"' end as state "+
            "from matches inner join supports on matches.id = supports.match_id inner join ageclasses on matches.ageclass = ageclasses.id left join categories on categories.id = matches.category_id left join resources on matches.resource_id = resources.id "+
            "order by kind, supports.score desc";

        setQuery(sql);
        setHeaderData(1, Qt::Horizontal, tr("Title"));
        setHeaderData(2, Qt::Horizontal, tr("Age Class"));
        setHeaderData(3, Qt::Horizontal, tr("Score"));
        setHeaderData(4, Qt::Horizontal, tr("Kind"));
        setHeaderData(5, Qt::Horizontal, tr("State"));
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
