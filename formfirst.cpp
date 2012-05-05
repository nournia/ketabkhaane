#include "formfirst.h"
#include "ui_formfirst.h"

FormFirst::FormFirst(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFirst)
{
    ui->setupUi(this);
}

FormFirst::~FormFirst()
{
    delete ui;
}
