#include "webconnection.h"
#include "ui_webconnection.h"

#include <sender.h>

WebConnection::WebConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebConnection)
{
    ui->setupUi(this);
}

WebConnection::~WebConnection()
{
    delete ui;
}

void WebConnection::on_bSync_clicked()
{
    static Sender sender(this);
    sender.sync();
}
