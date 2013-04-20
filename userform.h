#ifndef USERFORM_H
#define USERFORM_H

#include <QWidget>
#include <musers.h>

namespace Ui {
    class UserForm;
}

class UserForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserForm(QWidget *parent = 0);
    ~UserForm();

    void editMode(bool edit);
    void checkReadOnly();

private:
    Ui::UserForm *ui;

signals:
    void closeForm();

private slots:
    void selectUser();
    void cancelUser();

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // USERFORM_H
