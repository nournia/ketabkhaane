#ifndef FORMCHANGEPERMISSIONS_H
#define FORMCHANGEPERMISSIONS_H

#include <QMessageBox>

#include <helper.h>
#include <QSqlQueryModel>
#include <musers.h>

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

        QString id = data(QSqlQueryModel::index(index.row(), 0)).toString();

        clear();

        refresh();

        bool ok = false;
        QSqlQuery qry;
        if (index.column() == 2)
            ok = qry.exec(QString("update permissions set permission = '%1' where user_id = %2").arg(value.toString()).arg(id));
        else if (index.column() == 3)
        {
            QString msg = MUsers::setPassword(id, value.toString());
            if (! msg.isEmpty())
                QMessageBox::warning(0, QObject::tr("Reghaabat"), msg);
            ok = msg.isEmpty();
        }

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

    void refresh()
    {
        QString trs = "case permission ";
        QList<StrPair> permissions = getPermissions();
        for (int i = 0; i < permissions.size(); i++)
            trs += "when '" + permissions.at(i).second + "' then '" + permissions.at(i).first +"'";
        trs += "end";

        setQuery("select users.id, firstname ||' '|| lastname, "+ trs +", case when upassword is null then '' else '******' end from permissions inner join users on users.id = permissions.user_id");
        setHeaderData(1, Qt::Horizontal, tr("Name"));
        setHeaderData(2, Qt::Horizontal, tr("Permission"));
        setHeaderData(3, Qt::Horizontal, tr("Password"));
    }
};


#include <delegatecombobox.h>
#include <delegatepassword.h>
#include <mylineedit.h>

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

    MyLineEdit* eUser;
private:
    Ui::FormChangePermissions *ui;

private slots:
    void on_bAdd_clicked();
    void selectUser();
    void cancelUser();
};



#endif // FORMCHANGEPERMISSIONS_H
