#ifndef USERMAIN_H
#define USERMAIN_H

#include <QWidget>

#include <mylineedit.h>

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

private:
    Ui::UserMain *ui;
};

#endif // USERMAIN_H
