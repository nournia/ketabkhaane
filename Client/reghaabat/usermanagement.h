#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include <QDialog>

#include <mainwindow.h>


namespace Ui {
    class UserManagement;
}

class UserManagement : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagement(QWidget *parent = 0);
    ~UserManagement();

    ViewerForm* viewer;
private:
    Ui::UserManagement *ui;

private slots:
    void on_bMatchList_clicked();
};

#endif // USERMANAGEMENT_H
