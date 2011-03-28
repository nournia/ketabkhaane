#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <mylineedit.h>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    MyLineEdit *eUsername;
};

#endif // LOGINDIALOG_H
