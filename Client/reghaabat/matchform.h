#ifndef MATCHFORM_H
#define MATCHFORM_H

#include <QWidget>
#include <QAbstractButton>

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

    void editMode(bool edit);

    MyLineEdit *eMatch, *eCorrector, *eAuthor, *ePublication;
    QList<QuestionModule*> qModules;

    void fillMaps(StrMap& match, QList<StrPair>& questions);
    void clearQuestions();

private:
    Ui::MatchForm *ui;

signals:
    void closeForm();

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
    void selectMatch();
    void cancelMatch();

    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_bNewQuestion_clicked();
    void on_cType_currentIndexChanged(int index);
};

#endif // MATCHFORM_H
