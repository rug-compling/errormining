include('../errormining.pri')

TEMPLATE = app
TARGET = ../bin/createminedb
CONFIG += qt debug_and_release warn_on
QT = core sql

SOURCES += createminedb.cpp
