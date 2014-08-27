#-------------------------------------------------
#
# Project created by QtCreator 2014-08-06T08:19:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = omocac
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsqlite3

SOURCES += main.cpp\
        mainwindow.cpp \
    individual.cpp \
    normativephenotypicpart.cpp \
    normativegrid.cpp \
    externalfile.cpp \
    simulation.cpp \
    gridsubinterval.cpp \
    mutation.cpp \
    selection.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    individual.h \
    normativephenotypicpart.h \
    normativegrid.h \
    externalfile.h \
    simulation.h \
    gridsubinterval.h \
    mutation.h \
    selection.h \
    qcustomplot.h

FORMS    += mainwindow.ui
