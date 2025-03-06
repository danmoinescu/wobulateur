QT += core gui

TARGET = Wobulateur
CONFIG += qt \
 warn_on \
 designer \
 build_all \
 release \
 exceptions \
 thread
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui
HEADERS = src/mainwindowimpl.h src/rs232.h \
    src/boutonled.h \
    src/qled.h
SOURCES = src/mainwindowimpl.cpp src/main.cpp src/rs232.cpp \
    src/boutonled.cpp \
    src/qled.cpp
TEMPLATE = app
