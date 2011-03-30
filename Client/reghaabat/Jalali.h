#ifndef JALALI_H
#define JALALI_H

// Qt Specification
#include <QDate>
#include <QVariant>

QDate toGregorian(QString input); // 1370/03/11

QVariant getGregorianVariant(QString jalali);

QString toJalali(QDate input);

#endif JALALI_H
