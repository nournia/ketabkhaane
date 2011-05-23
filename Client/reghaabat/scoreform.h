#ifndef SCOREFORM_H
#define SCOREFORM_H

#include <mylineedit.h>

#include <QWidget>

namespace Ui {
    class ScoreForm;
}

class ScoreForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreForm(QWidget *parent = 0);
    ~ScoreForm();

    MyLineEdit *eCorrector;

private:
    Ui::ScoreForm *ui;

private slots:
    void selectCorrector();
    void cancelCorrector();
};

#endif // SCOREFORM_H
