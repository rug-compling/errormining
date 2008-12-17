include('../projects.pri')

TEMPLATE = app
TARGET = ../bin/mine
CONFIG += debug_and_release warn_on
QT =

SOURCES += mine.cpp

mac {
        CONFIG -= app_bundle
}
