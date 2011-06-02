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

private:
    Ui::OptionsForm *ui;

signals:
    void closeForm();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_bConvertDb_clicked();
};

#endif // OPTIONSFORM_H
