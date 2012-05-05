#ifndef DIALOGCHANGEPASSWORD_H
#define DIALOGCHANGEPASSWORD_H

#include <QDialog>

namespace Ui {
    class DialogChangePassword;
}

class DialogChangePassword : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangePassword(QWidget *parent = 0);
    ~DialogChangePassword();

private:
    Ui::DialogChangePassword *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // DIALOGCHANGEPASSWORD_H
