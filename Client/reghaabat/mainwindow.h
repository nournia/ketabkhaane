#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <mylineedit.h>

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
    void on_actionDeliverMatch_triggered();
    void on_actionOptions_triggered();
    void on_actionNewMatch_triggered();

    void on_actionNewUser_triggered();
    void on_actionLogout_triggered();
    void on_actionLogin_triggered();
    void on_actionSync_triggered();
};

#include <QDebug>

// rghaabat global variables
// from: http://stackoverflow.com/questions/3747085/global-variables-in-qt
class Reghaabat
{
private:
    static Reghaabat* m_Instance;

public:
    QString userId, userName, userPermission;

    static Reghaabat* instance()
    {
        if (! m_Instance)
            m_Instance = new Reghaabat;
        return m_Instance;
    }

    static bool hasAccess(QString limit)
    {
        QString permission = Reghaabat::instance()->userPermission;
        QStringList permissions = QStringList() << "user" << "operator" << "designer" << "manager" << "master" << "admin";

        if (permission == "designer" && limit == "operator")
            return false;

        return permissions.indexOf(permission) >= permissions.indexOf(limit);
    }
};

#endif // MAINWINDOW_H
