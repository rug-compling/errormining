include('../projects.pri')

TEMPLATE = app
TARGET = ../bin/mine
CONFIG += qt debug_and_release warn_on
QT = core

SOURCES += mine.cpp

mac {
        CONFIG -= app_bundle
}
