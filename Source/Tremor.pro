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
    trmmark.cpp \
    recwindow.cpp \
    calibrateiiwindow.cpp \
    Track.cpp

HEADERS  += mainwindow.h\
	calibratewindow.h \
    trmmark.h \
    recwindow.h \
    calibrateiiwindow.h \
    Track.h

PKGCONFIG += opencv jsoncpp
CONFIG += link_pkgconfig

INCLUDEPATH      += /usr/include/qwt
LIBS        += -lqwt -lm -lpthread
QMAKE_CXXFLAGS += -g
#QMAKE_CXXFLAGS += -E
