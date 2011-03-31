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
    explicit MatchRow(QString id, QString title, QWidget *parent = 0);
    ~MatchRow();

    QString matchId;

private:
    Ui::MatchRow *ui;
};

#endif // MATCHROW_H
