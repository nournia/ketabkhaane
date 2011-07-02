#ifndef USERFORM_H
#define USERFORM_H

#include <QWidget>
#include <mylineedit.h>
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

    QString importedId;
    MyLineEdit* eUser;
private:
    Ui::UserForm *ui;

signals:
    void closeForm();

private slots:
    void on_gImport_clicked();
    void on_bImport_clicked();
    void selectUser();
    void cancelUser();

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // USERFORM_H
