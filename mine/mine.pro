include('../errormining.pri')

TEMPLATE = app
TARGET = ../bin/mine
CONFIG += qt debug_and_release warn_on
QT = core

SOURCES += mine.cpp ProgramOptions.cpp
HEADERS += ProgramOptions.hh

# Internal headers
HEADERS += ProgramOptions.ih

mac {
        CONFIG -= app_bundle
}
