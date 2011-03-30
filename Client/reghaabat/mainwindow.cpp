#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <AccessToSqlite.h>
#include <sender.h>

#include <logindialog.h>
#include <userform.h>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

UserForm *userForm;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->setFont(QFont("Tahoma"));

    // add username edit
    eUsername = new MyLineEdit(ui->gUser);
    eUsername->setObjectName("eUsername");
    ui->horizontalLayout->addWidget(eUsername);
    eUsername->setFocus();

    MyCompleter * completer = new MyCompleter("select users.id, firstname || ' ' || lastname as ctitle from users", this);
    eUsername->setCompleter(completer);

    connect(eUsername, SIGNAL(returnPressed()), this, SLOT(selectUser()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clear()
{
    delete userForm; userForm = 0;
}

void MainWindow::selectUser()
{
    if (eUsername->value() != "")
    {
        if (userForm)
            userForm->edit(eUsername->value());
    }
}

void MainWindow::on_bConvert_clicked()
{
    convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb");
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
        userForm = new UserForm(this);
        ui->centralWidget->layout()->addWidget(userForm);
    }
}
