#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sender.h>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

#include <logindialog.h>
#include <usermain.h>
#include <userform.h>
#include <matchform.h>
#include <optionsform.h>
#include <formfirst.h>

FormFirst* formFirst;
UserMain* userMain;
UserForm* userForm;
MatchForm* matchForm;
OptionsForm* optionsForm;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->setFont(QFont("Tahoma"));

    // add username edit
    eUsername = new MyLineEdit("select id, firstname || ' ' || lastname as ctitle from users", ui->gUser);
    eUsername->setObjectName("eUsername");
    ui->horizontalLayout->addWidget(eUsername);
    eUsername->setFocus();

    connect(eUsername, SIGNAL(returnPressed()), this, SLOT(selectUser()));

    formFirst = new FormFirst(this);
    showForm(formFirst);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clear()
{
    delete formFirst; formFirst = 0;
    delete userForm; userForm = 0;
    delete userMain; userMain = 0;
    delete matchForm; matchForm = 0;
    delete optionsForm; optionsForm = 0;
}

void MainWindow::showForm(QWidget* form)
{
    QString formClass = form->metaObject()->className();
    ui->gUser->setVisible(formClass == "UserForm" || formClass == "UserMain");

    QVBoxLayout* lay =  new QVBoxLayout();
    lay->setContentsMargins(0, 0, 0, 0);

    lay->addWidget(form);
    delete ui->container->layout();
    ui->container->setLayout(lay);
    ui->container->show();
}

void MainWindow::selectUser()
{
    QString userId = eUsername->value();
    if (userId != "")
    {
        if (! userMain)
        {
            clear();
            userMain = new UserMain(this);
            showForm(userMain);
        }
        userMain->select(userId);
    }
}

void MainWindow::on_actionSync_triggered()
{
    static Sender sender(this);
    sender.sync();
}

void MainWindow::on_actionLogin_triggered()
{
    LoginDialog ld(this);
    ld.exec();

    ui->statusBar->showMessage(Reghaabat::instance()->userName);
}

void MainWindow::on_actionLogout_triggered()
{
    Reghaabat::instance()->userId = "";
    Reghaabat::instance()->userName = "";
    Reghaabat::instance()->userPermission = "";

    ui->statusBar->showMessage(Reghaabat::instance()->userName);

    clear();
}

void MainWindow::on_actionNewUser_triggered()
{
    if (! userForm)
    {
        clear();
        userForm = new UserForm(this);
        showForm(userForm);
    }
}

void MainWindow::on_actionNewMatch_triggered()
{
    if (! matchForm)
    {
        clear();
        matchForm = new MatchForm(this);
        showForm(matchForm);
    }
}

void MainWindow::on_actionOptions_triggered()
{
    if (! optionsForm)
    {
        clear();
        optionsForm = new OptionsForm(this);
        showForm(optionsForm);
    }

}
