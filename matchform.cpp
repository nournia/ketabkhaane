#include "matchform.h"
#include "ui_matchform.h"

#include <QMessageBox>
#include <QFormLayout>
#include <QPushButton>
#include <QFile>
#include <QFileDialog>
#include <QWebElementCollection>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>

#include <uihelper.h>
#include <viewerform.h>
#include <mainwindow.h>
#include <mobjects.h>

MatchForm::MatchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MatchForm)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(QIcon(":/images/preview.png"), ViewerForm::tr("Preview")), QDialogButtonBox::ResetRole);

    ui->eObject->setQuery(MObjects::getObjectsQuery());
    ui->eCorrector->setQuery("select users.id as cid, label as clabel, firstname||' '||lastname as ctitle from users inner join permissions on users.id = permissions.user_id");
    ui->eMatch->setQuery("select matches.id as cid, ifnull(belongs.label, categories.title) as clabel, matches.title as ctitle "
                         "from matches left join belongs on matches.object_id = belongs.object_id left join categories on matches.category_id = categories.id");

    fillComboBox(ui->cState, MMatches::states());
    fillComboBox(ui->cAgeClass, MMatches::ageclasses());
    fillComboBox(ui->cGroup, MMatches::categories());

    on_cType_currentIndexChanged(0);

    // add matchname edit
    connect(ui->eMatch, SIGNAL(select()), this, SLOT(selectMatch()));
    connect(ui->eMatch, SIGNAL(cancel()), this, SLOT(cancelMatch()));

    connect(ui->eObject, SIGNAL(select()), this, SLOT(selectObject()));
    connect(ui->eObject, SIGNAL(cancel()), this, SLOT(cancelObject()));

    // init fillerItem for new question
    fillerItem = new QWidget(this);
    QFormLayout* lay = new QFormLayout(fillerItem);
    QPushButton* button = new QPushButton(QIcon(":/images/star.png"), tr("New Question"), fillerItem);
    button->setMaximumWidth(80);
    connect(button, SIGNAL(clicked()), this, SLOT(bNewQuestion_clicked()));
    lay->addRow("", button);
    fillerItem->setLayout(lay);


    // init editor
    QFile file(getAbsoluteAddress("jwysiwyg.html"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString html = QTextStream(&file).readAll();
        html = html.replace("\"jwysiwyg/", QString("\"file:///%1/jwysiwyg/").arg(QCoreApplication::applicationDirPath().replace("\"", "/")));
        ui->eContent->setHtml(html);
        ui->eContent->page()->mainFrame()->addToJavaScriptWindowObject("containerForm", this);
    } else
        qDebug() << "missed" << getAbsoluteAddress("jwysiwyg.html");

    cancelMatch();
}

MatchForm::~MatchForm()
{
    delete ui;
}

void MatchForm::editMode(bool edit)
{
    if (ui->eMatch->text().isEmpty())
        cancelMatch();
    else
        ui->eMatch->setText("");

    ui->gMatch->setVisible(edit);
    ui->gData->setEnabled(! edit);

    if (edit) {
        if (!Reghaabat::instance()->tmpId.isEmpty())
            ui->eMatch->selectValue(Reghaabat::instance()->tmpId);
        else
            ui->eMatch->setFocus();

        Reghaabat::instance()->tmpId = "";
    }
    checkReadOnly();
}

void MatchForm::selectMatch()
{
    if (! ui->eMatch->value().isEmpty())
    {
        cancelMatch();
        clearQuestions();

        QList<StrPair> questions;
        StrMap match;
        MMatches::get(ui->eMatch->value(), match, questions);

        ui->eObject->setText(match["object"].toString());
        ui->eObject->setValue(match["object_id"].toString());
        ui->eTitle->setText(match["title"].toString());
        ui->eCorrector->setText(match["corrector"].toString());
        ui->eCorrector->setValue(match["corrector_id"].toString());
        ui->sScore->setValue(match["score"].toInt());
        ui->cAgeClass->setCurrentIndex(ui->cAgeClass->findData(match["ageclass"]));
        ui->cState->setCurrentIndex(ui->cState->findData(match["current_state"]));

        if (match["category_id"] == "")
        {
            ui->cType->setCurrentIndex(0); // Questions

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
    checkReadOnly();
}

void MatchForm::cancelMatch()
{
    ui->eTitle->setText("");
    ui->eObject->setText("");
    ui->eCorrector->setText("");
    ui->sScore->setValue(0);
    ui->cAgeClass->setCurrentIndex(0);
    ui->cState->setCurrentIndex(0);

    ui->cGroup->setCurrentIndex(0);

    ui->eContent->page()->mainFrame()->evaluateJavaScript("emptyEditor();");

    clearQuestions();

    bNewQuestion_clicked();

    ui->gData->setEnabled(false);

    ui->eMatch->setFocus();
}

void MatchForm::checkReadOnly()
{
    bool original = true;
    if (! ui->eMatch->value().isEmpty())
        original = ui->eMatch->value().startsWith(Reghaabat::instance()->libraryId);

    ui->cType->setEnabled(original);
    ui->cAgeClass->setEnabled(original);
    ui->cGroup->setEnabled(original);
    ui->eTitle->setEnabled(original);
    ui->eObject->setEnabled(original);
    ui->eContent->setEnabled(original);
    ui->sQuestions->setEnabled(original);
}

void MatchForm::on_cType_currentIndexChanged(int index)
{
    bool intructions = ui->cType->currentIndex() == 0;

    ui->gObject->setVisible(intructions);
    ui->cGroup->setVisible(! intructions);
    ui->lGroup->setVisible(! intructions);
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
    match["corrector_id"] = ui->eCorrector->value();
    match["object_id"] = ui->eObject->value();
    match["score"] = ui->sScore->value();
    match["ageclass"] = ui->cAgeClass->itemData(ui->cAgeClass->currentIndex());
    match["current_state"] = ui->cState->itemData(ui->cState->currentIndex());

    if (ui->cType->currentIndex() == 0)
    {
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

    QString msg = MMatches::set(ui->eMatch->value(), match, questions);

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

QString MatchForm::getFilename()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select Image File"));
    if (!filename.isEmpty())
        return QString("file:///%1").arg(filename);
    return "";
}

void MatchForm::selectObject()
{
    ui->eTitle->setText(ui->eObject->text());
}

void MatchForm::cancelObject()
{
    ui->eTitle->setText("");
}
