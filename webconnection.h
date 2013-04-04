#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

#include <QDialog>
#include <QStandardItemModel>

#include <syncer.h>

namespace Ui {
class WebConnection;
}

class WebConnection : public QDialog
{
    Q_OBJECT

    bool preview;
    QNetworkReply* reply;
    QNetworkAccessManager qnam;

    QStandardItemModel* items;

public:
    explicit WebConnection(QWidget *parent = 0);
    ~WebConnection();
    Syncer* syncer;

    void get(QString args, bool file = false);
    void storeRows(QString table, QVariant rows);

private slots:
    void receive();
    void synced(QString message);
    void on_bSync_clicked();

    void on_bImport_clicked();

    void on_bPreview_clicked();

private:
    Ui::WebConnection *ui;
};

#endif // WEBCONNECTION_H
