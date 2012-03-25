#ifndef MATCHFORM_H
#define MATCHFORM_H

#include <QWidget>
#include <QAbstractButton>

#include <mmatches.h>
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

    QList<QuestionModule*> qModules;

    void fillMaps(StrMap& match, QList<StrPair>& questions);
    void clearQuestions();

    QWidget* fillerItem;

private:
    Ui::MatchForm *ui;

signals:
    void closeForm();

public slots:
    QString getFilename();

private slots:
    void selectMatch();
    void cancelMatch();
    void selectObject();
    void cancelObject();
    void bNewQuestion_clicked();

    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_cType_currentIndexChanged(int index);
};

#endif // MATCHFORM_H
