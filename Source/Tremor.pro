#-------------------------------------------------
#
# Project created by QtCreator 2011-12-28T14:43:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tremor


SOURCES += main.cpp\
        mainwindow.cpp\
	calibratewindow.cpp \
    trmmosbat.cpp

HEADERS  += mainwindow.h\
	calibratewindow.h \
    trmmosbat.h

PKGCONFIG += opencv jsoncpp
CONFIG += link_pkgconfig

INCLUDEPATH      = /usr/include/qwt
LIBS        += -lqwt -lm
QMAKE_CXXFLAGS += -g
#QMAKE_CXXFLAGS += -E
