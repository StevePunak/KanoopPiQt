#-------------------------------------------------
#
# Project created by QtCreator 2019-07-06T15:55:01
#
#-------------------------------------------------

QT      -= gui
QT      += network

TARGET = KanoopPiQt
TEMPLATE = lib
#CONFIG += staticlib

DEFINES += KANOOPPIQT_LIBRARY

INCLUDEPATH+=$$(HOME)/src/KanoopCommonQt

# Sysroot /usr/include
INCLUDEPATH+= $$[QT_SYSROOT]/usr/include

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -Wl,-init,init -Wl,-fini,deinit

unix:
contains(CONFIG, cross_compile):{
        message("building for PI")
        target.path = /${HOME}/lib/arm
    }else{
        message("Not building for PI")
        target.path = /${HOME}/lib/x86
    }

SOURCES += \
        devices/ads1115.cpp \
        devices/bmp280.cpp \
        gpioreader.cpp \
        i2c.cpp \
        kanooppiqt.cpp \
        pigcommand.cpp \
        pigs.cpp

HEADERS += \
        devices/ads1115.h \
        devices/bmp280.h \
        gpio.h \
        gpioreader.h \
        i2c.h \
        kanooppiqt.h \
        kanooppiqt_global.h  \
        pigcommand.h \
        pigs.h

INSTALLS = target

DISTFILES += \
    deploy.sh
