#include "titleedit.h"

TitleEdit::TitleEdit(QLineEdit* edt, QObject *parent) :
    QObject(parent), edit(edt)
{
    edit->setText("mine");
}
