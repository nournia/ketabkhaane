#-------------------------------------------------
#
# Project created by QtCreator 2011-03-27T12:07:13
#
#-------------------------------------------------

QT       += core sql gui network webkit

TARGET = reghaabat
TEMPLATE = app
TRANSLATIONS = reghaabat_fa.ts
RESOURCES = reghaabat.qrc
RC_FILE = reghaabat.rc


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
    formchangepermissions.cpp \
    viewerform.cpp \
    scoreform.cpp \
    matchtable.cpp \
    musers.cpp \
    mmatches.cpp

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
    comboboxdelegate.h \
    lineeditdelegate.h \
    spinboxdelegate.h \
    viewerform.h \
    scoreform.h \
    matchtable.h

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
    formchangepermissions.ui \
    viewerform.ui \
    scoreform.ui \
    matchtable.ui
