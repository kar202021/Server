QT += core gui network widgets
TARGET = Server
CONFIG += c++11
TEMPLATE = app
SOURCES += main.cpp \
           business/serverlogic.cpp \
           ui/serverwindow.cpp
HEADERS += \
    business/serverlogic.h \
    ui/serverwindow.h
FORMS += \
    ui/serverwindow.ui
