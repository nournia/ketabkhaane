#ifndef MyLineEdit_H
#define MyLineEdit_H

#include <QLineEdit>
#include <QTreeWidget>
#include <QSqlQuery>

class MyLineEdit;

class MyCompleter : public QObject
{
    Q_OBJECT

public:
    MyCompleter(MyLineEdit *parent = 0);
    ~MyCompleter();
    bool eventFilter(QObject *obj, QEvent *ev);
    QString getText(QString id);
    void setQuery(QString _table, QString query);

    QTreeWidget *popup;

public slots:
    void doneCompletion();
    void updateSuggestions();

private:
    MyLineEdit *editor;
    QSqlQuery *qry;
    QString table;
    QColor grayColor;
};


class MyLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit(QWidget *parent = 0);

    void setValue(QString val);
    void selectValue(QString val);

    QString value() {
        return valueId;
    }

    void setQuery(QString query) {
        completer->setQuery(this->parent()->objectName() + this->objectName(), query);
    }

signals:
    void select();
    void cancel();

private:
    QString valueId;
    MyCompleter *completer;
};


#endif // MyLineEdit_H
