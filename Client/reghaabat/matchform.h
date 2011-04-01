#ifndef MATCHFORM_H
#define MATCHFORM_H

#include <QWidget>

#include <mmatches.h>
#include <mylineedit.h>
#include <questionmodule.h>

namespace Ui {
    class MatchForm;
}

class MatchForm : public QWidget
{
    Q_OBJECT

public:
    explicit MatchForm(QWidget *parent = 0);
    ~MatchForm();

    void select(QString id);

    MyLineEdit *eCorrector, *eAuthor, *ePublication;
    QString matchId; // in edit mode
    QList<QuestionModule*> qModules;

private:
    Ui::MatchForm *ui;

private slots:
    void on_bNewQuestion_clicked();
    void on_cType_currentIndexChanged(int index);
};

#endif // MATCHFORM_H
