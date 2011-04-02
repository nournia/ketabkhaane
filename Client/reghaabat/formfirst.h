#ifndef FORMFIRST_H
#define FORMFIRST_H

#include <QWidget>

namespace Ui {
    class FormFirst;
}

class FormFirst : public QWidget
{
    Q_OBJECT

public:
    explicit FormFirst(QWidget *parent = 0);
    ~FormFirst();

private:
    Ui::FormFirst *ui;
};

#endif // FORMFIRST_H
