#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <helper.h>
#include <QItemDelegate>
#include <QSpinBox>

class SpinBoxDelegate : public QItemDelegate
{
public:
    int min, max, step;

    SpinBoxDelegate(int _min, int _max, int _step, QObject *parent = 0) : QItemDelegate(parent), min(_min), max(_max), step(_step) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSpinBox* editor = new QSpinBox(parent);
        editor->setMaximum(max);
        editor->setMinimum(min);
        editor->setSingleStep(step);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value.toInt());
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        model->setData(index, (float)spinBox->value(), Qt::EditRole);
    }
};

#endif // SPINBOXDELEGATE_H
