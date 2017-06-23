#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T09:49:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Robots
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp \
    highlighter.cpp \
    editor.cpp \
    numberer.cpp \
    interpreter.cpp \
    syntaxcheck.cpp \
    result.cpp \
    serialport.cpp \
    iprinter.cpp \
    bauddialog.cpp \
    senddialog.cpp \
    logger.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    console.h \
    highlighter.h \
    editor.h \
    numberer.h \
    interpreter.h \
    syntaxcheck.h \
    result.h \
    serialport.h \
    iprinter.h \
    qasyncqueue.h \
    bauddialog.h \
    senddialog.h \
    logger.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    console.ui \
    bauddialog.ui \
    senddialog.ui
QT += serialport
QT +=gui
RESOURCES += style.qrc
