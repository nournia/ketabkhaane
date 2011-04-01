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
    userform.cpp \
    helper.cpp \
    jalali.cpp \
    usermain.cpp \
    matchrow.cpp \
    matchform.cpp \
    questionmodule.cpp

HEADERS  += mainwindow.h \
    connector.h \
    syncer.h \
    sender.h \
    jalali.h \
    logindialog.h \
    mylineedit.h \
    accesstosqlite.h \
    musers.h \
    userform.h \
    helper.h \
    usermain.h \
    matchrow.h \
    mmatches.h \
    matchform.h \
    questionmodule.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    userform.ui \
    usermain.ui \
    matchrow.ui \
    matchform.ui \
    questionmodule.ui

TRANSLATIONS = reghaabat_fa.ts
