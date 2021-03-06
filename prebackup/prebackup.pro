#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T21:20:47
#
#-------------------------------------------------

QT += core gui widgets
CONFIG += release c++11

TARGET = prebackup
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
	mainwindow.cpp \
	directory.cpp \
	snapshot.cpp \
	itemModelSnapshot.cpp \
	dialogRoots.cpp \
	dialogListChoose.cpp \
	dialogOuputFile.cpp

HEADERS  += mainwindow.h \
	directory.h \
	snapshot.h \
	itemModelSnapshot.h \
	dialogRoots.h \
	dialogListChoose.h \
	dialogOuputFile.h \
	version.h

TRANSLATIONS += tran/prebackup_it.ts

LIBS += -lsqlite3

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
RCC_DIR = .rcc

RESOURCES += \
	prebackup.qrc
