#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include <QDialog>

#include <mainwindow.h>
#include <jalali.h>
#include <musers.h>

#include <QSqlQueryModel>

class UsersModel : public QSqlQueryModel
{
    Q_OBJECT

public:

    UsersModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        sort(1);

        setHeaderData(1, Qt::Horizontal, tr("Name"));
        setHeaderData(2, Qt::Horizontal, tr("Object"));
        setHeaderData(3, Qt::Horizontal, tr("Borrow Date"));
        setHeaderData(4, Qt::Horizontal, tr("Days"));
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::TextAlignmentRole && (index.column() != 2))
            return Qt::AlignCenter;

        if (role == Qt::DisplayRole && index.column() == 3)
            return toJalali(QSqlQueryModel::data(index, role).toDate());

        return QSqlQueryModel::data(index, role);
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        if (column == 0) return;
        QStringList fields = QStringList() << "user" << "object" << "date" << "days";

        QString sql =
            "select borrows.id, firstname||' '||lastname as user, objects.title as object, ifnull(renewed_at, delivered_at) as date, julianday(date('now')) - julianday(date(ifnull(renewed_at, delivered_at))) as days "
            "from borrows inner join objects on borrows.object_id = objects.id inner join users on borrows.user_id = users.id "
            "where received_at is null "+ MUsers::getGenderCondition() +
            "order by " + fields[column-1];

        if (order == Qt::DescendingOrder)
            sql += " desc";

        setQuery(sql);
    }
};


namespace Ui {
    class UserManagement;
}

class UserManagement : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagement(QWidget *parent = 0);
    ~UserManagement();

    UsersModel* model;
    ViewerForm* viewer;

private:
    Ui::UserManagement *ui;

private slots:
    void on_bPreviewLogs_clicked();
    void on_bPreviewScoreList_clicked();
    void on_bScoreList_clicked();
    void on_bLogs_clicked();
};

#endif // USERMANAGEMENT_H
