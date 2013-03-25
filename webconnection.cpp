#include "webconnection.h"
#include "ui_webconnection.h"

#include <helper.h>

#include <QMessageBox>

WebConnection::WebConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebConnection)
{
    ui->setupUi(this);

    syncer = new Syncer(this);
    connect(syncer, SIGNAL(progress(int)), ui->pSync, SLOT(setValue(int)));
    connect(syncer, SIGNAL(finished(QString)), this, SLOT(synced(QString)));
}

WebConnection::~WebConnection()
{
    delete ui;
}

void WebConnection::on_bSync_clicked()
{
    ui->bSync->setEnabled(false);
    ui->pSync->setValue(0);
    syncer->sync();
}

void WebConnection::synced(QString message)
{
    QMessageBox::warning(this, QApplication::tr("Reghaabat"), message);
    ui->bSync->setEnabled(true);
    ui->pSync->setValue(0);
}
