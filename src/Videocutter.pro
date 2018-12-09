#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T00:07:22
#
#-------------------------------------------------

VERSION=1.0.0
QMAKE_TARGET_COMPANY = Jan Klass
QMAKE_TARGET_PRODUCT = Videocutter
QMAKE_TARGET_DESCRIPTION = A video cutter for marking, extracting and combining video scenes.
RC_ICONS = app.ico

QT       += core gui multimedia multimediawidgets

#CONFIG += c++11
unix {
    QMAKE_CXXFLAGS += -std=c++11 -Werror
}
win32-g++ {
    QMAKE_CXXFLAGS += -Werror
}
win32-msvc*{
    QMAKE_CXXFLAGS += /WX
    QMAKE_CXXFLAGS_WARN_ON *= -W3
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Videocutter
TEMPLATE = app
CONFIG(release, debug|release) {
	DESTDIR = bin
}


SOURCES += main.cpp\
    appwindow.cpp \
    markings.cpp \
    markwidget.cpp \
    markingswidget.cpp \
    markersmodel.cpp \
    markdelegate.cpp \
    exportprocessor.cpp \
    dialogs/filenamedialog.cpp

HEADERS  += \
    appwindow.h \
    markings.h \
    markwidget.h \
    markingswidget.h \
    markersmodel.h \
    markdelegate.h \
    markcolumns.h \
    exportprocessor.h \
    dialogs/filenamedialog.h

FORMS    += \
    appwindow.ui \
    markingswidget.ui \
    dialogs/filenamedialog.ui

DISTFILES +=

RESOURCES += \
    res.qrc
