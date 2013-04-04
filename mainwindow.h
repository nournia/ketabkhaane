#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

#include <viewerform.h>
#include <syncer.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void prepareUI();
    void applyPermission();

    ViewerForm* viewer;
    Syncer* syncer;

    QStackedLayout *stackedLayout;
    QLabel *lUser;
    QPushButton *bLogout, *bSync;
    QProgressBar *pSync;
private:
    Ui::MainWindow *ui;

public slots:
    void newUser();
    void editUser();
    void newMatch();
    void editMatch();
    void newObject();
    void editObject();

    void firstPage();
    void sync();
    void synced(QString message);

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

private slots:
    void on_actionAbout_triggered();
    void on_actionObjectManagement_triggered();
    void on_actionEditUser_triggered();
    void on_actionNewUser_triggered();
    void on_actionWeb_triggered();
};

#endif // MAINWINDOW_H
