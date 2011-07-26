#ifndef FormOperator_H
#define FormOperator_H

#include <QWidget>

#include <mmatches.h>
#include <viewerform.h>

namespace Ui {
    class FormOperator;
}

class FormOperator : public QWidget
{
    Q_OBJECT

public:
    explicit FormOperator(QWidget *parent = 0);
    ~FormOperator();

    void prepareViewer();
    void focus();

private:
    Ui::FormOperator *ui;
    ViewerForm* viewer;

private slots:
    void on_cQuickSearch_clicked();
    void on_bPreview_clicked();
    void selectUser();
    void selectMatch();
    void cancelUser();
    void cancelMatch();
    void on_bDeliver_clicked();
};

#endif // FormOperator_H
