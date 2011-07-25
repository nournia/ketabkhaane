
#include <mylineedit.h>
#include <helper.h>

#include <QHeaderView>
#include <QEvent>
#include <QKeyEvent>

MyCompleter::MyCompleter(MyLineEdit *parent): QObject(parent), editor(parent)
{
    qry = new QSqlQuery;
    const QPalette &pal = editor->palette();
    grayColor = pal.color(QPalette::Disabled, QPalette::WindowText);

    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);
    popup->setLayoutDirection(Qt::RightToLeft);
    QFont font("Tahoma");
    popup->setFont(font);

    popup->setColumnCount(3);
    popup->setColumnHidden(2, true);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(doneCompletion()));
    connect(editor, SIGNAL(textChanged(QString)), this, SLOT(updateSuggestions()));
}

MyCompleter::~MyCompleter()
{
    delete popup;
}

bool MyCompleter::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        popup->hide();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {

        bool consumed = false;
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;

        case Qt::Key_Escape:
            popup->hide();
            consumed = true;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->event(ev);
            //popup->hide();
            break;
        }

        return consumed;
    }

    return false;
}

void MyCompleter::doneCompletion()
{
    popup->hide();
    QTreeWidgetItem *item = popup->currentItem();
    if (item)
    {
        QString tmp = item->text(2);
        editor->setText(item->text(0));
        if (editor->value().isEmpty())
            editor->setValue(tmp);
    }
}

void MyCompleter::updateSuggestions()
{
    QString text = editor->text();
    QString valueId = "";

    if (text != refineText(text))
        editor->setText(refineText(text));

    QString fid, flabel, fname;
    if (! text.isEmpty())
    {
        int i;
        QString qtmp = query + (query.contains("where") ? " and " : " where ");

        popup->setUpdatesEnabled(false);
        popup->clear();

        qry->exec(qtmp + QString("(ctitle like '%"+ text +"%' or clabel like '%"+ text +"%') order by ctitle"));
        for (i = 0; qry->next(); i++)
        {
            if (i == 0)
            {
                fid = qry->value(0).toString();
                flabel = qry->value(1).toString();
                fname = qry->value(2).toString();
            }

            QTreeWidgetItem* item;
            item = new QTreeWidgetItem(popup);
            item->setText(0, qry->value(2).toString());
            item->setText(1, qry->value(1).toString());
            item->setText(2, qry->value(0).toString());
            item->setTextAlignment(1, Qt::AlignRight);
            item->setTextColor(1, grayColor);
        }

        if (i == 1 && (text == fname || text == flabel))
            valueId = fid;
        else if (i > 0)
        {
            // show popup
            popup->setCurrentItem(popup->topLevelItem(0));
            popup->resizeColumnToContents(0);
            popup->resizeColumnToContents(1);
            popup->adjustSize();
            popup->setUpdatesEnabled(true);
            int h = popup->sizeHintForRow(0) * qMin(7, i) + 3;
            popup->resize(editor->width(), h);
            popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
            popup->show();
        }
        else
            popup->hide();
    } else
        popup->hide();

    editor->setValue(valueId);
}

MyLineEdit::MyLineEdit(QWidget *parent): QLineEdit(parent)
{
    completer = new MyCompleter(this);
    connect(this, SIGNAL(returnPressed()), completer, SLOT(updateSuggestions()));
}

void MyLineEdit::setValue(QString val)
{
    if (! valueId.isEmpty() && val.isEmpty())
        emit cancel();

    valueId = val;

    if (text().isEmpty())
    {
        setStyleSheet("");
        return;
    }

    if (valueId.isEmpty())
        setStyleSheet("background-color:  hsv(0, 60, 255)");
    else {
        setStyleSheet("background-color:  hsv(120, 60, 255)");
        completer->popup->hide();
        emit select();
    }
}
