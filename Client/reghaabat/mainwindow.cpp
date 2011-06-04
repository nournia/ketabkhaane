#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>

#include <logindialog.h>
#include <dialogchangepassword.h>
#include <formchangepermissions.h>
#include <viewerform.h>
#include <scoreform.h>
#include <sender.h>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

// forms
FormFirst* formFirst;
FormOperator* formOperator;
FormChangePermissions* formChangePermissions;

UserForm* userForm;
MatchForm* matchForm;
OptionsForm* optionsForm;
ViewerForm* viewerForm;
ScoreForm* scoreForm;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // files
    Reghaabat::instance()->files = QDir::currentPath() + "/data/files/";

    applyPermission();

//    optionsForm = new OptionsForm(this);
//    showForm(optionsForm);

    firstPage();
    on_actionLogin_triggered();
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
    delete formChangePermissions; formChangePermissions = 0;
    delete viewerForm; viewerForm = 0;
    delete scoreForm; scoreForm = 0;
}

void MainWindow::applyPermission()
{
   ui->actionSync->setEnabled(false);
   ui->actionOptions->setEnabled(false);
   ui->actionDeliverMatch->setEnabled(false);
   ui->actionNewUser->setEnabled(false);
   ui->actionEditUser->setEnabled(false);
   ui->actionNewMatch->setEnabled(false);
   ui->actionEditMatch->setEnabled(false);
   ui->actionChangePermissions->setEnabled(false);
   ui->actionLists->setEnabled(false);
   ui->actionSetScores->setEnabled(false);

   ui->actionLogin->setVisible(Reghaabat::instance()->userId.isEmpty());
   ui->actionLogout->setVisible(! ui->actionLogin->isVisible());
   ui->actionChangePassword->setEnabled(! ui->actionLogin->isVisible());

   // enables

   if (Reghaabat::hasAccess("operator"))
   {
//       ui->actionSync->setEnabled(true);
       ui->actionNewUser->setEnabled(true);
       ui->actionEditUser->setEnabled(true);
       ui->actionDeliverMatch->setEnabled(true);
   }

   if (Reghaabat::hasAccess("designer"))
   {
       ui->actionNewMatch->setEnabled(true);
       ui->actionEditMatch->setEnabled(true);
   }

   if (Reghaabat::hasAccess("manager"))
   {
       ui->actionChangePermissions->setEnabled(true);
       ui->actionLists->setEnabled(true);
       ui->actionSetScores->setEnabled(true);
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
        QLabel* label = new QLabel(Reghaabat::instance()->userName, this);

        QVBoxLayout* lay =  new QVBoxLayout();
        lay->setContentsMargins(9, 3, 9, 3);
        lay->addWidget(label);
        delete ui->statusBar->layout();
        ui->statusBar->setLayout(lay);
        ui->statusBar->show();

        applyPermission();

        on_actionDeliverMatch_triggered();
    }
}

void MainWindow::on_actionLogout_triggered()
{
    Reghaabat::instance()->userId = "";
    Reghaabat::instance()->userName = "";
    Reghaabat::instance()->userPermission = "";

    // clean status bar
    QLayoutItem *child;
    while ((child = ui->statusBar->layout()->takeAt(0)) != 0)
         delete child->widget();
    delete ui->statusBar->layout();

    clear();

    applyPermission();
}

void MainWindow::on_actionNewUser_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    clear();
    userForm = new UserForm(this);
    connect(userForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
    showForm(userForm);
}

void MainWindow::on_actionEditUser_triggered()
{
    on_actionNewUser_triggered();
    if (userForm)
    {
        userForm->editMode(true);
        userForm->eUser->setFocus();
    }
}

void MainWindow::on_actionNewMatch_triggered()
{
    if (! Reghaabat::hasAccess("designer")) return;

    clear();
    matchForm = new MatchForm(this);
    connect(matchForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
    showForm(matchForm);
}

void MainWindow::on_actionEditMatch_triggered()
{
    on_actionNewMatch_triggered();
    if (matchForm)
    {
       matchForm->editMode(true);
       matchForm->eMatch->setFocus();
    }
}

void MainWindow::on_actionOptions_triggered()
{
    if (! Reghaabat::hasAccess("master")) return;

    clear();
    optionsForm = new OptionsForm(this);
    connect(optionsForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
    showForm(optionsForm);
}

void MainWindow::on_actionDeliverMatch_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    clear();
    formOperator = new FormOperator(this);
    showForm(formOperator);

    formOperator->eUser->setFocus();
}

void MainWindow::firstPage()
{
    clear();
    formFirst = new FormFirst(this);
    showForm(formFirst);
}

void MainWindow::on_actionChangePassword_triggered()
{
    DialogChangePassword dialog(this);
    dialog.exec();
}

void MainWindow::on_actionChangePermissions_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    clear();
    formChangePermissions = new FormChangePermissions(this);
    showForm(formChangePermissions);
}

void MainWindow::on_actionLists_triggered()
{
    clear();
    viewerForm = new ViewerForm(this);
    showForm(viewerForm);
}

void MainWindow::on_actionSetScores_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    clear();
    scoreForm = new ScoreForm(this);
    showForm(scoreForm);
}
