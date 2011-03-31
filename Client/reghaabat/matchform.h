#ifndef MATCHFORM_H
#define MATCHFORM_H

#include <QWidget>

namespace Ui {
    class MatchForm;
}

class MatchForm : public QWidget
{
    Q_OBJECT

public:
    explicit MatchForm(QWidget *parent = 0);
    ~MatchForm();

private:
    Ui::MatchForm *ui;
};

#endif // MATCHFORM_H
