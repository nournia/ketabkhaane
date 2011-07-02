#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <formoperator.h>
#include <userform.h>
#include <matchform.h>
#include <optionsform.h>
#include <formfirst.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void clear();
    void applyPermission();
    void showForm(QWidget* form);

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionMatchTable_triggered();
    void on_actionSetScores_triggered();
    void on_actionLists_triggered();
    void on_actionChangePermissions_triggered();
    void on_actionChangePassword_triggered();
    void on_actionEditMatch_triggered();
    void on_actionEditUser_triggered();
    void on_actionDeliverMatch_triggered();
    void on_actionOptions_triggered();
    void on_actionNewMatch_triggered();
    void on_actionNewUser_triggered();
    void on_actionLogout_triggered();
    void on_actionLogin_triggered();
    void on_actionSync_triggered();

    void firstPage();
};

#endif // MAINWINDOW_H
