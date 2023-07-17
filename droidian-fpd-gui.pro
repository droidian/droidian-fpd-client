TARGET = droidian-fpd-gui

QT += dbus
QT += quick

SOURCES += \
    src/fpdgui.cpp \
    src/fpdinterface.cpp \
    src/fpdinterfaceqmladapter.cpp

HEADERS += \
    src/fpdinterface.h \
    src/fpdinterfaceqmladapter.h
