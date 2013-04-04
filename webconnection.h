#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

#include <QDialog>
#include <QStandardItemModel>
#include <QNetworkAccessManager>

namespace Ui {
class WebConnection;
}

class WebConnection : public QDialog
{
    Q_OBJECT

    bool preview;
    QStringList queue;
    QNetworkReply* reply;
    QNetworkAccessManager qnam;

    QStandardItemModel* items;

public:
    explicit WebConnection(QWidget *parent = 0);
    ~WebConnection();

    void popUrl();
    void queueUrl(QString args, bool file = false);
    void storeRows(QString table, QVariant rows);

private slots:
    void receive();
    void on_bImport_clicked();
    void on_bPreview_clicked();

private:
    Ui::WebConnection *ui;
};

#endif // WEBCONNECTION_H
