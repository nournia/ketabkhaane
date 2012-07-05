#ifndef OBJECTMANAGEMENT_H
#define OBJECTMANAGEMENT_H

#include <QDialog>

namespace Ui {
    class ObjectManagement;
}

class ObjectManagement : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectManagement(QWidget *parent = 0);
    ~ObjectManagement();

private slots:
    void on_bPrint_clicked();

    void on_bPreview_clicked();

private:
    Ui::ObjectManagement *ui;
};

#endif // OBJECTMANAGEMENT_H
