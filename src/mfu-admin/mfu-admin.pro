!include(../../config.pri):error("Could not find config.pri")

QT += core websockets qml quick widgets
CONFIG += c++14

TEMPLATE = app
TARGET = mfu-admin
DESTDIR=$${BIN_OUT}

useLibs($$MFU_OUT/lib, mfu-common_lib)
useLibs($$RPCLIB_OUT/lib, common_lib client_lib)

SOURCES += \
    clientsmodel.cpp \
    main.cpp \
    mfuadmin.cpp \

HEADERS += \
    clientsmodel.h \
    mfuadmin.h \

RESOURCES += \
    qml.qrc
