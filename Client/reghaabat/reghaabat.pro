#-------------------------------------------------
#
# Project created by QtCreator 2011-03-27T12:07:13
#
#-------------------------------------------------

QT       += core gui sql network

TARGET = reghaabat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connector.cpp \
    syncer.cpp \
    sender.cpp \
    logindialog.cpp \
    mylineedit.cpp \
    userform.cpp

HEADERS  += mainwindow.h \
    connector.h \
    syncer.h \
    sender.h \
    Jalali.h \
    logindialog.h \
    mylineedit.h \
    AccessToSqlite.h \
    MUsers.h \
    userform.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    userform.ui

TRANSLATIONS = reghaabat_fa.ts
