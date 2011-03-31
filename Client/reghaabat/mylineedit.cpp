#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent), c(0), valueId(-1) {}

MyLineEdit::~MyLineEdit() {}

void MyLineEdit::setCompleter(MyCompleter *completer)
{
    if (c) QObject::disconnect(c, 0, this, 0);
    c = completer;
    if (!c) return;

    c->setWidget(this);
    connect(completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
    connect(this, SIGNAL(textChanged(QString)), SLOT(setIdValue()));
//    connect(this, SIGNAL(editingFinished()), this, SLOT(setIdValue()));
//    connect(this, SIGNAL(returnPressed()), this, SLOT(setIdValue()));
}

MyCompleter *MyLineEdit::completer() const
{
    return c;
}

void MyLineEdit::insertCompletion(const QString& completion)
{
    setText(completion);
    selectAll();
}

void MyLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // Let the completer do default behavior
        }
    }

    bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E;
    if (!isShortcut)
        QLineEdit::keyPressEvent(e); // Don't send the shortcut (CTRL-E) to the text edit.

    if (!c)
        return;

    bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!isShortcut && !ctrlOrShift && e->modifiers() != Qt::NoModifier)
    {
        c->popup()->hide();
        return;
    }

    c->update(text());
    c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
}

void MyLineEdit::setIdValue()
{
    QSqlQuery qry;
    qry.prepare(completer()->query + " where ctitle = ?");
    qry.addBindValue(this->text());
    qry.exec();

    valueId = "";
    if (qry.next())
    {
        valueId = qry.value(0).toString();
        if (qry.next())
            valueId = "";
    }

    if (text() == "")
        setStyleSheet("");
    else if (valueId == "")
        setStyleSheet("background-color:  hsv(0, 60, 255)");
    else
    {
        setStyleSheet("background-color:  hsv(120, 60, 255)");
        emit returnPressed();
    }
}
