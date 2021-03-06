#ifndef MATCHROW_H
#define MATCHROW_H

#include <QWidget>

namespace Ui {
    class MatchRow;
}

class MatchRow : public QWidget
{
    Q_OBJECT

public:
    explicit MatchRow(QString title, QString label, QStringList states, QString mid, QString oid, int fi, QDate delivered, QDate renewed, QWidget *parent = 0);
    ~MatchRow();
    QString getState();

    QString matchId, objectId;
    int fine;

signals:
    void changed();

private:
    Ui::MatchRow *ui;
};

#endif // MATCHROW_H
