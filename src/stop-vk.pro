#-------------------------------------------------
#
# Project created by QtCreator 2014-09-02T19:34:37
#
#-------------------------------------------------

QT       += network core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stop-vk
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
        user.cpp

HEADERS  += dialog.h \
    user.h \
    dialog.h

FORMS    += dialog.ui
