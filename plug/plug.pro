#-------------------------------------------------
#
# Project created by QtCreator 2010-12-13T13:30:06
#
#-------------------------------------------------

QT       += core gui

TARGET = plug
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    effect.cpp \
    amplifier.cpp \
    mustang.cpp

HEADERS  += mainwindow.h \
    effect.h \
    amplifier.h \
    mustang.h \
    effects_enum.h

FORMS    += mainwindow.ui \
    effect.ui \
    amplifier.ui

LIBS += -lusb-1.0
