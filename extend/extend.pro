include('../errormining.pri')

TEMPLATE = app
TARGET = ../bin/extend
CONFIG += qt debug_and_release warn_on
QT = core sql

SOURCES += extend.cpp
