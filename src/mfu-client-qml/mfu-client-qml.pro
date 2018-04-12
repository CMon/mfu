!include(../../config.pri):error("Could not find config.pri")

QT += core websockets qml
CONFIG += c++14

TEMPLATE = app
TARGET = mfu-client-qml
DESTDIR=$${BIN_OUT}

useLibs($$MFU_OUT/lib, mfu-common_lib)
useLibs($$RPCLIB_OUT/lib, common_lib client_lib)

HEADERS += \
    mfu-client.h

SOURCES += \
    main.cpp \
    mfu-client.cpp

target.path = $${TARGET_PREFIX}"/bin/"

INSTALLS += \
    target \
