#-------------------------------------------------
#
# Project created by QtCreator 2016-09-14T12:20:01
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connect.cpp

HEADERS  += mainwindow.h \
    connect.h

FORMS    += mainwindow.ui
RC_FILE = icon.rc
