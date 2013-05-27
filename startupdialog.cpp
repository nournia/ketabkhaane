#include "startupdialog.h"
#include "ui_startupdialog.h"

#include <QMessageBox>
#include <QCryptographicHash>

#include <jalali.h>
#include <musers.h>

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);

    resize(380, 200);
    login = false;
    ui->gRegister->setVisible(!login);
    ui->gLogin->setVisible(login);
    ui->gError->setVisible(false);

    syncer = new Syncer(this);
    connect(syncer, SIGNAL(finished(QString)), this, SLOT(synced(QString)));
    syncer->sync();

    receiver = new Receiver(this);
    connect(receiver, SIGNAL(received(QVariantMap)), this, SLOT(received(QVariantMap)));
}

StartupDialog::~StartupDialog()
{
    delete ui;
}

void StartupDialog::on_bLogin_clicked()
{
    login = true;
    ui->gRegister->setVisible(!login);
    ui->gLogin->setVisible(login);
}

void StartupDialog::synced(QString message)
{
    if (message == Syncer::tr("Sync complete.")) {
        QSqlQuery qry;
        qry.exec("select id from library"); qry.next();
        App::instance()->libraryId = qry.value(0).toString();
        ui->gError->setVisible(false);
    } else {
        ui->lWebConnection->setText(message);
        ui->gError->setVisible(true);
    }
}

void StartupDialog::received(QVariantMap data)
{
    QString msg;
    QSqlQuery qry;

    if (data["operation"] == "login" && data["user"].toList().length() > 0) {
        QString userId = data["user"].toList()[0].toStringList()[0];
        receiver->storeRows("users", data["user"]);
        msg = MUsers::setPermission(userId, "master");
    }
    else
        msg = tr("Invalid id or password.");

    ui->buttonBox->setEnabled(true);
    if (msg.isEmpty())
        this->close();
    else
        QMessageBox::warning(this, QObject::tr("Ketabkhaane"), msg);
}

void StartupDialog::on_buttonBox_accepted()
{
    QString msg;

    if (App::instance()->libraryId.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("Ketabkhaane"), tr("Please wait until server connection establishment."));
        return;
    }

    QSqlDatabase db = Connector::connectDb();
    db.transaction();
    insertLog("library", "insert", App::instance()->libraryId);

    if (!login) {
        // register user
        if (ui->eFirstname->text().isEmpty() || ui->eLastname->text().isEmpty() || ui->eRegisterPassword->text().isEmpty())
            msg = tr("Please complete required info.");
        else {
            StrMap user;
            user["firstname"] = ui->eFirstname->text();
            user["lastname"] = ui->eLastname->text();
            user["gender"] = "male";
            user["birth_date"] = "";
            user["account_id"] = "0";
            msg = MUsers::set("", user);

            // add permission
            if (msg.isEmpty()) {
                msg = MUsers::setPermission(user["id"].toString(), "master");
                if (msg.isEmpty())
                    msg = MUsers::setPassword(user["id"].toString(), ui->eRegisterPassword->text());
            }
        }
    } else {
        // import user
        if (ui->eNationalId->text().isEmpty() || ui->eLoginPassword->text().isEmpty())
            msg = tr("Please complete required info.");
        else {
            QString password = QCryptographicHash::hash(ui->eLoginPassword->text().toUtf8(), QCryptographicHash::Sha1).toHex();
            receiver->get(QString("user_login/%1/%2").arg(ui->eNationalId->text(), password));
            ui->buttonBox->setEnabled(false);
            return;
        }
    }

    if (msg.isEmpty()) {
        db.commit();
        // todo: login
        this->close();
    } else {
        db.rollback();
        QMessageBox::warning(this, QObject::tr("Ketabkhaane"), msg);
    }
}

void StartupDialog::on_buttonBox_rejected()
{
    this->close();
}

void StartupDialog::on_bRetry_clicked()
{
    syncer->sync();
}
