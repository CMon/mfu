!include(../../config.pri):error("Could not find config.pri")

QT += core
TEMPLATE = lib
TARGET = mfu-common_lib
DESTDIR=$${LIB_OUT}
CONFIG += c++14

useLibs($$RPCLIB_OUT/lib, common_lib)

HEADERS += \
    clientdataresponse.h \
    clientdata.h \
    util.h

SOURCES += \
    clientdataresponse.cpp \
    clientdata.cpp \
    util.cpp

target.path = $${TARGET_PREFIX}"/lib/"

INSTALLS += \
    target \
