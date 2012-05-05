#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <helper.h>
#include <QItemDelegate>
#include <QComboBox>

class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    QList<StrPair> list;

    ComboBoxDelegate(QList<StrPair> data, QObject *parent = 0) : QItemDelegate(parent), list(data) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QComboBox* editor = new QComboBox(parent);
        for (int i = 0; i < list.size(); i++)
            editor->addItem(list.at(i).first, list.at(i).second);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findText(value));
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        QString value = comboBox->itemData(comboBox->currentIndex()).toString();
        model->setData(index, value, Qt::EditRole);
    }
};

#endif // COMBOBOXDELEGATE_H
