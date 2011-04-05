#ifndef DELEGATEPASSWORD_H
#define DELEGATEPASSWORD_H

#include <QLineEdit>
#include <QCryptographicHash>
#include <QItemDelegate>

class DelegatePassword : public QItemDelegate
{
    Q_OBJECT

public:
    DelegatePassword(QObject *parent = 0) : QItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setEchoMode(QLineEdit::Password);
        return lineEdit;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const {}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
        QString value = lineEdit->text(); // QCryptographicHash::hash(lineEdit->text().toUtf8(), QCryptographicHash::Sha1).toHex();
        model->setData(index, value, Qt::EditRole);
    }
};

#endif // DELEGATEPASSWORD_H
