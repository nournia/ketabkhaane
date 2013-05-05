#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QSpacerItem>

#include <logindialog.h>
#include <startupdialog.h>
#include <aboutdialog.h>
#include <dialogchangepassword.h>
#include <formchangepermissions.h>
#include <scoreform.h>
#include <matchtable.h>
#include <formoperator.h>
#include <userform.h>
#include <matchform.h>
#include <objectform.h>
#include <optionsform.h>
#include <formfirst.h>
#include <usermanagement.h>
#include <objectmanagement.h>
#include <payment.h>
#include <webconnection.h>

// forms
FormFirst* formFirst;
FormOperator* formOperator;
FormChangePermissions* formChangePermissions;

UserForm* userForm;
MatchForm* matchForm;
ObjectForm* objectForm;
OptionsForm* optionsForm;
ScoreForm* scoreForm;
MatchTable* matchListForm;
UserManagement* userManagement;
ObjectManagement* objectManagement;
Payment* paymentForm;
WebConnection* webConnection;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Reghaabat::instance()->serverUrl = "http://reghaabat-nournia.rhcloud.com/server/";
    prepareUI();

    // check for db connection
    if (! Connector::connectDb().isOpen()) {
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), tr("Database Connection Error!"));
        exit(1);
    }

    // check for startup situation
    if (isStartup()) {
        StartupDialog dialog(this);
        dialog.exec();

        if (isStartup())
            exit(0);
    }

    applyPermission();
    firstPage();
    on_actionLogin_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isStartup()
{
    QSqlQuery qry;
    qry.exec("select id from library"); qry.next();
    Reghaabat::instance()->libraryId = qry.value(0).toString();
    if (Reghaabat::instance()->libraryId.isEmpty())
        return true;

    qry.exec("select id from users where upassword is not null");
    return !qry.next();
}

void MainWindow::prepareUI()
{
    // window
    resize(790, 550);
    this->move(QApplication::desktop()->screen()->rect().center()-this->rect().center());

    viewer = new ViewerForm(this);
    syncer = new Syncer(this);

    // ui elements
    stackedLayout = new QStackedLayout();
    stackedLayout->setContentsMargins(0, 0, 0, 0);
    delete ui->container->layout();
    ui->container->setLayout(stackedLayout);

    // status bar
    lUser = new QLabel("", this);

    bLogout = new QPushButton(QIcon(":/images/logout.png"), "", this);
    bLogout->setFlat(true); bLogout->setMaximumWidth(20); bLogout->setMaximumHeight(20);
    connect(bLogout, SIGNAL(clicked()), this, SLOT(on_actionLogout_triggered()));
    bLogout->setToolTip(tr("Logout"));

    bChangePassword = new QPushButton(QIcon(":/images/key.png"), "", this);
    bChangePassword->setFlat(true); bChangePassword->setMaximumWidth(20); bChangePassword->setMaximumHeight(20);
    connect(bChangePassword, SIGNAL(clicked()), this, SLOT(on_actionChangePassword_triggered()));
    bChangePassword->setToolTip(tr("Change Password"));

    bSync = new QPushButton(QIcon(":/images/sync.png"), "", this);
    bSync->setFlat(true); bSync->setMaximumWidth(20); bSync->setMaximumHeight(20);
    connect(bSync, SIGNAL(clicked()), this, SLOT(sync()));
    bSync->setToolTip(tr("Sync"));

    pSync = new QProgressBar(this);
    pSync->setMaximumWidth(70);
    pSync->setLayoutDirection(Qt::LeftToRight);
    pSync->setTextVisible(false);
    connect(syncer, SIGNAL(progress(int)), pSync, SLOT(setValue(int)));
    connect(syncer, SIGNAL(finished(QString)), this, SLOT(synced(QString)));
    pSync->setVisible(false);

    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout* lay =  new QHBoxLayout();
    lay->setContentsMargins(9, 3, 9, 3);
    lay->addWidget(bLogout);
    lay->addWidget(bChangePassword);
    lay->addWidget(lUser);
    lay->addItem(spacer);
    lay->addWidget(bSync);
    lay->addWidget(pSync);
    delete ui->statusBar->layout();
    ui->statusBar->setLayout(lay);
    ui->statusBar->show();
    ui->statusBar->setVisible(false);
}

void MainWindow::applyPermission()
{
   bool match = options()["Match"].toBool();
   ui->actionMatchManagement->setVisible(match);
   ui->actionSetScores->setVisible(match);
   ui->actionPayment->setVisible(match);
   ui->actionWeb->setVisible(match);

   // enables

   ui->actionWeb->setEnabled(false);
   ui->actionDeliver->setEnabled(false);
   ui->actionNewUser->setEnabled(false);
   ui->actionEditUser->setEnabled(false);
   ui->actionUserManagement->setEnabled(false);
   ui->actionMatchManagement->setEnabled(false);
   ui->actionObjectManagement->setEnabled(false);
   ui->actionOptions->setEnabled(false);
   ui->actionSetScores->setEnabled(false);
   ui->actionPayment->setEnabled(false);
   ui->actionChangePermissions->setEnabled(false);

   if (Reghaabat::hasAccess("operator"))
   {
       ui->actionNewUser->setEnabled(true);
       ui->actionEditUser->setEnabled(true);
       ui->actionDeliver->setEnabled(true);
       ui->actionUserManagement->setEnabled(true);
   }

   if (Reghaabat::hasAccess("designer"))
   {
       ui->actionMatchManagement->setEnabled(true);
       ui->actionObjectManagement->setEnabled(true);
   }

   if (Reghaabat::hasAccess("manager"))
   {
       ui->actionSetScores->setEnabled(true);
       ui->actionPayment->setEnabled(true);
   }

   if (Reghaabat::hasAccess("master"))
   {
       ui->actionChangePermissions->setEnabled(true);
       ui->actionOptions->setEnabled(true);
       ui->actionWeb->setEnabled(true);
   }

   if (Reghaabat::instance()->userPermission == "designer") {
       ui->actionOptions->setEnabled(true);
   }
}

void MainWindow::on_actionLogin_triggered()
{
    LoginDialog ld(this);
    ld.exec();

    if (! Reghaabat::instance()->userId.isEmpty()) {
        lUser->setText(Reghaabat::instance()->userName);
        ui->statusBar->setVisible(true);
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
    ui->statusBar->setVisible(false);

    // close open forms
    if (formOperator) {
        stackedLayout->removeWidget(formOperator);
        delete formOperator;
        formOperator = 0;
    }
    if (userForm) {
        stackedLayout->removeWidget(userForm);
        delete userForm;
        userForm = 0;
    }

    firstPage();

    applyPermission();
    on_actionLogin_triggered();
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
    if (! Reghaabat::hasAccess("designer")) return;

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

void MainWindow::newObject()
{
    if (! Reghaabat::hasAccess("designer")) return;

    if (! objectForm)
    {
        objectForm = new ObjectForm(this);
        connect(objectForm, SIGNAL(closeForm()), this, SLOT(firstPage()));
        stackedLayout->addWidget(objectForm);
    }
    stackedLayout->setCurrentWidget(objectForm);
    objectForm->editMode(false);
}

void MainWindow::editObject()
{
    if (! Reghaabat::hasAccess("designer")) return;

    newObject();
    objectForm->editMode(true);
}

void MainWindow::on_actionOptions_triggered()
{
    if (! (Reghaabat::hasAccess("master") || Reghaabat::instance()->userPermission == "designer")) return;

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
    formOperator->focus();
}

void MainWindow::firstPage()
{
    if (! formFirst)
    {
        formFirst = new FormFirst(this);
        stackedLayout->addWidget(formFirst);
    }
    stackedLayout->setCurrentWidget(formFirst);
    applyPermission();
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
    scoreForm->focus();
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

void MainWindow::on_actionNewUser_triggered()
{
    newUser();
}

void MainWindow::on_actionEditUser_triggered()
{
    editUser();
}

void MainWindow::on_actionObjectManagement_triggered()
{
    if (! Reghaabat::hasAccess("designer")) return;

    if (! objectManagement)
    {
        objectManagement = new ObjectManagement(this);
        stackedLayout->addWidget(objectManagement);
    }
    stackedLayout->setCurrentWidget(objectManagement);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog ad(this);
    ad.exec();
}

void MainWindow::on_actionWeb_triggered()
{
    if (! webConnection)
    {
        webConnection = new WebConnection(this);
        stackedLayout->addWidget(webConnection);
    }
    stackedLayout->setCurrentWidget(webConnection);
}

void MainWindow::sync()
{
    pSync->setValue(0);
    pSync->setVisible(true);
    bSync->setVisible(false);
    syncer->sync();
}

void MainWindow::synced(QString message)
{
    QMessageBox::warning(this, QApplication::tr("Reghaabat"), message);
    pSync->setVisible(false);
    bSync->setVisible(true);
}
