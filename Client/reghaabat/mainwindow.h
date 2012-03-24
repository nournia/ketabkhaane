#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>

#include <viewerform.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void applyPermission();

    ViewerForm* viewer;
    QStackedLayout *stackedLayout;
    QString tmpId;
private:
    Ui::MainWindow *ui;

public slots:
    void newUser();
    void editUser();
    void newMatch();
    void editMatch();
    void newObject();
    void editObject();

    void on_actionPayment_triggered();
    void on_actionSetScores_triggered();
    void on_actionChangePermissions_triggered();
    void on_actionChangePassword_triggered();
    void on_actionDeliver_triggered();
    void on_actionOptions_triggered();
    void on_actionMatchManagement_triggered();
    void on_actionUserManagement_triggered();
    void on_actionLogout_triggered();
    void on_actionLogin_triggered();
    void on_actionSync_triggered();

    void firstPage();

private slots:
    void on_actionObjectManagement_triggered();
    void on_actionEditUser_triggered();
    void on_actionNewUser_triggered();
};

#endif // MAINWINDOW_H
