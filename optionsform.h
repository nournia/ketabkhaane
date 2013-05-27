#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QWidget>
#include <treemodel.h>


namespace Ui {
    class OptionsForm;
}

class OptionsForm : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsForm(QWidget *parent = 0);
    ~OptionsForm();
    QString libraryLogo, newLibraryLogo;

    TreeModel* model;

private:
    Ui::OptionsForm *ui;

signals:
    void closeForm();

private slots:
    void updateSelectedBranch();
    void dataChanged(const QModelIndex& index);

    void on_bLibraryLogo_clicked();
    void on_bSelectDataFolder_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_bAddBranch_clicked();
    void on_bRemoveBranch_clicked();
    void on_bImportBookList_clicked();
};

#endif // OPTIONSFORM_H
