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

    QString userId; // user under edit
    void edit(QString userId);
    void clear();

private:
    Ui::UserForm *ui;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // USERFORM_H
