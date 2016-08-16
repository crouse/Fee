#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T07:13:54
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fee
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    form.cpp

HEADERS  += mainwindow.h \
    form.h

FORMS    += mainwindow.ui \
    form.ui

RESOURCES += \
    files.qrc
