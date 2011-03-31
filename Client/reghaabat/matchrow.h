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
    explicit MatchRow(QString uid, QString mid, QString title, QWidget *parent = 0);
    ~MatchRow();

    QString userId, matchId;

private:
    Ui::MatchRow *ui;

private slots:
    void on_button_clicked();
};

#endif // MATCHROW_H
