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

    void select(QString userId);

    MyLineEdit* eMatchname;
    QString userId;

private:
    Ui::FormOperator *ui;

private slots:
    void on_bDeliver_clicked();
    void selectMatch();
};

#endif // FormOperator_H
