#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QDesktopWidget>

#include <logindialog.h>
#include <dialogchangepassword.h>
#include <formchangepermissions.h>
#include <scoreform.h>
#include <matchtable.h>
#include <sender.h>
#include <formoperator.h>
#include <userform.h>
#include <matchform.h>
#include <optionsform.h>
#include <formfirst.h>
#include <usermanagement.h>
#include <payment.h>

// forms
FormFirst* formFirst;
FormOperator* formOperator;
FormChangePermissions* formChangePermissions;

UserForm* userForm;
MatchForm* matchForm;
OptionsForm* optionsForm;
ScoreForm* scoreForm;
MatchTable* matchListForm;
UserManagement* userManagement;
Payment* paymentForm;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->move(QApplication::desktop()->screen()->rect().center()-this->rect().center());

    applyPermission();

    stackedLayout = new QStackedLayout;
    stackedLayout->setContentsMargins(0, 0, 0, 0);
    delete ui->container->layout();
    ui->container->setLayout(stackedLayout);

//    optionsForm = new OptionsForm(this);
//    stackedLayout->addWidget(optionsForm);

    firstPage();
    on_actionLogin_triggered();

    viewer = new ViewerForm(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyPermission()
{
   ui->actionSync->setVisible(false);

   ui->actionDeliver->setEnabled(false);
   ui->actionUserManagement->setEnabled(false);
   ui->actionMatchManagement->setEnabled(false);
   ui->actionSync->setEnabled(false);
   ui->actionOptions->setEnabled(false);
   ui->actionChangePermissions->setEnabled(false);
   ui->actionSetScores->setEnabled(false);
   ui->actionPayment->setEnabled(false);

   ui->actionLogin->setVisible(Reghaabat::instance()->userId.isEmpty());
   ui->actionLogout->setVisible(! ui->actionLogin->isVisible());
   ui->actionChangePassword->setEnabled(! ui->actionLogin->isVisible());

   // enables

   if (Reghaabat::hasAccess("operator"))
   {
//       ui->actionSync->setEnabled(true);
       ui->actionUserManagement->setEnabled(true);
       ui->actionDeliver->setEnabled(true);
   }

   if (Reghaabat::hasAccess("designer"))
   {
       ui->actionMatchManagement->setEnabled(true);
   }

   if (Reghaabat::hasAccess("manager"))
   {
       ui->actionChangePermissions->setEnabled(true);
       ui->actionSetScores->setEnabled(true);
       ui->actionPayment->setEnabled(true);
   }

   if (Reghaabat::hasAccess("master"))
   {
       ui->actionOptions->setEnabled(true);
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

        on_actionDeliver_triggered();
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

    firstPage();

    applyPermission();
}

void MainWindow::on_actionUserManagement_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    if (! userManagement)
    {
        userManagement = new UserManagement(this);
        stackedLayout->addWidget(userManagement);
    }
    stackedLayout->setCurrentWidget(userManagement);
}

void MainWindow::on_actionMatchManagement_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    if (!matchListForm)
    {
        matchListForm = new MatchTable(this);
        stackedLayout->addWidget(matchListForm);
    }
    stackedLayout->setCurrentWidget(matchListForm);
}

void MainWindow::newUser()
{
    if (! Reghaabat::hasAccess("operator")) return;

    if (! userForm)
    {
        userForm = new UserForm(this);
        connect(userForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
        stackedLayout->addWidget(userForm);
    }
    stackedLayout->setCurrentWidget(userForm);
    userForm->editMode(false);
}

void MainWindow::editUser()
{
    if (! Reghaabat::hasAccess("operator")) return;

    newUser();
    userForm->editMode(true);
}

void MainWindow::newMatch()
{
    if (! Reghaabat::hasAccess("designer")) return;

    if (! matchForm)
    {
        matchForm = new MatchForm(this);
        connect(matchForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
        stackedLayout->addWidget(matchForm);
    }
    stackedLayout->setCurrentWidget(matchForm);
    matchForm->editMode(false);
}

void MainWindow::editMatch()
{
    if (! Reghaabat::hasAccess("designer")) return;

    newMatch();
    matchForm->editMode(true);
}

void MainWindow::on_actionOptions_triggered()
{
    if (! Reghaabat::hasAccess("master")) return;

    if (! optionsForm)
    {
        optionsForm = new OptionsForm(this);
        connect(optionsForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
        stackedLayout->addWidget(optionsForm);
    }
    stackedLayout->setCurrentWidget(optionsForm);
}

void MainWindow::on_actionDeliver_triggered()
{
    if (! Reghaabat::hasAccess("operator")) return;

    if (! formOperator)
    {
        formOperator = new FormOperator(this);
        stackedLayout->addWidget(formOperator);
    }
    stackedLayout->setCurrentWidget(formOperator);
}

void MainWindow::firstPage()
{
    if (! formFirst)
    {
        formFirst = new FormFirst(this);
        stackedLayout->addWidget(formFirst);
    }
    stackedLayout->setCurrentWidget(formFirst);
}

void MainWindow::on_actionChangePassword_triggered()
{
    DialogChangePassword dialog(this);
    dialog.exec();
}

void MainWindow::on_actionChangePermissions_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    if (! formChangePermissions)
    {
        formChangePermissions = new FormChangePermissions(this);
        stackedLayout->addWidget(formChangePermissions);
    }
    stackedLayout->setCurrentWidget(formChangePermissions);
}

void MainWindow::on_actionSetScores_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    if (! scoreForm)
    {
        scoreForm = new ScoreForm(this);
        stackedLayout->addWidget(scoreForm);
    }
    stackedLayout->setCurrentWidget(scoreForm);
}

void MainWindow::on_actionPayment_triggered()
{
    if (! Reghaabat::hasAccess("manager")) return;

    if (! paymentForm)
    {
        paymentForm = new Payment(this);
        stackedLayout->addWidget(paymentForm);
    }
    stackedLayout->setCurrentWidget(paymentForm);
}
