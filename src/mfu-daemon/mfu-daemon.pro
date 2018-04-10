!include(../../config.pri):error("Could not find config.pri")

QT += core websockets sql
CONFIG += c++14

TEMPLATE = app
TARGET = mfu-daemon
DESTDIR=$${BIN_OUT}

useLibs($$MFU_OUT/lib, mfu-common_lib)
useLibs($$MFU_OUT/lib, database_lib)
useLibs($$RPCLIB_OUT/lib, common_lib server_lib)

SOURCES = \
    database/dbclients.cpp \
    database/dbusers.cpp \
    main.cpp \
    services/clientservice.cpp \
    services/userservice.cpp \
    user.cpp \

HEADERS = \
    database/dbclients.h \
    database/dbusers.h \
    services/clientservice.h \
    services/userservice.h \
    user.h \

RESOURCES += \
    scheme.qrc
