#-------------------------------------------------
#
# Project created by QtCreator 2011-12-28T14:43:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openCV


SOURCES += main.cpp\
        mainwindow.cpp\
	calibratewindow.cpp

HEADERS  += mainwindow.h\
	calibratewindow.h

PKGCONFIG += opencv
CONFIG += link_pkgconfig

INCLUDEPATH      = /usr/share/qwt-6.1.1-svn/include
LIBS        *= -L/usr/share/qwt-6.1.1-svn/lib
LIBS        += -lqwt
QMAKE_CXXFLAGS += -g
#QMAKE_CXXFLAGS += -E
