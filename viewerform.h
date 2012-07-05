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
    void savePdf(QString filename);
    void showLabels(QString from, QString to);

    void bMatchAgeGroup();
    void bUserGenderGroup();
    void bMatchAll();
    void bUserAll();
private:
    Ui::ViewerForm *ui;

public slots:
    void on_bPrint_clicked();
    void on_bPdf_clicked();
};

#endif // VIEWERFORM_H
