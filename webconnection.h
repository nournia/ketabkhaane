#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

#include <QDialog>
#include <QStandardItemModel>
#include <QNetworkAccessManager>

#include <receiver.h>

namespace Ui {
class WebConnection;
}

class WebConnection : public QDialog
{
    Q_OBJECT

    bool preview;
    Receiver* receiver;
    QStandardItemModel* items;

public:
    explicit WebConnection(QWidget *parent = 0);
    ~WebConnection();

private slots:
    void received(QVariantMap data);
    void on_bImport_clicked();
    void on_bPreview_clicked();

private:
    Ui::WebConnection *ui;
};

#endif // WEBCONNECTION_H
