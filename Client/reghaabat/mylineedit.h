/*
 based on:
 http://www.qtcentre.org/threads/23518-How-to-change-completion-rule-of-QCompleter
 */

#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QStringList>
#include <QStringListModel>
#include <QString>
#include <QCompleter>
#include <QKeyEvent>
#include <QSqlQuery>

#include <helper.h>

#include <QDebug>
class MyCompleter : public QCompleter
{
    Q_OBJECT

public:
    QString query;

    MyCompleter (QString q, QObject* parent)
        : QCompleter(parent), query(q), m_model()
    {
        setModel(&m_model);

        // configurations
        QFont font("Tahoma");
        popup()->setFont(font);
        popup()->setLayoutDirection(Qt::RightToLeft);

        setQuery(q);
    }

    void setQuery(QString q)
    {
        query = q;
        m_list.clear();

        QSqlQuery qry;
        qry.exec(query + " order by ctitle");
        while (qry.next())
            m_list << qry.value(1).toString();
    }

    inline void update(QString word)
    {
        // Do any filtering you like.
        // Here we just include all items that contain word.
        QStringList filtered = m_list.filter(word, caseSensitivity());
        m_model.setStringList(filtered);
        m_word = word;

        // for only one exact match choice
        if (!word.isEmpty() && !(filtered.size() == 1 && filtered.first() == m_word))
            complete();
    }

    inline QString word()
    {
        return m_word;
    }

private:
    QStringList m_list;
    QStringListModel m_model;
    QString m_word;
};

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit(QString query, QWidget *parent = 0);
    ~MyLineEdit();

    void setCompleter(MyCompleter *c);
    MyCompleter *completer() const;

    QString value() { return valueId; }

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void insertCompletion(const QString &completion);
    void setIdValue();

signals:
    void select();
    void cancel();

private:
    MyCompleter *c;
    QString valueId; // from completer query
    QSqlQuery* qry;
};

#endif // MYLINEEDIT_H
