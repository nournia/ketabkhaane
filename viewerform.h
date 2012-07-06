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
    void loadHtml(QString name, bool _margin = true, bool _landscape = false);
    void showMatch(StrMap match, QList<StrPair> questions);
    void savePdf(QString filename);
    void showObjectLabels(QString from, QString to, bool ageclass);
    void showObjectList(QString from, QString to);

    void bMatchAgeGroup();
    void bUserGenderGroup();
    void bMatchAll();
    void bUserAll();

    bool landscape, margin;
private:
    Ui::ViewerForm *ui;

public slots:
    void on_bPrint_clicked();
    void on_bPdf_clicked();
};

#endif // VIEWERFORM_H
