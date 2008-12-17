include('../projects.pri')

TEMPLATE = app
TARGET = ../bin/miningviewer
CONFIG += qt debug_and_release warn_on
QT += sql

HEADERS += FormTreeWidgetItem.hh MinerMainWindow.hh
FORMS += MinerMainWindow.ui
SOURCES += FormTreeWidgetItem.cpp MinerMainWindow.cpp miningviewer.cpp
