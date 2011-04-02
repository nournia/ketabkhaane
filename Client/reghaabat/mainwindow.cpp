#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sender.h>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

#include <logindialog.h>
#include <formoperator.h>
#include <userform.h>
#include <matchform.h>
#include <optionsform.h>
#include <formfirst.h>

FormFirst* formFirst;
FormOperator* formOperator;
UserForm* userForm;
MatchForm* matchForm;
OptionsForm* optionsForm;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->setFont(QFont("Tahoma"));

    applyPermission();

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
    delete formOperator; formOperator = 0;
    delete matchForm; matchForm = 0;
    delete optionsForm; optionsForm = 0;
}

void MainWindow::applyPermission()
{
   ui->actionSync->setEnabled(false);
   ui->actionOptions->setEnabled(false);
   ui->actionDeliverMatch->setEnabled(false);
   ui->actionNewUser->setEnabled(false);
   ui->actionNewMatch->setEnabled(false);

   if (Reghaabat::instance()->userId.isEmpty())
   {
       ui->actionLogin->setVisible(true);
       ui->actionLogout->setVisible(false);
   } else
   {
       ui->actionLogin->setVisible(false);
       ui->actionLogout->setVisible(true);
   }

   // enables
   ui->actionSync->setEnabled(true);

   if (Reghaabat::hasAccess("operator"))
   {
       ui->actionNewUser->setEnabled(true);
       ui->actionDeliverMatch->setEnabled(true);
   }

   if (Reghaabat::hasAccess("designer"))
   {
       ui->actionNewMatch->setEnabled(true);
   }

   if (Reghaabat::hasAccess("master"))
   {
       ui->actionOptions->setEnabled(true);
   }
}

void MainWindow::showForm(QWidget* form)
{
    QVBoxLayout* lay =  new QVBoxLayout();
    lay->setContentsMargins(0, 0, 0, 0);

    lay->addWidget(form);
    delete ui->container->layout();
    ui->container->setLayout(lay);
    ui->container->show();
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

    if (! Reghaabat::instance()->userId.isEmpty())
    {
        ui->statusBar->showMessage(Reghaabat::instance()->userName);
        applyPermission();

        ui->actionDeliverMatch->trigger();
    }
}

void MainWindow::on_actionLogout_triggered()
{
    Reghaabat::instance()->userId = "";
    Reghaabat::instance()->userName = "";
    Reghaabat::instance()->userPermission = "";

    ui->statusBar->showMessage(Reghaabat::instance()->userName);

    clear();

    applyPermission();
}

void MainWindow::on_actionNewUser_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    if (! userForm)
    {
        clear();
        userForm = new UserForm(this);
        showForm(userForm);
    }
}

void MainWindow::on_actionNewMatch_triggered()
{
    if (! Reghaabat::hasAccess("designer")) return;

    if (! matchForm)
    {
        clear();
        matchForm = new MatchForm(this);
        showForm(matchForm);
    }
}

void MainWindow::on_actionOptions_triggered()
{
    if (! Reghaabat::hasAccess("master")) return;

    if (! optionsForm)
    {
        clear();
        optionsForm = new OptionsForm(this);
        showForm(optionsForm);
    }
}

void MainWindow::on_actionDeliverMatch_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    if (! formOperator)
    {
        clear();
        formOperator = new FormOperator(this);
        showForm(formOperator);
    }

    formOperator->eUser->setFocus();
}
