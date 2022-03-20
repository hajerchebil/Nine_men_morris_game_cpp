#-------------------------------------------------
#
# Project created by QtCreator 2016-03-10T13:51:14
#
#-------------------------------------------------

QT       += core gui network
CONFIG  += c++11


QMAKE_CXXFLAGS  += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Muehle
TEMPLATE = app


SOURCES += main.cpp\
        muehlegui.cpp \
        network.cpp \
        networkgui.cpp \
        packages.cpp \
    guicontroller.cpp \
    muehlelogik.cpp \
    waitscreen.cpp \
    clickablegraphicsitem.cpp \
    gamescene.cpp

HEADERS  += muehlegui.h \
    network.hpp \
    muehlelogik.h \
    networkgui.h \
    packages.hpp \
    guicontroller.h \
    waitscreen.h \
    clickablegraphicsitem.h \
    gamescene.h


FORMS    += muehlegui.ui \
            networkgui.ui

RESOURCES += \
    Res.qrc



