include('../projects.pri')

TEMPLATE = app
TARGET = ../bin/miningeval
CONFIG += qt debug_and_release warn_on
QT = core sql

SOURCES += miningeval.cpp
