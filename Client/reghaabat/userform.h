#ifndef USERFORM_H
#define USERFORM_H

#include <QWidget>

#include <MUsers.h>

namespace Ui {
    class UserForm;
}

class UserForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserForm(QWidget *parent = 0);
    ~UserForm();

    void edit(QString userId);

private:
    Ui::UserForm *ui;
};

#endif // USERFORM_H
