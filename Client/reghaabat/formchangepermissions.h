#ifndef FORMCHANGEPERMISSIONS_H
#define FORMCHANGEPERMISSIONS_H

#include <QSqlQueryModel>
#include <delegatecombobox.h>

//class PermissionModel : public QSqlQueryModel
//{
//    Q_OBJECT

//public:
//    QString sql;
//    PermissionModel(QObject *parent = 0) : QSqlQueryModel(parent) { refresh(); }

//    Qt::ItemFlags flags(const QModelIndex &index) const
//   {
//        Qt::ItemFlags flags = QSqlQueryModel::flags(index);
//        if (index.column() == 2 || index.column() == 3)
//            flags |= Qt::ItemIsEditable;
//        return flags;
//    }

//    bool setData(const QModelIndex &index, const QVariant &value, int role)
//    {
//        if (!(index.column() == 2 || index.column() == 3))
//            return false;

////        int id = data(QSqlQueryModel::index(index.row(), 0)).toInt();

////        clear();

////        bool ok = false;
////        QSqlQuery qry;
////        if (index.column() == 2)
////            ok = qry.exec(QString("update permissions set permission = %1 where user_id").arg(value.toString()).arg(id));
////        else if (index.column() == 3)
////            ok = true;

////        refresh();
////        return ok;
//    }

//private:
//    void refresh()
//    {
//        setQuery("select users.id, firstname ||' '|| lastname, permission, '' as password from permissions inner join users on users.id = permissions.user_id");
//        setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));


//    }
//};


namespace Ui {
    class FormChangePermissions;
}

class FormChangePermissions : public QWidget
{
    Q_OBJECT

public:
    explicit FormChangePermissions(QWidget *parent = 0);
    ~FormChangePermissions();

//    PermissionModel* model;
    DelegateComboBox* permissionDelegate;
private:
    Ui::FormChangePermissions *ui;
};



#endif // FORMCHANGEPERMISSIONS_H
