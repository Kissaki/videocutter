#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T00:07:22
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

#CONFIG += c++11
unix {
    QMAKE_CXXFLAGS += -std=c++11 -Werror
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Videocutter
TEMPLATE = app
DESTDIR = bin
OBJECTS_DIR = build


SOURCES += main.cpp\
    appwindow.cpp \
    markings.cpp \
    markwidget.cpp \
    markingswidget.cpp \
    markersmodel.cpp \
    markdelegate.cpp \
    exportprocessor.cpp

HEADERS  += \
    appwindow.h \
    markings.h \
    markwidget.h \
    markingswidget.h \
    markersmodel.h \
    markdelegate.h \
    markcolumns.h \
    exportprocessor.h

FORMS    += \
    appwindow.ui

DISTFILES +=

RESOURCES += \
    res.qrc
