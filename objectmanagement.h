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
    void on_bPreviewList_clicked();

    void on_bPreviewLabels_clicked();

    void on_bPrintLabels_clicked();

    void on_bPrintList_clicked();

private:
    Ui::ObjectManagement *ui;
};

#endif // OBJECTMANAGEMENT_H
