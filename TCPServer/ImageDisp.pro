#-------------------------------------------------
#
# Project created by QtCreator 2015-05-02T09:36:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageDisp
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        mytcpsever.cpp \
        mytcpsocket.cpp

HEADERS  += mainwindow.h \
            mytcpsever.h \
            mytcpsocket.h

FORMS    += mainwindow.ui
