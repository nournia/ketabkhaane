#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <titleedit.h>

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
    TitleEdit *titleEdit;
};

#endif // LOGINDIALOG_H
