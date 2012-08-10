#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

#include <QDialog>

namespace Ui {
class WebConnection;
}

class WebConnection : public QDialog
{
    Q_OBJECT
    
public:
    explicit WebConnection(QWidget *parent = 0);
    ~WebConnection();
    
private slots:
    void on_bSync_clicked();

private:
    Ui::WebConnection *ui;
};

#endif // WEBCONNECTION_H
