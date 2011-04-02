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
    void showForm(QWidget* form);

private:
    Ui::MainWindow *ui;
    MyLineEdit *eUsername;

private slots:
    void on_actionOptions_triggered();
    void on_actionNewMatch_triggered();
    void selectUser();

    void on_actionNewUser_triggered();
    void on_actionLogout_triggered();
    void on_actionLogin_triggered();
    void on_actionSync_triggered();
};


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
};

#endif // MAINWINDOW_H
