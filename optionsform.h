#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QWidget>

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

private:
    Ui::OptionsForm *ui;

signals:
    void closeForm();

private slots:
    void on_bLibraryLogo_clicked();
    void on_bSelectDataFolder_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // OPTIONSFORM_H
