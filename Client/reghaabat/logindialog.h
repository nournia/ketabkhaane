#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <mylineedit.h>
#include <mainwindow.h>

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

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void selectUser();
};

#endif // LOGINDIALOG_H
