#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

#include <syncer.h>
#include <receiver.h>

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT
    
    Syncer* syncer;
    Receiver* receiver;

public:
    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

    bool login;

private slots:
    void synced(QString message);
    void received(QVariantMap data);
    void on_bLogin_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_bRetry_clicked();

private:
    Ui::StartupDialog *ui;
};

#endif // STARTUPDIALOG_H
