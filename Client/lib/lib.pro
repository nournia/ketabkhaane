#-------------------------------------------------
#
# Project created by QtCreator 2011-02-24T08:35:42
#
#-------------------------------------------------

QT       += core \
            sql \
            network

QT       -= gui

TARGET = lib
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    syncer.cpp \
    sender.cpp \
    connector.cpp

HEADERS += \
    AccessToSqlite.h \
    Jalali.h \
    syncer.h \
    sender.h \
    connector.h
