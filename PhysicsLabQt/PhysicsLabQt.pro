#-------------------------------------------------
#
# Project created by QtCreator 2018-09-30T10:57:19
#
#-------------------------------------------------

QT       += core gui
QT+= network
QT+= charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhysicsLabQt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    splash.cpp \
    unrealcommunicatorhelper.cpp \
    charts.cpp \
    elecappliancespanel.cpp

HEADERS += \
        mainwindow.h \
    splash.h \
    unrealcommunicatorhelper.h \
    charts.h \
    elecappliancespanel.h

FORMS += \
        mainwindow.ui \
    splash.ui \
    charts.ui \
    elecappliancespanel.ui

LIBS += -luser32
