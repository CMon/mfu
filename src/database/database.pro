!include(../../config.pri):error("Could not find config.pri")

QT += core sql
TEMPLATE = lib
TARGET = database_lib
DESTDIR=$${LIB_OUT}
CONFIG += c++14

HEADERS += \
    database.h \
    database_p.h \
    schemeupdater.h

SOURCES += \
    database.cpp \
    database_p.cpp \
    schemeupdater.cpp

target.path = $${TARGET_PREFIX}"/lib/"

INSTALLS += \
    target \
