#-------------------------------------------------
#
# Project created by QtCreator 2019-07-06T15:55:01
#
#-------------------------------------------------

QT      -= gui
QT      += network

TARGET = KanoopPiQt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += KANOOPPIQT_LIBRARY

INCLUDEPATH+=$$(HOME)/src/KanoopCommonQt

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

message(PLATFORM is $(PLATFORM_LIBS))
target.path = /home/spunak/$(PLATFORM_LIBS)

SOURCES += \
        kanooppiqt.cpp \
        pigcommand.cpp \
        pigs.cpp

HEADERS += \
        gpio.h \
        kanooppiqt.h \
        kanooppiqt_global.h  \
        pigcommand.h \
        pigs.h

INSTALLS = target
