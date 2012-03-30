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
    explicit MatchRow(QString title, QStringList states, QString mid, QString oid, int fine, QWidget *parent = 0);
    ~MatchRow();
    QString getState();

    QString matchId, objectId;

private:
    Ui::MatchRow *ui;
};

#endif // MATCHROW_H
