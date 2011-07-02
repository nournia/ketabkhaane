#include "matchform.h"
#include "ui_matchform.h"

#include <QMessageBox>
#include <QFormLayout>
#include <QWebView>
#include <QWebFrame>
#include <QWebElementCollection>
#include <QPushButton>
#include <QFile>

#include <viewerform.h>
#include <mainwindow.h>

MatchForm::MatchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MatchForm)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(ViewerForm::tr("Preview"), QDialogButtonBox::ResetRole);

    eCorrector = new MyLineEdit("select id as cid, id as clabel, firstname ||' '|| lastname as ctitle from users", this);
    ui->lCorrector->addWidget(eCorrector);

    eAuthor = new MyLineEdit("select id as cid, '' as clabel, title as ctitle from authors", this);
    ui->lAuthor->addWidget(eAuthor);

    ePublication = new MyLineEdit("select id as cid, '' as clabel, title as ctitle from publications", this);
    ui->lPublication->addWidget(ePublication);

    QWidget::setTabOrder(ui->eTitle, eAuthor);
    QWidget::setTabOrder(eAuthor, ePublication);
    QWidget::setTabOrder(ePublication, eCorrector);

    fillComboBox(ui->cState, MMatches::states());
    fillComboBox(ui->cAgeClass, MMatches::ageclasses());

    on_cType_currentIndexChanged(0);

    // add matchname edit
    eMatch = new MyLineEdit("", this);
    ui->lMatch->addWidget(eMatch);
    connect(eMatch, SIGNAL(select()), this, SLOT(selectMatch()));
    connect(eMatch, SIGNAL(cancel()), this, SLOT(cancelMatch()));

    // init fillerItem for new question
    fillerItem = new QWidget(this);
    QFormLayout* lay = new QFormLayout(fillerItem);
    QPushButton* button = new QPushButton(tr("New Question"), fillerItem);
    connect(button, SIGNAL(clicked()), this, SLOT(bNewQuestion_clicked()));
    lay->addRow("", button);
    fillerItem->setLayout(lay);

    cancelMatch();
    editMode(false);
}

MatchForm::~MatchForm()
{
    delete ui;
}

void MatchForm::editMode(bool edit)
{
    ui->gMatch->setVisible(edit);
    ui->gData->setEnabled(! edit);
}

void MatchForm::selectMatch()
{
    if (! eMatch->value().isEmpty())
    {
        cancelMatch();
        clearQuestions();

        QList<StrPair> questions;
        StrMap match;
        MMatches::get(eMatch->value(), match, questions);

        ui->eTitle->setText(match["title"].toString());
        eCorrector->setText(match["corrector"].toString());
        ui->sScore->setValue(match["score"].toInt());
        ui->cAgeClass->setCurrentIndex(ui->cAgeClass->findData(match["ageclass"]));
        ui->cState->setCurrentIndex(ui->cState->findData(match["current_state"]));

        if (match["category_id"] == "")
        {
            ui->cType->setCurrentIndex(0); // Questions
            ui->cGroup->setCurrentIndex(ui->cGroup->findData(match["kind"]));
            eAuthor->setText(match["author"].toString());
            ePublication->setText(match["publication"].toString());

            // questions
            for (int i = 0; i < questions.size(); i++)
            {
                QuestionModule* module = new QuestionModule(questions.at(i).first, questions.at(i).second, this);
                qModules.append(module);
                module->refresh(true);
                ui->lQuestions->layout()->addWidget(module);
            }
            ui->lQuestions->layout()->addWidget(fillerItem);
        }
        else
        {
            ui->cType->setCurrentIndex(1); // Instructions
            ui->cGroup->setCurrentIndex(ui->cGroup->findData(match["category_id"].toString()));
            ui->eContent->page()->mainFrame()->evaluateJavaScript(QString("$('#wysiwyg').wysiwyg('setContent', '%1');").arg(match["content"].toString()));
        }

        ui->gData->setEnabled(true);
        ui->eTitle->setFocus();
    }
}

void MatchForm::cancelMatch()
{
    eMatch->setQuery("select matches.id as cid, objects.label as clabel, matches.title as ctitle from matches left join objects on matches.resource_id = objects.resource_id");

    ui->eTitle->setText("");
    eCorrector->setText("");
    ui->sScore->setValue(0);
    ui->cAgeClass->setCurrentIndex(0);
    ui->cState->setCurrentIndex(0);

    ui->cGroup->setCurrentIndex(0);
    eAuthor->setText("");
    ePublication->setText("");

    QFile file(getAbsoluteAddress("jwysiwyg.html"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString html = QTextStream(&file).readAll();
        html = html.replace("\"jwysiwyg/", QString("\"%1/jwysiwyg/").arg(QCoreApplication::applicationDirPath()));
        ui->eContent->setHtml(html);
    }

    clearQuestions();

    bNewQuestion_clicked();

    ui->gData->setEnabled(false);

    eMatch->setFocus();
}

void MatchForm::on_cType_currentIndexChanged(int index)
{
    bool intructions = ui->cType->currentIndex() == 0;
    if (intructions)
    {
        ui->cGroup->clear();
        ui->cGroup->addItem(tr("book"), "book");
        ui->cGroup->addItem(tr("multimedia"), "multimedia");
    } else
        fillComboBox(ui->cGroup, MMatches::categories());

    ui->gResource->setVisible(intructions);
    ui->sQuestions->setVisible(intructions);
    ui->eContent->setVisible(! intructions);
}

void MatchForm::bNewQuestion_clicked()
{
    // check for last question for
    if (qModules.size() >= 1 && qModules.at(qModules.size()-1)->question() == "")
        qModules.at(qModules.size()-1)->select();
    else // create new one
    {
        QLayout* lay = ui->lQuestions->layout();

        // remove filler widget
        if (lay->count() > 0)
        {
            QLayoutItem* last = lay->takeAt(lay->count()-1);
            lay->removeItem(last);
        }

        // collapse all questions
        for (int i = 0; i < qModules.size(); i++)
            qModules.at(i)->refresh(true);

        // add new question
        QuestionModule* module = new QuestionModule("", "", this);
        qModules.append(module);
        lay->addWidget(module);
        module->select();

        // add filler
        lay->addWidget(fillerItem);
    }
}
/*
QString refineHtml(QString html)
{
    QWebView* view = new QWebView();
    view->setHtml(html);
    QWebFrame* frame = view->page()->mainFrame();

    QString result;
    QWebElementCollection ps = frame->findAllElements("p");
    foreach (QWebElement p, ps)
        result += QString("<p>%1</p>").arg(p.toInnerXml());

    return result;
}*/

void MatchForm::fillMaps(StrMap& match, QList<StrPair>& questions)
{
    match["title"] = ui->eTitle->text();
    match["corrector_id"] = eCorrector->value();
    match["score"] = ui->sScore->value();
    match["ageclass"] = ui->cAgeClass->itemData(ui->cAgeClass->currentIndex());
    match["current_state"] = ui->cState->itemData(ui->cState->currentIndex());

    if (ui->cType->currentIndex() == 0)
    {
        match["kind"] = ui->cGroup->itemData(ui->cGroup->currentIndex());
        match["author"] = ! eAuthor->value().isEmpty() ? eAuthor->value() : eAuthor->text();
        match["publication"] = ! ePublication->value().isEmpty() ? ePublication->value() : ePublication->text();

        // questions
        for (int i = 0; i < qModules.size(); i++)
            if (qModules.at(i)->question() != "")
                questions.append(qMakePair(qModules.at(i)->question(), qModules.at(i)->answer()));
    }
    else
    {
        match["category_id"] = ui->cGroup->itemData(ui->cGroup->currentIndex());
        QString html = ui->eContent->page()->mainFrame()->evaluateJavaScript("document.getElementById('wysiwyg').value").toString();
        match["content"]  = html;
    }
}

void MatchForm::on_buttonBox_accepted()
{
    StrMap match;
    QList<StrPair> questions;
    fillMaps(match, questions);

    QString msg = MMatches::set(eMatch->value(), match, questions);

    // there isn't any error
    if (msg == "")
        emit closeForm();
    else
        QMessageBox::critical(this, QApplication::tr("Reghaabat"), msg);
}

void MatchForm::on_buttonBox_rejected()
{
    emit closeForm();
}

// questions
void MatchForm::clearQuestions()
{
    qModules.clear();
    QLayoutItem *child;
    while ((child = ui->lQuestions->layout()->takeAt(0)) != 0)
    {
        if (child->widget() != fillerItem)
            delete child->widget();
        delete child;
    }
    ui->lQuestions->show();
}

void MatchForm::on_buttonBox_clicked(QAbstractButton* button)
{
    if (button->text() == ViewerForm::tr("Preview"))
    {
        StrMap match;
        QList<StrPair> questions;
        fillMaps(match, questions);

        ViewerForm* viewer = new ViewerForm((MainWindow*) parent());
        viewer->showMatch(match, questions);
        viewer->exec();
    }
}
