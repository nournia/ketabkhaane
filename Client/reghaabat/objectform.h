#ifndef OBJECTFORM_H
#define OBJECTFORM_H

#include <QDialog>

namespace Ui {
    class ObjectForm;
}

class ObjectForm : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectForm(QWidget *parent = 0);
    ~ObjectForm();

    void editMode(bool edit);

private:
    Ui::ObjectForm *ui;

signals:
    void closeForm();

private slots:
    void selectObject();
    void cancelObject();

};

#endif // OBJECTFORM_H
