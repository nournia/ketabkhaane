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

class MyCompleter : public QCompleter
{
    Q_OBJECT

public:
    inline MyCompleter(const QStringList& words, QObject* parent)
        : QCompleter(parent), m_list(words), m_model()
    {
        setModel(&m_model);
    }

    MyCompleter (QString query, QObject* parent)
        : QCompleter(parent)
    {
        QSqlQuery qry;
        qry.exec(query);
        while (qry.next())
            m_list << qry.value(0).toString();

        setModel(&m_model);

        // configurations

    }

    inline void update(QString word)
    {
        // Do any filtering you like.
        // Here we just include all items that contain word.
        QStringList filtered = m_list.filter(word, caseSensitivity());
        m_model.setStringList(filtered);
        m_word = word;
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
    MyLineEdit(QWidget *parent = 0);
    ~MyLineEdit();

    void setCompleter(MyCompleter *c);
    MyCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void insertCompletion(const QString &completion);

private:
    MyCompleter *c;
};

#endif // MYLINEEDIT_H
