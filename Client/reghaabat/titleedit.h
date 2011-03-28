#ifndef TITLEEDIT_H
#define TITLEEDIT_H

#include <QObject>
#include <QLineEdit>

class TitleEdit : public QObject
{
    Q_OBJECT

    QLineEdit* edit;

public:
    explicit TitleEdit(QLineEdit* edt, QObject *parent = 0);

signals:

public slots:

};

#endif // TITLEEDIT_H
