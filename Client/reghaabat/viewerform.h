#ifndef VIEWERFORM_H
#define VIEWERFORM_H

#include <QWidget>

namespace Ui {
    class ViewerForm;
}

class ViewerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerForm(QWidget *parent = 0);
    ~ViewerForm();

    QString addTable(QString title, QStringList fields, QString query);
    void loadHtml(QString name);

private:
    Ui::ViewerForm *ui;

private slots:
    void on_bUserGenderGroup_clicked();
    void on_bMatchAll_clicked();
    void on_bUserAll_clicked();
    void on_bPrint_clicked();
};

#endif // VIEWERFORM_H
