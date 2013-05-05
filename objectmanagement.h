#ifndef OBJECTMANAGEMENT_H
#define OBJECTMANAGEMENT_H

#include <QDialog>
#include <QSqlQueryModel>

class ObjectsModel : public QSqlQueryModel
{
    Q_OBJECT

public:

    ObjectsModel(QObject *parent = 0) : QSqlQueryModel(parent)
    {
        sort(1);

        setHeaderData(1, Qt::Horizontal, tr("Label"));
        setHeaderData(2, Qt::Horizontal, tr("Title"));
        setHeaderData(3, Qt::Horizontal, tr("Author"));
        setHeaderData(4, Qt::Horizontal, tr("Publication"));
        setHeaderData(5, Qt::Horizontal, tr("Type"));
        setHeaderData(6, Qt::Horizontal, tr("Branch"));
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::TextAlignmentRole && (index.column() != 2))
            return Qt::AlignCenter;

        return QSqlQueryModel::data(index, role);
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        if (column == 0) return;
        QStringList fields = QStringList() << "label" << "title" << "author" << "publication" << "type" << "branch";

        QString sql =
            "select objects.id, belongs.label, objects.title as title, authors.title as author, publications.title as publication, types.title as type, _branches.title as branch "
            "from objects inner join belongs on objects.id = belongs.object_id inner join types on objects.type_id = types.id left join authors on objects.author_id = authors.id left join publications on objects.publication_id = publications.id "
            "left join (select branches.id, case when branches.title != '' then roots.title ||' - '|| branches.title else roots.title end as title from branches inner join roots on branches.root_id = roots.id) as _branches on belongs.branch_id = _branches.id "
            "order by " + fields[column-1];

        if (order == Qt::DescendingOrder)
            sql += " desc";

        setQuery(sql);
    }
};


namespace Ui {
    class ObjectManagement;
}

class ObjectManagement : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectManagement(QWidget *parent = 0);
    ~ObjectManagement();

    ObjectsModel* model;

private slots:
    void on_bPreviewList_clicked();

    void on_bPreviewLabels_clicked();

    void on_bPrintLabels_clicked();

    void on_bPrintList_clicked();

    void on_bEditObject_clicked();

private:
    Ui::ObjectManagement *ui;
};

#endif // OBJECTMANAGEMENT_H
