#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <AccessToSqlite.h>
#include <sender.h>

#include <logindialog.h>

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
    convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");
}

void MainWindow::on_actionSync_triggered()
{
//    Sender sender;
//    sender.sync();
}

void MainWindow::on_actionLogin_triggered()
{
    LoginDialog ld(this);
    ld.exec();
}
