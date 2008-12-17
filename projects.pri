QMAKE_CXXFLAGS += -Wextra -I../libmine
unix:LIBS += -L../lib -lmine

mac {
	CONFIG -= app_bundle
}
