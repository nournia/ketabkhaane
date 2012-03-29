#ifndef FormOperator_H
#define FormOperator_H

#include <QWidget>

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
    void on_bReceive_clicked();
    void on_cDeliver_currentIndexChanged(int);
    void on_cLimitedSearch_clicked();
    void on_bPreview_clicked();
    void selectUser();
    void selectObject();
    void cancelUser();
    void cancelObject();
    void on_bDeliver_clicked();
};

#endif // FormOperator_H
