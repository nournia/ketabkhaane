#ifndef FORMCHANGEPERMISSIONS_H
#define FORMCHANGEPERMISSIONS_H

#include <QMessageBox>
#include <QSqlQueryModel>

#include <helper.h>
#include <musers.h>

class PermissionModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    int _column;
    Qt::SortOrder _order;

    PermissionModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        sort(1);

        setHeaderData(1, Qt::Horizontal, tr("Name"));
        setHeaderData(2, Qt::Horizontal, tr("Permission"));
        setHeaderData(3, Qt::Horizontal, tr("Password"));
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
   {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        if (index.column() == 2 || index.column() == 3)
            flags |= Qt::ItemIsEditable;
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
        if (!(index.column() == 2 || index.column() == 3))
            return false;

        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        QString msg = "";
        if (index.column() == 2)
            msg = MUsers::setPermission(id , value.toString());
        else if (index.column() == 3)
            msg = MUsers::setPassword(id, value.toString());

        if (! msg.isEmpty())
            QMessageBox::warning(0, QObject::tr("Ketabkhaane"), msg);

        sort(_column, _order);
        return msg.isEmpty();
    }

    static QList<StrPair> getPermissions()
    {
        QList<StrPair> permissions;
        permissions.append(qMakePair(PermissionModel::tr("master"), QString("master")));
        permissions.append(qMakePair(PermissionModel::tr("manager"), QString("manager")));
        permissions.append(qMakePair(PermissionModel::tr("designer"), QString("designer")));
        permissions.append(qMakePair(PermissionModel::tr("operator"), QString("operator")));
        permissions.append(qMakePair(PermissionModel::tr("user"), QString("user")));
        return permissions;
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        if (column == 0) return; _column = column; _order = order;
        QStringList fields = QStringList() << "name" << "permission" << "password";

        QString trs = "case permission ";
        QList<StrPair> permissions = getPermissions();
        for (int i = 0; i < permissions.size(); i++)
            trs += "when '" + permissions.at(i).second + "' then '" + permissions.at(i).first +"'";
        trs += "end";

        QString sql = QString("select users.id, firstname ||' '|| lastname as name, %1 as permission, case when upassword is null then '' else '******' end as password from permissions inner join users on users.id = permissions.user_id order by %2").arg(trs).arg(fields[column-1]);

        if (order == Qt::DescendingOrder)
            sql += " desc";

        setQuery(sql);
    }
};


namespace Ui {
    class FormChangePermissions;
}

class FormChangePermissions : public QWidget
{
    Q_OBJECT

public:
    explicit FormChangePermissions(QWidget *parent = 0);
    ~FormChangePermissions();

    PermissionModel* model;
private:
    Ui::FormChangePermissions *ui;
};



#endif // FORMCHANGEPERMISSIONS_H
