
TEMPLATE = app

TARGET = worldclock

# QMAKE_CC = ~/bin/colorgcc
# QMAKE_CXX = ~/bin/colorgcc
# QMAKE_LINK = ~/bin/g++

QT += core gui

# CONFIG = += release

HEADERS += MainWindow.h \
	   LocationModel.h \
	   Preferences.h \
	   SwatchCache.h \
	   ZoneSelector.h \
	   ZoneSelectorDelg.h \
	   HereDlog.h \
	   Clock24.h

SOURCES += MainWindow.cpp \
	   LocationModel.cpp \
	   Preferences.cpp \
	   SwatchCache.cpp \
	   ZoneSelector.cpp \
	   ZoneSelectorDelg.cpp \
	   HereDlog.cpp \
	   Clock24.cpp \
           main.cpp

RESOURCES += worldclock.prc
