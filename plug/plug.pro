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
    mustang.cpp \
    amp_advanced.cpp

HEADERS  += mainwindow.h \
    effect.h \
    amplifier.h \
    mustang.h \
    effects_enum.h \
    amp_advanced.h

FORMS    += mainwindow.ui \
    effect.ui \
    amplifier.ui \
    amp_advanced.ui

LIBS += -lusb-1.0

 target.path += /usr/local/bin
 INSTALLS += target
