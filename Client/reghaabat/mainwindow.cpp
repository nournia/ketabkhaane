#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <AccessToSqlite.h>
#include <sender.h>

#include <logindialog.h>

// init reghaabat global variables
Reghaabat* Reghaabat::m_Instance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    Reghaabat::instance()->userName = "new one";

    LoginDialog ld(this);
    ld.exec();
}
