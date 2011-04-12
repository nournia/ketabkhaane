#ifndef FormOperator_H
#define FormOperator_H

#include <QWidget>

#include <mylineedit.h>
#include <mmatches.h>

namespace Ui {
    class FormOperator;
}

class FormOperator : public QWidget
{
    Q_OBJECT

public:
    explicit FormOperator(QWidget *parent = 0);
    ~FormOperator();

    MyLineEdit *eUser, *eMatch;

    void previewMatch(bool print);

private:
    Ui::FormOperator *ui;

private slots:
    void on_bPreview_clicked();
    void selectUser();
    void selectMatch();
    void cancelUser();
    void cancelMatch();
    void on_bDeliver_clicked();
};

#endif // FormOperator_H
