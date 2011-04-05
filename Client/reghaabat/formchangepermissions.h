#ifndef FORMCHANGEPERMISSIONS_H
#define FORMCHANGEPERMISSIONS_H

#include <QSqlQueryModel>
#include <delegatecombobox.h>

class PermissionModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    QString sql;
    PermissionModel(QObject *parent = 0) : QSqlQueryModel(parent) { refresh(); }

    Qt::ItemFlags flags(const QModelIndex &index) const
   {
        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        if (index.column() == 2 || index.column() == 3)
            flags |= Qt::ItemIsEditable;
        return flags;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!(index.column() == 2 || index.column() == 3))
            return false;

        int id = data(QSqlQueryModel::index(index.row(), 0)).toInt();

        clear();

        bool ok = false;
        QSqlQuery qry;
        if (index.column() == 2)
            ok = qry.exec(QString("update permissions set permission = '%1' where user_id = %2").arg(value.toString()).arg(id));
        else if (index.column() == 3)
            ok = true;

        refresh();
        return ok;
    }

    static QList<StrPair> getPermissions()
    {
        QList<StrPair> permissions;
        permissions.append(qMakePair(PermissionModel::tr("admin"), QString("admin")));
        permissions.append(qMakePair(PermissionModel::tr("master"), QString("master")));
        permissions.append(qMakePair(PermissionModel::tr("manager"), QString("manager")));
        permissions.append(qMakePair(PermissionModel::tr("designer"), QString("designer")));
        permissions.append(qMakePair(PermissionModel::tr("operator"), QString("operator")));
        permissions.append(qMakePair(PermissionModel::tr("user"), QString("user")));
        return permissions;
    }

private:
    void refresh()
    {
        QString trs = "case permission ";
        QList<StrPair> permissions = getPermissions();
        for (int i = 0; i < permissions.size(); i++)
            trs += "when '" + permissions.at(i).second + "' then '" + permissions.at(i).first +"'";
        trs += "end";

        setQuery("select users.id, firstname ||' '|| lastname, "+ trs +", '' as password from permissions inner join users on users.id = permissions.user_id");
        setHeaderData(1, Qt::Horizontal, tr("Name"));
        setHeaderData(2, Qt::Horizontal, tr("Permission"));
        setHeaderData(3, Qt::Horizontal, tr("Password"));
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
    DelegateComboBox* permissionDelegate;
private:
    Ui::FormChangePermissions *ui;
};



#endif // FORMCHANGEPERMISSIONS_H
