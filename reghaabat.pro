#-------------------------------------------------
#
# Project created by QtCreator 2011-03-27T12:07:13
#
#-------------------------------------------------

QT = widgets sql network webkitwidgets

TARGET = reghaabat
TEMPLATE = app
TRANSLATIONS = reghaabat_fa.ts
RESOURCES = resources.qrc
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
    mmatches.cpp \
    usermanagement.cpp \
    payment.cpp \
    json.cpp \
    uihelper.cpp \
    objectform.cpp \
    objectmanagement.cpp \
    mobjects.cpp \
    aboutdialog.cpp \
    webconnection.cpp

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
    matchtable.h \
    usermanagement.h \
    payment.h \
    migrations.h \
    json.h \
    uihelper.h \
    objectform.h \
    objectmanagement.h \
    mobjects.h \
    aboutdialog.h \
    webconnection.h

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
    matchtable.ui \
    usermanagement.ui \
    payment.ui \
    objectform.ui \
    objectmanagement.ui \
    aboutdialog.ui \
    webconnection.ui
