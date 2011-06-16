#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include <QLineEdit>
#include <QItemDelegate>

class LineEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    bool password;

    LineEditDelegate(bool _password, QObject *parent = 0) : QItemDelegate(parent), password(_password) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        if (password)
            lineEdit->setEchoMode(QLineEdit::Password);
        return lineEdit;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
        if (!password) lineEdit->setText(value);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
        QString value = lineEdit->text();
        model->setData(index, value, Qt::EditRole);
    }
};

#endif // LINEEDITDELEGATE_H
