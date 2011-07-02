
#include <mylineedit.h>
#include <helper.h>

#include <QHeaderView>
#include <QEvent>
#include <QKeyEvent>

MyCompleter::MyCompleter(MyLineEdit *parent): QObject(parent), editor(parent)
{
    qry = new QSqlQuery;

    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(2);
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

void MyCompleter::showCompletion(const QStringList &choices, const QStringList &hits)
{
    if (choices.isEmpty() || choices.count() != hits.count())
        return;

    const QPalette &pal = editor->palette();
    QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

    popup->setLayoutDirection(Qt::RightToLeft);
    QFont font("Tahoma");
    popup->setFont(font);

    popup->setUpdatesEnabled(false);
    popup->clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem(popup);
        item->setText(0, choices[i]);
        item->setText(1, hits[i]);
        item->setTextAlignment(1, Qt::AlignRight);
        item->setTextColor(1, color);
    }
    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->resizeColumnToContents(1);
    popup->adjustSize();
    popup->setUpdatesEnabled(true);

    int h = popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
    popup->resize(editor->width(), h);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->show();
}

void MyCompleter::doneCompletion()
{
    popup->hide();
    QTreeWidgetItem *item = popup->currentItem();
    if (item)
        editor->setText(item->text(0));
}

void MyCompleter::updateSuggestions()
{
    QString text = editor->text();

    if (text != refineText(text))
        editor->setText(refineText(text));

    QStringList labels, names;
    if (! text.isEmpty())
    qry->exec(query + (query.contains("where") ? " and " : " where " ) + QString("(ctitle like '%"+ text +"%' or clabel like '%"+ text +"%') order by ctitle"));

    int i = 0;
    QString valueId = "";
    for ( ; qry->next(); i++)
    {
        if (i == 0 && (text == qry->value(2).toString() || text == qry->value(1).toString()))
            valueId = qry->value(0).toString();

        labels.append(qry->value(1).toString());
        names.append(qry->value(2).toString());
    }
    showCompletion(names, labels);

    if (i > 1) valueId = "";

    editor->setValue(valueId);
}

MyLineEdit::MyLineEdit(QString q, QWidget *parent): QLineEdit(parent)
{
    completer = new MyCompleter(this);
    completer->setQuery(q);

    connect(this, SIGNAL(returnPressed()), completer, SLOT(updateSuggestions()));

    adjustSize();
    resize(400, height());
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
    else
    {
        setStyleSheet("background-color:  hsv(120, 60, 255)");
        completer->doneCompletion();
        emit select();
    }
}
