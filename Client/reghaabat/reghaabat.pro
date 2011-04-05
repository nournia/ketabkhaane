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
    formoperator.cpp \
    matchrow.cpp \
    matchform.cpp \
    questionmodule.cpp \
    optionsform.cpp \
    formfirst.cpp \
    dialogchangepassword.cpp \
    formchangepermissions.cpp

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
    formoperator.h \
    matchrow.h \
    mmatches.h \
    matchform.h \
    questionmodule.h \
    optionsform.h \
    formfirst.h \
    dialogchangepassword.h \
    formchangepermissions.h \
    delegatecombobox.h \
    delegatepassword.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    userform.ui \
    formoperator.ui \
    matchrow.ui \
    matchform.ui \
    questionmodule.ui \
    optionsform.ui \
    formfirst.ui \
    dialogchangepassword.ui \
    formchangepermissions.ui

TRANSLATIONS = reghaabat_fa.ts
