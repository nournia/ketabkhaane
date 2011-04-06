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

    void showList(QString title, QStringList fields, QString query);
    void loadHtml(QString name);

private:
    Ui::ViewerForm *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // VIEWERFORM_H
