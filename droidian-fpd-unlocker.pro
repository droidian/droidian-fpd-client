TARGET = droidian-fpd-unlocker

QT += dbus

SOURCES += \
    src/fpdunlocker.cpp \
    src/fpdinterface.cpp \
    src/journallistener.cpp

HEADERS += \
    src/fpdinterface.h \
    src/journallistener.h

LIBS += -lbatman-wrappers
