#ifndef USERMAIN_H
#define USERMAIN_H

#include <QWidget>

#include <mylineedit.h>
#include <mmatches.h>

namespace Ui {
    class UserMain;
}

class UserMain : public QWidget
{
    Q_OBJECT

public:
    explicit UserMain(QWidget *parent = 0);
    ~UserMain();

    void select(QString userId);

    MyLineEdit* eMatchname;
    QString userId;

private:
    Ui::UserMain *ui;

private slots:
    void on_bDeliver_clicked();
    void selectMatch();
};

#endif // USERMAIN_H
