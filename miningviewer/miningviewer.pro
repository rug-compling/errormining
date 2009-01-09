include('../errormining.pri')
TEMPLATE = app
TARGET = ../bin/miningviewer
CONFIG += qt \
    debug_and_release \
    warn_on
QT += sql
HEADERS += FormTreeWidgetItem.hh \
    MinerMainWindow.hh \
    PreferencesDialog.hh \
    global.hh
FORMS += MinerMainWindow.ui \
    PreferencesDialog.ui
SOURCES += FormTreeWidgetItem.cpp \
    MinerMainWindow.cpp \
    PreferencesDialog.cpp \
    miningviewer.cpp

# Internal headers
HEADERS += MinerMainWindow.ih \
    PreferencesDialog.ih
