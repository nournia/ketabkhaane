#ifndef VIEWERFORM_H
#define VIEWERFORM_H

#include <QDialog>
#include <helper.h>

namespace Ui {
    class ViewerForm;
}

class ViewerForm : public QDialog
{
    Q_OBJECT

public:
    explicit ViewerForm(QWidget *parent = 0);
    ~ViewerForm();

    QString addTable(QString title, QStringList fields, QString query);
    void loadHtml(QString name);
    void showMatch(StrMap match, QList<StrPair> questions);

private:
    Ui::ViewerForm *ui;

private slots:
    void on_bMatchAgeGroup_clicked();
    void on_bUserGenderGroup_clicked();
    void on_bMatchAll_clicked();
    void on_bUserAll_clicked();
    void on_bPrint_clicked();
};

#endif // VIEWERFORM_H
