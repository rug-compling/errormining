QMAKE_CXXFLAGS += -O2 -Wall -Wextra -I../libmine
QMAKE_LFLAGS += -O2
unix:LIBS += -L../lib -lmine

mac {
	CONFIG -= app_bundle
}
